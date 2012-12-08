/*
 File:       iccCalcBPCInfo.cpp
 
 Contains:   Console app to calculate and display black point
             compensation information
 
 Version:    V1
 
 Copyright:  © see below
 */

/*
 * The ICC Software License, Version 0.2
 *
 *
 * Copyright (c) 2003-2011 The International Color Consortium. All rights 
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
 * 3. In the absence of prior written permission, the names "ICC" and "The
 *    International Color Consortium" must not be used to imply that the
 *    ICC organization endorses or promotes products derived from this
 *    software.
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
// -Initial implementation by Max Derhak Aug-24-2011
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "IccProfile.h"
#include "IccCmm.h"
#include "IccApplyBPC.h"
#include "IccUtil.h"

struct BPCInfo
{
  icFloatNumber blackPoint[3];
  icFloatNumber scale[3];
  icFloatNumber offset[3];
};

BPCInfo srcBPC, dstBPC;

/**
**************************************************************************
* Type: Class
* 
* Purpose: 
*		Interface and hint for creating a BPC xform object
**************************************************************************
*/
class CMyIccApplyBPCHint : public  CIccApplyBPCHint
{
public:
  CMyIccApplyBPCHint(BPCInfo *pBPCInfo);
  
  virtual IIccAdjustPCSXform* GetNewAdjustPCSXform() const;
protected:
  BPCInfo *m_pBPCInfo;
};

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: This is the hint for applying black point compensation.
 *                Also does the calculations to setup actual application of BPC.
 * 
 *                Stores results in BPCInfo structure.
 * 
 **************************************************************************
*/
class CMyIccApplyBPC : public CIccApplyBPC
{
public:
  CMyIccApplyBPC(BPCInfo *pBPCInfo);

  virtual bool CalcFactors(const CIccProfile* pProfile, const CIccXform* pXfm, icFloatNumber* Scale, icFloatNumber* Offset) const;

protected:
  virtual bool calcBlackPoint(const CIccProfile* pProfile, const CIccXform* pXform, icFloatNumber* XYZb) const;
  
  BPCInfo *m_pBPCInfo;
};

//Constructor for Hint Creator
CMyIccApplyBPCHint::CMyIccApplyBPCHint(BPCInfo *pBPCInfo)
{
  m_pBPCInfo = pBPCInfo;
}

IIccAdjustPCSXform* CMyIccApplyBPCHint::GetNewAdjustPCSXform() const
{
  return new CMyIccApplyBPC(m_pBPCInfo);
}

//Constructor for BPC offset calculator
CMyIccApplyBPC::CMyIccApplyBPC(BPCInfo *pBPCInfo)
{
  m_pBPCInfo = pBPCInfo;
}

bool CMyIccApplyBPC::CalcFactors(const CIccProfile* pProfile, const CIccXform* pXfm, icFloatNumber* Scale, icFloatNumber* Offset) const
{
  bool rv = CIccApplyBPC::CalcFactors(pProfile, pXfm, Scale, Offset);
  
  if (rv) {
    memcpy(m_pBPCInfo->scale, Scale, 3*sizeof(icFloatNumber));
    memcpy(m_pBPCInfo->offset, Offset, 3*sizeof(icFloatNumber));
    icXyzFromPcs(m_pBPCInfo->offset);
  }
  else {
    m_pBPCInfo->scale[0] = -1;
    m_pBPCInfo->scale[1] = -1;
    m_pBPCInfo->scale[2] = -1;

    m_pBPCInfo->offset[0] = 0;
    m_pBPCInfo->offset[1] = 0;
    m_pBPCInfo->offset[2] = 0;
  }

  return rv;
}

bool CMyIccApplyBPC::calcBlackPoint(const CIccProfile* pProfile, const CIccXform* pXform, icFloatNumber* XYZb) const
{
  bool rv = CIccApplyBPC::calcBlackPoint(pProfile, pXform, XYZb);
  
  if (rv)
    memcpy(m_pBPCInfo->blackPoint, XYZb, 3*sizeof(icFloatNumber));
  else {
    m_pBPCInfo->blackPoint[0] = -1;
    m_pBPCInfo->blackPoint[1] = -1;
    m_pBPCInfo->blackPoint[2] = -1;
  }
  
  return rv;
}

int main(int argc, char* argv[])
{
	CIccProfile *pSrcIcc = NULL;
	CIccProfile *pDstIcc = NULL;
	CIccCreateXformHintManager srcHintManager;
	CIccCreateXformHintManager dstHintManager;
  icRenderingIntent nIntent = icRelativeColorimetric;
  icXformInterp nInterp = icInterpLinear;
	
	if (argc<=2) {
    printf("Usage: IccCalcBPCInfo path_src_profile path_dst_profile {intent {interp}}\n");
    printf("\nWhere:\n");
    printf(" intent = 0 (perceptual), 1 (relative), 2 (saturation)\n");
    printf(" interp = 0 (linear), 1 (tetrahedral)\n");

		return -1;
	}
	
	pSrcIcc = OpenIccProfile(argv[1]);
	pDstIcc = OpenIccProfile(argv[2]);

  if (argc>3) {
    nIntent = (icRenderingIntent)atoi(argv[3]);
  }

  if (argc>4) {
    nInterp = (icXformInterp)atoi(argv[4]);
  }

	if (!pSrcIcc) {
		printf("Unable to parse source profile '%s'\n", argv[1]);
		return -1;
	}
	
	if (!pDstIcc) {
		printf("Unable to parse destination profile '%s'\n", argv[2]);
		return -1;
	}
	
  srcBPC.blackPoint[0] = -2;
  srcBPC.scale[0] = -2;
  dstBPC.blackPoint[0] = -2;
  dstBPC.scale[0] = -2;

	srcHintManager.AddHint(new CMyIccApplyBPCHint(&srcBPC));	
	dstHintManager.AddHint(new CMyIccApplyBPCHint(&dstBPC));

  CIccCmm cmm(pSrcIcc->m_Header.colorSpace, pDstIcc->m_Header.colorSpace);

  if (cmm.AddXform(pSrcIcc, nIntent, nInterp, icXformLutColor, true, &srcHintManager)!=icCmmStatOk) {
    printf("Unable to attach profile '%s' to cmm\n", argv[1]);
    return -1;
  }

  if (cmm.AddXform(pDstIcc, nIntent, nInterp, icXformLutColor, true, &dstHintManager)!=icCmmStatOk) {
    printf("Unable to attach profile '%s' to cmm\n", argv[2]);
    return -1;
  }

  if (cmm.Begin()!=icCmmStatOk) {
    printf("Unable to begin cmm operation");
    return -1;
  }

  icFloatNumber srcLab[3], dstLab[3];
  CIccInfo info;

  printf("BPC calculation results:\n\n");
  printf("Rendering intent = %s\n", info.GetRenderingIntentName(nIntent));
  printf("Interpolation = %s\n\n", nInterp==icInterpLinear ? "Linear" : "Tetrahedral");

  printf("Source Profile = '%s'\n", argv[1]);

  if (srcBPC.blackPoint[0]>=0.0) {
    icXYZtoLab(srcLab, srcBPC.blackPoint);
    printf("Source Black point L=%f, a=%f, b=%f\n", srcLab[0], srcLab[1], srcLab[2]);
  }
  if (srcBPC.scale[0]>=0.0) {
    printf("Source Scale to V4 PRM = XYZ(%f, %f, %f)\n", srcBPC.scale[0], srcBPC.scale[1], srcBPC.scale[2]);
    printf("Source Offset to V4 PRM = XYZ(%f, %f, %f)\n", srcBPC.offset[0], srcBPC.offset[1], srcBPC.offset[2]);
  }

  printf("\n");
  printf("Dest Profile = '%s'\n", argv[2]);

  if (dstBPC.blackPoint[0]>=0.0) {
    icXYZtoLab(dstLab, dstBPC.blackPoint);
    printf("Dest Black point L=%f, a=%f, b=%f\n", dstLab[0], dstLab[1], dstLab[2]);
  }
  if (dstBPC.scale[0]>=0.0) {
    printf("Dest Scale from V4 PRM = XYZ(%f, %f, %f)\n", dstBPC.scale[0], dstBPC.scale[1], dstBPC.scale[2]);
    printf("Dest Offset from V4 PRM = XYZ(%f, %f, %f)\n", dstBPC.offset[0], dstBPC.offset[1], dstBPC.offset[2]);
  }
  icFloatNumber srcPixel[16], dstPixel[16];

  icUInt32Number nSrcSamples = cmm.GetSourceSamples();
  icUInt32Number nDstSamples = cmm.GetDestSamples();
  icUInt32Number i;

  printf("\n");
  //Calculate BPC scaling going directly from src PCS to dest PCS
  icFloatNumber scale = (1.0f - dstBPC.blackPoint[1]) / (1.0f - srcBPC.blackPoint[1]);
  icFloatNumber offset = (1.0f - scale);

  printf("Direct BPC Source to Dest scale = %f\n", scale);
  printf("Direct BPC Source to Dest offset = %f\n\n", offset);

  for (i=0; i<nSrcSamples; i++) {
    srcPixel[i]=0.0;
  }
  if (cmm.Apply(dstPixel, srcPixel)!=icCmmStatOk) {
    printf("Unable to apply cmm to pixel values\n");
    return -1;
  }

  printf("Cmm.Apply of %s", info.GetColorSpaceSigName(cmm.GetSourceSpace()));
  for (i=0; i<nSrcSamples; i++) {
    printf(" %f", srcPixel[i]);
  }
  printf("\nResults in %s", info.GetColorSpaceSigName(cmm.GetDestSpace()));
  for (i=0; i<nDstSamples; i++) {
    printf(" %f", dstPixel[i]);
  }
  printf("\n");
	
  for (i=0; i<nSrcSamples; i++) {
    srcPixel[i]=1.0;
  }
  if (cmm.Apply(dstPixel, srcPixel)!=icCmmStatOk) {
    printf("Unable to apply cmm to pixel values\n");
    return -1;
  }

  printf("\n");

  printf("Cmm.Apply of %s", info.GetColorSpaceSigName(cmm.GetSourceSpace()));
  for (i=0; i<nSrcSamples; i++) {
    printf(" %f", srcPixel[i]);
  }
  printf("\nResults in %s", info.GetColorSpaceSigName(cmm.GetDestSpace()));
  for (i=0; i<nDstSamples; i++) {
    printf(" %f", dstPixel[i]);
  }
  printf("\n");

	return 0;
}

