/** @file
File:       IccTagFactory.cpp

Contains:   Implementation of the CIccTag class and creation factories

Version:    V1

Copyright:  © see ICC Software License
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
* 4. In the absence of prior written permission, the names "ICC" and "The
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
// -Oct 30, 2005
// Added CICCTag Creation using factory support
//
//////////////////////////////////////////////////////////////////////

#include "IccTagXml.h"
#include "IccTagXmlFactory.h"
#include "IccUtil.h"
#include "IccProfile.h"

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

CIccTag* CIccTagXmlFactory::CreateTag(icTagTypeSignature tagSig)
{
  switch(tagSig) {
  case icSigSignatureType:
    return new CIccTagXmlSignature;

  case icSigTextType:
    return new CIccTagXmlText;

  case icSigXYZArrayType:
    return new CIccTagXmlXYZ;

  case icSigUInt8ArrayType:
    return new CIccTagXmlUInt8;

  case icSigUInt16ArrayType:
    return new CIccTagXmlUInt16;

  case icSigUInt32ArrayType:
    return new CIccTagXmlUInt32;

  case icSigUInt64ArrayType:
    return new CIccTagXmlUInt64;

  case icSigS15Fixed16ArrayType:
    return new CIccTagXmlS15Fixed16;

  case icSigU16Fixed16ArrayType:
    return new CIccTagXmlU16Fixed16;

  case icSigCurveType:
    return new CIccTagXmlCurve;

  case icSigMeasurementType:
    return new CIccTagXmlMeasurement;

  case icSigMultiLocalizedUnicodeType:
    return new CIccTagXmlMultiLocalizedUnicode;

  case icSigMultiProcessElementType:
    return new CIccTagXmlMultiProcessElement;

  case icSigParametricCurveType:
    return new CIccTagXmlParametricCurve;

  case icSigLutAtoBType:
    return new CIccTagXmlLutAtoB;

  case icSigLutBtoAType:
    return new CIccTagXmlLutBtoA;

  case icSigLut16Type:
    return new CIccTagXmlLut16;

  case icSigLut8Type:
    return new CIccTagXmlLut8;

  case icSigTextDescriptionType:
    return new CIccTagXmlTextDescription;

  case icSigNamedColor2Type:
    return new CIccTagXmlNamedColor2;

  case icSigChromaticityType:
    return new CIccTagXmlChromaticity;

  case icSigDataType:
    return new CIccTagXmlData;

  case icSigDateTimeType:
    return new CIccTagXmlDateTime;

  case icSigColorantOrderType:
    return new CIccTagXmlColorantOrder;

  case icSigColorantTableType:
    return new CIccTagXmlColorantTable;

  case icSigViewingConditionsType:
    return new CIccTagXmlViewingConditions;

  case icSigProfileSequenceDescType:
    return new CIccTagXmlProfileSeqDesc;

  case icSigResponseCurveSet16Type:
    return new CIccTagXmlResponseCurveSet16;

  case icSigProfileSequceIdType:
    return new CIccTagXmlProfileSequenceId;

  case icSigScreeningType:
  case icSigUcrBgType:
  case icSigCrdInfoType:

  default:
    return new CIccTagXmlUnknown;
  }
}

const icChar* CIccTagXmlFactory::GetTagSigName(icTagSignature tagSig)
{
  return NULL;
}

const icChar* CIccTagXmlFactory::GetTagTypeSigName(icTagTypeSignature tagSig)
{
  return NULL;
}

#ifdef USESAMPLEICCNAMESPACE
}
#endif

