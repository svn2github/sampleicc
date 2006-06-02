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
// -Valication implementation by Max Derhak 4-30-2005
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "IccProfile.h"
#include "ProfileDump.h"
#include "ProfileReportDlg.h"


// CProfileReportDlg dialog

IMPLEMENT_DYNAMIC(CProfileReportDlg, CDialog)
CProfileReportDlg::CProfileReportDlg(CWnd* pParent /*=NULL*/, const char *szProfilePath/*=NULL*/)
  : CDialog(CProfileReportDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CProfileReportDlg)
  //}}AFX_DATA_INIT

  icValidateStatus nStat;

  if (!szProfilePath || !szProfilePath) {
    m_sReport = "Invalid Profile Path\r\n";
    nStat = (icValidateStatus)-1;
  }
  else {
    std::string sReport;
    BeginWaitCursor();
    CIccProfile *pIcc = ValidateIccProfile(szProfilePath, sReport, nStat);
    if (pIcc)
      delete pIcc;
    EndWaitCursor();

    if (sReport.empty())
      sReport = "There is nothing to report\r\n";

    m_sReport = sReport.c_str();
  }

  switch(nStat) {
    case icValidateOK:
      m_sValidateStatus = "Valid Profile";
      break;

    case icValidateWarning:
      m_sValidateStatus = "Validation Warning(s)";
      break;

    case icValidateNonCompliant:
      m_sValidateStatus = "Profile violates ICC Specification";
      break;

    case icValidateCriticalError:
      m_sValidateStatus = "Critical Error - Profile Violates ICC Specification";
      break;

    default:
      m_sValidateStatus = "Unknown Validation Status";
      break;
  }
}

CProfileReportDlg::~CProfileReportDlg()
{
}

void CProfileReportDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProfileReportDlg)
  DDX_Text(pDX, IDC_TAG_SIGNATURE, m_sValidateStatus);
  DDX_Text(pDX, IDC_TAG_DATA, m_sReport);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProfileReportDlg, CDialog)
  //{{AFX_MSG_MAP(CProfileReportDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CProfileReportDlg message handlers
BOOL CProfileReportDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  m_FixedFont.CreatePointFont(80, "Terminal");

  CEdit *pData = (CEdit*)GetDlgItem(IDC_TAG_DATA);

  int start, end;

//  pData->SetFont(&m_FixedFont);
  pData->GetSel(start, end);
  // TODO: Add extra initialization here

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}
