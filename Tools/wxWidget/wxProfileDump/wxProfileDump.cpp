/////////////////////////////////////////////////////////////////////////////
// Name:        wxProfileDump.cpp
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------
#include "IccProfile.h"
#include "IccTag.h"
#include "IccUtil.h"
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "wx/toolbar.h"
#include "wx/config.h"
#include "wx/filename.h"

#if !defined(__WXMSW__)
#include "mondrian.xpm"
#endif

#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/help.xpm"


#include "wxProfileDump.h"

IMPLEMENT_APP(MyApp)

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

MyFrame *frame = (MyFrame *) NULL;
wxList my_children;

// For drawing lines in a canvas
static long xpos = -1;
static long ypos = -1;

static int gs_nFrames = 0;

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

#define ID_VALIDATE_PROFILE 1000
#define ID_TAG_LIST         1001

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
EVT_MENU(MDI_ABOUT, MyFrame::OnAbout)
EVT_MENU(MDI_OPEN_PROFILE, MyFrame::OnOpenProfile)
EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MyFrame::OnOpenProfile)
EVT_MENU(MDI_QUIT, MyFrame::OnQuit)

EVT_CLOSE(MyFrame::OnClose)
EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

// Note that MDI_NEW_WINDOW and MDI_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.
BEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
EVT_MENU(MDI_CHILD_QUIT, MyChild::OnQuit)
EVT_CLOSE(MyChild::OnClose)
EVT_BUTTON(ID_VALIDATE_PROFILE, MyChild::OnValidate)
EVT_LIST_ITEM_ACTIVATED(ID_TAG_LIST, MyChild::OnTagClicked) 
END_EVENT_TABLE()


// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
  // Create the main frame window
  frame = new MyFrame((wxFrame *)NULL, wxID_ANY, _T("ProfileDump"),
    wxDefaultPosition, wxSize(1024, 768),
    wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

  // using wxConfig instead of writing wxFileConfig or wxRegConfig enhances
  // portability of the code
  wxConfig config("wxProfileDump");

  m_history.Load(config);

#ifdef __WXMSW__
#if 0
  // Experimental: change the window menu
  wxMenu* windowMenu = new wxMenu;
  windowMenu->Append(5000, _T("My menu item!"));
  frame->SetWindowMenu(windowMenu);
#endif
#endif

  // Give it an icon
#ifdef __WXMSW__
  frame->SetIcon(wxIcon(_T("wxProfileDump_icn")));
#else
  frame->SetIcon(wxIcon( wxProfileDump_xpm ));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(MDI_OPEN_PROFILE, _T("&Open Profile\tCtrl-O"), _T("Open an ICC profile"));
  file_menu->Append(MDI_QUIT, _T("&Exit\tAlt-X"), _T("Quit the program"));

  m_history.UseMenu(file_menu);
  m_history.AddFilesToMenu(file_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(MDI_ABOUT, _T("&About\tF1"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  menu_bar->Append(help_menu, _T("&Help"));

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
  frame->CreateStatusBar();
#endif // wxUSE_STATUSBAR

  frame->Show(true);
  frame->SetDropTarget(new MyDnDFile(frame));

  SetTopWindow(frame);

  return true;
}

int MyApp::OnExit()
{
  wxConfig config;

  m_history.Save(config);

  return 0;
}

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
                 : wxMDIParentFrame(parent, id, title, pos, size,
                 style | wxNO_FULL_REPAINT_ON_RESIZE)
{
#if wxUSE_TOOLBAR
  CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
  InitToolBar(GetToolBar());
#endif // wxUSE_TOOLBAR

  // Accelerators
  wxAcceleratorEntry entries[3];
  entries[0].Set(wxACCEL_CTRL, (int) 'O', MDI_OPEN_PROFILE);
  entries[1].Set(wxACCEL_CTRL, (int) 'X', MDI_QUIT);
  entries[2].Set(wxACCEL_CTRL, (int) 'A', MDI_ABOUT);
  wxAcceleratorTable accel(3, entries);
  SetAcceleratorTable(accel);
}

void MyFrame::OnClose(wxCloseEvent& event)
{
  if ( event.CanVeto() && (gs_nFrames > 0) )
  {
    wxString msg;
    msg.Printf(_T("%d windows still open, close anyhow?"), gs_nFrames);
    if ( wxMessageBox(msg, _T("Please confirm"),
      wxICON_QUESTION | wxYES_NO) != wxYES )
    {
      event.Veto();

      return;
    }
  }

  event.Skip();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
  (void)wxMessageBox(_T("wxProfileDump\n")
    _T("Copyright (C) 20005\n"),
    _T("About wxProfileDump"));
}

void MyFrame::OpenFile(wxString profilePath)
{
  wxFileName filepath(profilePath);
  wxString profileTitle = filepath.GetName();

  CIccProfile *pIcc = OpenIccProfile(profilePath);

  if (!pIcc) {
    (void)wxMessageBox(wxString(_T("Unable to open profile '")) + profilePath + _T("'"),
      _T("Open Error!"));
    return;
  }

  wxGetApp().m_history.AddFileToHistory(profilePath);

  // Make another frame, containing a canvas
  MyChild *subframe = new MyChild(frame, profileTitle, pIcc, profilePath);

  subframe->SetTitle(profileTitle);

  // Give it an icon
#ifdef __WXMSW__
  subframe->SetIcon(wxIcon(_T("ProfileDumpDoc_icn")));
#else
  subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  subframe->SetFileMenu(file_menu);

  file_menu->Append(MDI_OPEN_PROFILE, _T("&Open Profile"));
  file_menu->Append(MDI_CHILD_QUIT, _T("&Close"), _T("Close this window"));
  file_menu->Append(MDI_QUIT, _T("&Exit"));

  wxGetApp().m_history.UseMenu(file_menu);
  wxGetApp().m_history.AddFilesToMenu(file_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(MDI_ABOUT, _T("&About"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  menu_bar->Append(help_menu, _T("&Help"));

  // Associate the menu bar with the frame
  subframe->SetMenuBar(menu_bar);
  subframe->Show(true);
}

void MyFrame::OnOpenProfile(wxCommandEvent& event )
{
  wxString profilePath;

  if (event.GetId()==MDI_OPEN_PROFILE) {
    wxFileDialog dialog(this, _T("Open Profile"), wxEmptyString, wxEmptyString, _T("ICC files (*.icc)|*.icc|ICM files (*.icm)|*.icm|All files|*.*"), wxOPEN |wxHIDE_READONLY |wxFILE_MUST_EXIST);

    if (dialog.ShowModal()!=wxID_OK)
      return;

    profilePath = dialog.GetPath();
  }
  else {
    profilePath = wxGetApp().m_history.GetHistoryFile(event.GetId() - wxID_FILE1);
  }

  OpenFile(profilePath);
}


#if wxUSE_TOOLBAR
void MyFrame::InitToolBar(wxToolBar* toolBar)
{
  wxBitmap* bitmaps[6];

  int index=0;
  bitmaps[index++] = new wxBitmap( open_xpm );
  //bitmaps[index++] = new wxBitmap( save_xpm );
  //bitmaps[index++] = new wxBitmap( copy_xpm );
  //bitmaps[index++] = new wxBitmap( cut_xpm );
  //bitmaps[index++] = new wxBitmap( paste_xpm );
  bitmaps[index++] = new wxBitmap( help_xpm );

  int width = 24;
  int currentX = 5;

  index = 0;
  toolBar->AddTool( MDI_OPEN_PROFILE, *(bitmaps[index]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Open Profile"));
  currentX += width + 5;

  //index++;
  //toolBar->AddTool(index+1, *bitmaps[index], wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Save Profile"));
  //currentX += width + 5;

  //toolBar->AddSeparator();

  //index++;
  //toolBar->AddTool(index+1, *bitmaps[index], wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Copy"));
  //currentX += width + 5;

  //index++;
  //toolBar->AddTool(index+1, *bitmaps[index], wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Cut"));
  //currentX += width + 5;

  //index++;
  //toolBar->AddTool(index+1, *bitmaps[index], wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Paste"));
  //currentX += width + 5;

  toolBar->AddSeparator();

  index++;
  toolBar->AddTool( MDI_ABOUT, *bitmaps[index], wxNullBitmap, true, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Help"));

  toolBar->Realize();

  int i;
  for (i = 0; i <= index; i++)
    delete bitmaps[i];
}
#endif // wxUSE_TOOLBAR

// ---------------------------------------------------------------------------
// MyChild
// ---------------------------------------------------------------------------

MyChild::MyChild(wxMDIParentFrame *parent, const wxString& title, CIccProfile *pIcc, const wxString &profilePath)
: wxMDIChildFrame(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
                  wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
  m_pIcc = pIcc;
  m_profilePath = profilePath;

  my_children.Append(this);
  // this should work for MDI frames as well as for normal ones
  SetSizeHints(100, 200);

  // create controls
  m_panel = new wxPanel(this, wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);

  wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

  wxSizer *sizerBox = new wxStaticBoxSizer(new wxStaticBox(m_panel, wxID_ANY, _T("&ProfileHeader")), wxVERTICAL);

  sizerBox->Add(CreateSizerWithText(_("Size:"), &m_textSize), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Suggested CMM:"), &m_textCMM), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Version:"), &m_textVersion), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Type of Profile:"), &m_textType), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Data Color Space:"), &m_textColorSpace), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("PCS Color Space:"), &m_textPCS), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Creation Date:"), &m_textCreationDate), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Primary Platform:"), &m_textPlatform), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Flags:"), &m_textFlags), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Attributes:"), &m_textAttribute), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Rendering Intent:"), &m_textRenderingIntent), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Illuminant:"), &m_textIlluminant), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Creator:"), &m_textCreator), wxSizerFlags().Expand().Border(wxALL, 0));
  sizerBox->Add(CreateSizerWithText(_("Profile ID:"), &m_textProfileID), wxSizerFlags().Expand().Border(wxALL, 0));

  sizerTop->Add(sizerBox, wxSizerFlags().Expand().Border(wxALL, 5));

  wxSizer *sizerBtn = new wxBoxSizer(wxHORIZONTAL);
  sizerBtn->Add(new wxButton(m_panel, ID_VALIDATE_PROFILE, _("&Validate Profile")), wxSizerFlags().Border(wxRIGHT, 5));

  sizerTop->Add(sizerBtn, wxSizerFlags().Right());

  wxSizer *sizerLabel = new wxBoxSizer(wxHORIZONTAL);
  sizerLabel->Add(new wxStaticText(m_panel, wxID_ANY, _("Profile Tags")), wxSizerFlags().Border(wxLEFT, 5));

  sizerTop->Add(sizerLabel, wxSizerFlags().Left());

  m_tagsCtrl = new wxListCtrl(m_panel, ID_TAG_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
  wxSizer *sizerTags = new wxBoxSizer(wxVERTICAL);
  sizerTags->Add(m_tagsCtrl, wxSizerFlags(1).Expand().Border(wxALL, 0));
  sizerTags->SetItemMinSize((size_t)0, 455, 175);
  sizerTop->Add(sizerTags, wxSizerFlags(1).Expand().Border(wxALL, 5));

  m_tagsCtrl->InsertColumn(0, _("Tag ID"), wxLIST_FORMAT_LEFT, 160);
  m_tagsCtrl->InsertColumn(1, _("Tag Type"), wxLIST_FORMAT_LEFT, 160);
  m_tagsCtrl->InsertColumn(2, _("Offset"), wxLIST_FORMAT_RIGHT, 60);
  m_tagsCtrl->InsertColumn(3, _("Size"), wxLIST_FORMAT_RIGHT, 60);

  // don't allow frame to get smaller than what the sizers tell it and also set
  // the initial size as calculated by the sizers
  sizerTop->SetSizeHints( this );

  m_panel->SetSizer(sizerTop);

  icHeader *pHdr = &pIcc->m_Header;
  CIccInfo Fmt;
  char buf[64];
  int n;
  wxString str;

  m_textAttribute->SetLabel(Fmt.GetDeviceAttrName(pHdr->attributes));
  m_textCMM->SetLabel(icGetSig(buf, pHdr->cmmId));
  m_textCreationDate->SetLabel(wxString::Format(_T("%d/%d/%d  %02u:%02u:%02u"),
    pHdr->date.month, pHdr->date.day, pHdr->date.year,
    pHdr->date.hours, pHdr->date.minutes, pHdr->date.seconds));
  m_textCreator->SetLabel(icGetSig(buf, pHdr->creator));
  m_textColorSpace->SetLabel(Fmt.GetColorSpaceSigName(pHdr->colorSpace));
  m_textFlags->SetLabel(Fmt.GetProfileFlagsName(pHdr->flags));
  m_textPCS->SetLabel(Fmt.GetColorSpaceSigName(pHdr->pcs));
  m_textPlatform->SetLabel(Fmt.GetPlatformSigName(pHdr->platform));
  m_textRenderingIntent->SetLabel(Fmt.GetRenderingIntentName((icRenderingIntent)(pHdr->renderingIntent)));
  m_textSize->SetLabel(wxString::Format(_T("%d(0x%x) bytes"), pHdr->size, pHdr->size));
  m_textType->SetLabel(Fmt.GetProfileClassSigName(pHdr->deviceClass));
  m_textVersion->SetLabel(Fmt.GetVersionName(pHdr->version));
  m_textIlluminant->SetLabel(wxString::Format(_T("X=%.4lf, Y=%.4lf, Z=%.4lf"),
    icFtoD(pHdr->illuminant.X),
    icFtoD(pHdr->illuminant.Y),
    icFtoD(pHdr->illuminant.Z)));
  str.Empty();
  for (n=0; n<16; n++) {
    sprintf(buf, "%02x", pHdr->profileID.ID8[n]);
    if (n && !(n%4))
      str += " ";
    str += buf;
  }
  m_textProfileID->SetLabel(str);

  int item;
  TagEntryList::iterator i;

  for (n=0, i=pIcc->m_Tags->begin(); i!=pIcc->m_Tags->end(); i++, n++) {
    item = m_tagsCtrl->InsertItem(n, Fmt.GetTagSigName(i->TagInfo.sig));
    CIccTag *pTag = pIcc->FindTag(i->TagInfo.sig);
    if (!pTag)
      m_tagsCtrl->SetItem(item, 1, _T("***Invalid Tag!***"));
    else
      m_tagsCtrl->SetItem(item, 1, Fmt.GetTagTypeSigName(pTag->GetType()));

    m_tagsCtrl->SetItem(item, 2, wxString::Format("%d", i->TagInfo.offset));

    m_tagsCtrl->SetItem(item, 3, wxString::Format("%d", i->TagInfo.size));

    m_tagsCtrl->SetItemData(item, (long)i->TagInfo.sig);
  }
  m_panel->Layout();
}

MyChild::~MyChild()
{
  my_children.DeleteObject(this);
  if (m_pIcc)
    delete m_pIcc;
}

wxSizer *MyChild::CreateSizerWithText(const wxString &labelText, wxStaticText **ppText)
{
  wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);

  wxSize winSize = wxDefaultSize;

  winSize.SetWidth(100);
  wxStaticText *label = new wxStaticText(m_panel, wxID_ANY, labelText, wxDefaultPosition, winSize, wxALIGN_RIGHT);

  winSize.SetWidth(250);
  wxStaticText *text = new wxStaticText(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, winSize, wxSTATIC_BORDER|wxTE_RIGHT);

  sizerRow->Add(label, 0, wxRIGHT | wxALIGN_CENTRE_VERTICAL, 5);
  sizerRow->Add(text, 1, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);

  if ( ppText )
    *ppText = text;

  return sizerRow;
}

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

void MyChild::SetFileMenu(wxMenu *file_menu)
{
  m_fileMenu = file_menu;
}

void MyChild::OnClose(wxCloseEvent& event)
{
  wxGetApp().m_history.RemoveMenu(m_fileMenu);
  gs_nFrames--;

  event.Skip();
}

void MyChild::OnValidate(wxCommandEvent& WXUNUSED(event))
{
  MyDialog dialog(this, _T("Profile Validation Report"), m_profilePath);

  dialog.ShowModal();
}

void MyChild::OnTagClicked(wxListEvent& event)
{
  icTagSignature tagSig = (icTagSignature)event.GetData();
  CIccTag *pTag = m_pIcc->FindTag(tagSig);

  if (!pTag) {
    CIccInfo Fmt;

    wxString sTagSignature = Fmt.GetTagSigName(tagSig);
    wxMessageBox(wxString(_T("Unable to parse tag '")) + sTagSignature + _T("'"),
      _T("Invalid Tag!"));
  }
  else {
    MyTagDialog dialog(this, m_pIcc, tagSig, pTag);

    dialog.ShowModal();
  }
}

MyDialog::MyDialog(wxWindow *pParent, const wxString& title, wxString &profilePath) : 
wxDialog(pParent, wxID_ANY, title,wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);

  wxSize winSize = wxDefaultSize;

  winSize.SetWidth(100);
  wxStaticText *labelSttus = new wxStaticText(this, wxID_ANY, _T("Validation Status:"), wxDefaultPosition, winSize, wxALIGN_RIGHT);

  winSize.SetWidth(400);
  wxStaticText *textStatus = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, winSize, wxSTATIC_BORDER|wxTE_RIGHT);

  sizerRow->Add(labelSttus, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
  sizerRow->Add(textStatus, 1, wxALL | wxALIGN_CENTRE_VERTICAL, 5);

  sizer->Add(sizerRow, wxSizerFlags().Expand());

  winSize = wxSize(500, 400);
  wxTextCtrl *textReport = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, winSize,
    wxTE_MULTILINE |wxTE_READONLY | wxTE_RICH);

  sizer->Add(textReport, wxSizerFlags(1).Expand()); 

  icValidateStatus nStat;
  wxString theReport, theValidateStatus;

  if (profilePath.IsEmpty()) {
    theReport = "Invalid Profile Path\r\n";
    nStat = (icValidateStatus)-1;
  }
  else {
    std::string sReport;
    wxBeginBusyCursor();
    CIccProfile *pIcc = ValidateIccProfile(profilePath, sReport, nStat);
    if (pIcc)
      delete pIcc;
    wxEndBusyCursor();

    if (sReport.empty())
      sReport = "There is nothing to report\r\n";

    theReport = sReport.c_str();
  }

  switch(nStat) {
    case icValidateOK:
      theValidateStatus = "Valid Profile";
      break;

    case icValidateWarning:
      theValidateStatus = "Validation Warning(s)";
      break;

    case icValidateNonCompliant:
      theValidateStatus = "Profile violates ICC Specification";
      break;

    case icValidateCriticalError:
      theValidateStatus = "Critical Error - Profile Violates ICC Specification";
      break;

    default:
      theValidateStatus = "Unknown Validation Status";
      break;
  }

  textStatus->SetLabel(theValidateStatus);
  textReport->SetLabel(theReport);

  SetSizer(sizer);
  sizer->Fit(this);
}

MyTagDialog::MyTagDialog(wxWindow *pParent, CIccProfile *pIcc /*=NULL*/, icTagSignature sig/*=icMaxEnumTag*/, CIccTag *pTag /*=NULL*/) : 
wxDialog(pParent, wxID_ANY, _T("View Tag"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  m_pIcc = pIcc;
  m_sigTag = sig;
  m_pTag = pTag;

  CIccInfo Fmt;

  wxString sTagSignature = Fmt.GetTagSigName(sig);
  wxString sTagType;
  if (pTag->IsArrayType()) {
    sTagType = _T("Array of ");
  }
  else {
    sTagType.Empty();
  }
  sTagType += Fmt.GetTagTypeSigName(pTag->GetType());

  std::string desc;
  wxBeginBusyCursor();
  pTag->Describe(desc);
  wxEndBusyCursor();

  wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

  wxSizer *sizerRow1 = new wxBoxSizer(wxHORIZONTAL);
  wxSize winSize = wxDefaultSize;

  winSize.SetWidth(100);
  wxStaticText *labelSig = new wxStaticText(this, wxID_ANY, _T("Tag Signature:"), wxDefaultPosition, winSize, wxALIGN_RIGHT);

  winSize.SetWidth(400);
  wxStaticText *textSig = new wxStaticText(this, wxID_ANY, sTagSignature, wxDefaultPosition, winSize, wxSTATIC_BORDER|wxTE_RIGHT);

  sizerRow1->Add(labelSig, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
  sizerRow1->Add(textSig, 1, wxALL | wxALIGN_CENTRE_VERTICAL, 5);

  sizer->Add(sizerRow1, wxSizerFlags().Expand());

  wxSizer *sizerRow2 = new wxBoxSizer(wxHORIZONTAL);
  winSize = wxDefaultSize;

  winSize.SetWidth(100);
  wxStaticText *labelType = new wxStaticText(this, wxID_ANY, _T("Tag Type:"), wxDefaultPosition, winSize, wxALIGN_RIGHT);

  winSize.SetWidth(400);
  wxStaticText *textType = new wxStaticText(this, wxID_ANY, sTagType, wxDefaultPosition, winSize, wxSTATIC_BORDER|wxTE_RIGHT);

  sizerRow2->Add(labelType, 0, wxALL | wxALIGN_CENTRE_VERTICAL, 5);
  sizerRow2->Add(textType, 1, wxALL | wxALIGN_CENTRE_VERTICAL, 5);

  sizer->Add(sizerRow2, wxSizerFlags().Expand());

  winSize = wxSize(500, 400);
  wxTextCtrl *textReport = new wxTextCtrl(this, wxID_ANY, wxString(desc.c_str()), wxDefaultPosition, winSize,
    wxTE_MULTILINE |wxTE_READONLY | wxTE_RICH| wxTE_DONTWRAP);

  sizer->Add(textReport, wxSizerFlags(1).Expand());

  SetSizer(sizer);
  sizer->Fit(this);

}
