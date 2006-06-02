/*
    File:       ProfileDumpView.cpp

    Contains:   implementation of the CProfileDumpView class

    Version:    V1

    Copyright:  © see below
*/

/*
 * The ICC Software License, Version 0.1
 *
 *
 * Copyright (c) 2003-2006 The International Color Consortium. All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        The International Color Consortium (www.color.org)"
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "ICC" and "The International Color Consortium" must
 *    not be used to imply that the ICC organization endorses or
 *    promotes products derived from this software without prior
 *    written permission. For written permission, please see
 *    <http://www.color.org/>.
 *
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNATIONAL COLOR CONSORTIUM OR
 * ITS CONTRIBUTING MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the The International Color Consortium. 
 *
 *
 * Membership in the ICC is encouraged when this software is used for
 * commercial purposes. 
 *
 *  
 * For more information on The International Color Consortium, please
 * see <http://www.color.org/>.
 *  
 * 
 */

////////////////////////////////////////////////////////////////////// 
// HISTORY:
//
// -Initial implementation by Max Derhak 5-15-2003
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProfileDump.h"
#include "ProfileDumpDoc.h"
#include "ProfileDumpView.h"
#include "IccTagDlg.h"
#include "ProfileReportDlg.h"
#include "iccutil.h"
#include ".\profiledumpview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProfileDumpView

IMPLEMENT_DYNCREATE(CProfileDumpView, CFormView)

BEGIN_MESSAGE_MAP(CProfileDumpView, CFormView)
  //{{AFX_MSG_MAP(CProfileDumpView)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_PROFILE_VALIDATE, OnBnClickedProfileValidate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileDumpView construction/destruction

CProfileDumpView::CProfileDumpView()
  : CFormView(CProfileDumpView::IDD)
{
  //{{AFX_DATA_INIT(CProfileDumpView)
  m_sHdrAttributes = _T("");
  m_sHdrCmm = _T("");
  m_sHdrCreationDate = _T("");
  m_sHdrCreator = _T("");
  m_sHdrDataColorspace = _T("");
  m_sHdrFlags = _T("");
  m_sHdrIlluminant = _T("");
  m_sHdrPcsColorSpace = _T("");
  m_sHdrPlatform = _T("");
  m_sHdrRenderingIntent = _T("");
  m_sHdrSize = _T("");
  m_sHdrType = _T("");
  m_sHdrVersion = _T("");
  m_sHdrProfileID = _T("");
  //}}AFX_DATA_INIT
  // TODO: add construction code here

}

CProfileDumpView::~CProfileDumpView()
{
}

void CProfileDumpView::DoDataExchange(CDataExchange* pDX)
{
  CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProfileDumpView)
  DDX_Control(pDX, IDC_TAG_LIST, m_listTags);
  DDX_Text(pDX, IDC_HDR_ATTRIBUTES, m_sHdrAttributes);
  DDX_Text(pDX, IDC_HDR_CMM, m_sHdrCmm);
  DDX_Text(pDX, IDC_HDR_CREATION_DATE, m_sHdrCreationDate);
  DDX_Text(pDX, IDC_HDR_CREATOR, m_sHdrCreator);
  DDX_Text(pDX, IDC_HDR_DATA_COLORSPACE, m_sHdrDataColorspace);
  DDX_Text(pDX, IDC_HDR_FLAGS, m_sHdrFlags);
  DDX_Text(pDX, IDC_HDR_ILLUMINANT, m_sHdrIlluminant);
  DDX_Text(pDX, IDC_HDR_PCS_COLORSPACE, m_sHdrPcsColorSpace);
  DDX_Text(pDX, IDC_HDR_PLATFORM, m_sHdrPlatform);
  DDX_Text(pDX, IDC_HDR_RENDERING_INTENT, m_sHdrRenderingIntent);
  DDX_Text(pDX, IDC_HDR_SIZE, m_sHdrSize);
  DDX_Text(pDX, IDC_HDR_TYPE, m_sHdrType);
  DDX_Text(pDX, IDC_HDR_VERSION, m_sHdrVersion);
  DDX_Text(pDX, IDC_HDR_PROFILEID, m_sHdrProfileID);
  //}}AFX_DATA_MAP
}

BOOL CProfileDumpView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return CFormView::PreCreateWindow(cs);
}

void CProfileDumpView::OnInitialUpdate()
{
  CIccProfile *pIcc = GetDocument()->m_pProfile;
  icHeader *pHdr = &pIcc->m_Header;
  CIccInfo Fmt;
  char buf[64];
  int n;

  m_sHdrAttributes = Fmt.GetDeviceAttrName(pHdr->attributes);
  m_sHdrCmm = icGetSig(buf, pHdr->cmmId);
  m_sHdrCreationDate.Format("%d/%d/%d  %02u:%02u:%02u",
                            pHdr->date.month, pHdr->date.day, pHdr->date.year,
                            pHdr->date.hours, pHdr->date.minutes, pHdr->date.seconds);
  m_sHdrCreator = icGetSig(buf, pHdr->creator);
  m_sHdrDataColorspace = Fmt.GetColorSpaceSigName(pHdr->colorSpace);
  m_sHdrFlags = Fmt.GetProfileFlagsName(pHdr->flags);
  m_sHdrPcsColorSpace = Fmt.GetColorSpaceSigName(pHdr->pcs);
  m_sHdrPlatform = Fmt.GetPlatformSigName(pHdr->platform);
  m_sHdrRenderingIntent = Fmt.GetRenderingIntentName((icRenderingIntent)(pHdr->renderingIntent));
  m_sHdrSize.Format("%d(0x%x) bytes", pHdr->size, pHdr->size);
  m_sHdrType = Fmt.GetProfileClassSigName(pHdr->deviceClass);
  m_sHdrVersion = Fmt.GetVersionName(pHdr->version);
  m_sHdrIlluminant.Format("X=%.4lf, Y=%.4lf, Z=%.4lf",
                          icFtoD(pHdr->illuminant.X),
                          icFtoD(pHdr->illuminant.Y),
                          icFtoD(pHdr->illuminant.Z));
  m_sHdrProfileID.Empty();
  for (n=0; n<16; n++) {
    sprintf(buf, "%02x", pHdr->profileID.ID8[n]);
    if (n && !(n%4))
      m_sHdrProfileID += " ";
    m_sHdrProfileID += buf;
  }

  CFormView::OnInitialUpdate();
  ResizeParentToFit(FALSE);

  m_listTags.InsertColumn(0, "Tag ID", LVCFMT_LEFT, 200);
  m_listTags.InsertColumn(1, "Offset", LVCFMT_RIGHT, 75);
  m_listTags.InsertColumn(2, "Size", LVCFMT_RIGHT, 75);

  int item;
  TagEntryList::iterator i;
  CString Text;

  for (n=0, i=pIcc->m_Tags->begin(); i!=pIcc->m_Tags->end(); i++, n++) {
    item = m_listTags.InsertItem(n, Fmt.GetTagSigName(i->TagInfo.sig));

    Text.Format("%d", i->TagInfo.offset);
    m_listTags.SetItemText(item, 1, Text);

    Text.Format("%d", i->TagInfo.size);
    m_listTags.SetItemText(item, 2, Text);

    m_listTags.SetItemData(item, (DWORD)i->TagInfo.sig);
  }

}

/////////////////////////////////////////////////////////////////////////////
// CProfileDumpView diagnostics

#ifdef _DEBUG
void CProfileDumpView::AssertValid() const
{
  CFormView::AssertValid();
}

void CProfileDumpView::Dump(CDumpContext& dc) const
{
  CFormView::Dump(dc);
}

CProfileDumpDoc* CProfileDumpView::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProfileDumpDoc)));
  return (CProfileDumpDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProfileDumpView message handlers

void CProfileDumpView::ShowTag(icTagSignature sig)
{
  CIccProfile *pIcc = GetDocument()->m_pProfile;

  if (pIcc) {
    CIccTag *pTag = pIcc->FindTag(sig);

    if (pTag) {
      CIccTagDlg Dlg(this, pIcc, sig, pTag);

      Dlg.DoModal();
    }
    else { 
      MessageBox("Unable to open tag", "Invalid Tag!", MB_OK | MB_ICONSTOP);
    }
  }
  else { 
    MessageBox("Unable to open tag", "Invalid Profile!", MB_OK | MB_ICONSTOP);
  }
}

void CProfileDumpView::OnBnClickedProfileValidate()
{
  CProfileReportDlg dlg(this, GetDocument()->m_sProfilePath);

  dlg.DoModal();
}
