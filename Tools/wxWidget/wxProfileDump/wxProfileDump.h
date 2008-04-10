/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: mdi.h,v 1.18 2005/08/11 13:29:28 VZ Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/toolbar.h>
#include <wx/listctrl.h>
#include <wx/docview.h>
#include <wx/dnd.h>

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
    int OnExit();

    wxFileHistory m_history;
};

class MyCanvas : public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual void OnDraw(wxDC& dc);

    bool IsDirty() const { return m_dirty; }

    void OnEvent(wxMouseEvent& event);

private:
    bool m_dirty;

    DECLARE_EVENT_TABLE()
};

// Define a new frame
class MyFrame : public wxMDIParentFrame
{
public:
    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);

    void InitToolBar(wxToolBar* toolBar);

    void OnAbout(wxCommandEvent& event);
    void OnOpenProfile(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    void OpenFile(wxString path);

    DECLARE_EVENT_TABLE()
};

class MyDnDFile : public wxFileDropTarget
{
public:
  MyDnDFile(MyFrame *owner) { m_owner = owner; }

  virtual bool OnDropFiles(wxCoord x, wxCoord y,
                           const wxArrayString& filenames)
  {
     size_t nFiles = filenames.GetCount();
     wxString str;
     for (size_t n=0; n<nFiles; n++) {
       m_owner->OpenFile(filenames[n]);
     }
     return true;
  }
private:
  MyFrame *m_owner;
};

class MyChild: public wxMDIChildFrame
{
public:
    MyChild(wxMDIParentFrame *parent, const wxString& title, CIccProfile *pIcc, const wxString &profilePath);
    ~MyChild();

    void OnActivate(wxActivateEvent& event);

    void OnRefresh(wxCommandEvent& event);
    void OnUpdateRefresh(wxUpdateUIEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
		void OnValidate(wxCommandEvent& event);
    void OnRoundTrip(wxCommandEvent& event);
		void OnTagClicked(wxListEvent& event);

    void SetFileMenu(wxMenu *menu);

protected:
    wxMenu *m_fileMenu;

		CIccProfile *m_pIcc;
		wxString m_profilePath;

		wxPanel *m_panel;
		wxListCtrl *m_tagsCtrl;

		wxStaticText *m_textAttribute;
		wxStaticText *m_textCMM;
		wxStaticText *m_textColorSpace;
		wxStaticText *m_textCreationDate;
		wxStaticText *m_textCreator;
		wxStaticText *m_textFlags;
		wxStaticText *m_textIlluminant;
		wxStaticText *m_textPCS;
		wxStaticText *m_textPlatform;
		wxStaticText *m_textProfileID;
		wxStaticText *m_textRenderingIntent;
		wxStaticText *m_textSize;
		wxStaticText *m_textType;
		wxStaticText *m_textVersion;

		wxSizer *CreateSizerWithText(const wxString &labelText, wxStaticText **ppText);

    DECLARE_EVENT_TABLE()
};

class MyDialog : public wxDialog
{
public:
	MyDialog(wxWindow *pParent, const wxString& title, wxString &profilePath);

	wxString m_profilePath;
};

class MyRoundTripDialog : public wxDialog
{
public:
  MyRoundTripDialog(wxWindow *pParent, const wxString& title, wxString &profilePath, CIccProfile *pIcc);

  wxString m_profilePath;
};

class MyTagDialog : public wxDialog
{
public:
	MyTagDialog(wxWindow *pParent, CIccProfile *pIcc /*=NULL*/, icTagSignature sig/*=icMaxEnumTag*/, CIccTag *pTag /*=NULL*/);

	CIccProfile *m_pIcc;
	icTagSignature m_sigTag;
	CIccTag *m_pTag;

	wxFont m_FixedFont;

};
// menu items ids
enum
{
    MDI_QUIT = wxID_EXIT,
    MDI_OPEN_PROFILE = 101,
    MDI_CHILD_QUIT,
    MDI_ABOUT = wxID_ABOUT
};
