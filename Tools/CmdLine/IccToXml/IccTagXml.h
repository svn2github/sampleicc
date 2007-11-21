/** @file
File:       IccTagXml.h

Contains:   Header for implementation of CIccTagXml class and
creation factories

Version:    V1

Copyright:  © see ICC Software License
*/

/*
* The ICC Software License, Version 0.1
*
*
* Copyright (c) 2005 The International Color Consortium. All rights 
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
// Jan 2007 - Created
//
//////////////////////////////////////////////////////////////////////

#ifndef _ICCTAGXML_H
#define _ICCTAGXML_H
#include "IccTag.h"

const char *icFixXml(char *szDest, const char *szSrc);

class CIccTagXml : public IIccExtensionTag
{
public:
  virtual ~CIccTagXml(void) {}

  virtual const char *GetExtClassName() { return "CIccTagXml"; }

  virtual bool ToXml(std::string &xml, std::string blanks="") = 0;
};

class CIccTagXmlUnknown : public CIccTagUnknown, public CIccTagXml
{
public:
  virtual ~CIccTagXmlUnknown() {}

  virtual const char *GetClassName() { return "CIccTagXmlUnknown"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlText : public CIccTagText, public CIccTagXml
{
public:
  virtual ~CIccTagXmlText() {}

  virtual const char *GetClassName() { return "CIccTagXmlText"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlTextDescription : public CIccTagTextDescription, public CIccTagXml
{
public:
  virtual ~CIccTagXmlTextDescription() {}

  virtual const char *GetClassName() { return "CIccTagXmlTextDescription"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlSignature : public CIccTagSignature, public CIccTagXml
{
public:
  virtual ~CIccTagXmlSignature() {}

  virtual const char *GetClassName() { return "CIccTagXmlSignature"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlNamedColor2 : public CIccTagNamedColor2, public CIccTagXml
{
public:
  virtual ~CIccTagXmlNamedColor2() {}

  virtual const char *GetClassName() { return "CIccTagXmlNamedColor2"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlXYZ : public CIccTagXYZ, public CIccTagXml
{
public:
  virtual ~CIccTagXmlXYZ() {}

  virtual const char *GetClassName() { return "CIccTagXmlXYZ"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlChromaticity : public CIccTagChromaticity, public CIccTagXml
{
public:
  virtual ~CIccTagXmlChromaticity() {}

  virtual const char *GetClassName() { return "CIccTagXmlChromaticity"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

template <class T, icTagTypeSignature Tsig>
class CIccTagXmlFixedNum : public CIccTagFixedNum<T, Tsig>, public CIccTagXml
{
public:
  virtual ~CIccTagXmlFixedNum() {}

  virtual const char *GetClassName();

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

typedef CIccTagXmlFixedNum<icS15Fixed16Number, icSigS15Fixed16ArrayType> CIccTagXmlS15Fixed16;
typedef CIccTagFixedNum<icU16Fixed16Number, icSigU16Fixed16ArrayType> CIccTagXmlU16Fixed16;


template <class T, icTagTypeSignature Tsig>
class CIccTagXmlNum : public CIccTagNum<T, Tsig>, public CIccTagXml
{
public:
  virtual ~CIccTagXmlNum() {}

  virtual const char *GetClassName();

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

typedef CIccTagNum<icUInt8Number, icSigUInt8ArrayType> CIccTagXmlUInt8;
typedef CIccTagNum<icUInt16Number, icSigUInt16ArrayType> CIccTagXmlUInt16;
typedef CIccTagNum<icUInt32Number, icSigUInt32ArrayType> CIccTagXmlUInt32;
typedef CIccTagNum<icUInt64Number, icSigUInt64ArrayType> CIccTagXmlUInt64;

class CIccTagXmlMeasurement : public CIccTagMeasurement, public CIccTagXml
{
public:
  virtual ~CIccTagXmlMeasurement() {}

  virtual const char *GetClassName() { return "CIccTagXmlMeasurement"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlMultiLocalizedUnicode : public CIccTagMultiLocalizedUnicode, public CIccTagXml
{
public:
  virtual ~CIccTagXmlMultiLocalizedUnicode() {}

  virtual const char *GetClassName() { return "CIccTagXmlMultiLocalizedUnicode"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlData : public CIccTagData, public CIccTagXml
{
public:
  virtual ~CIccTagXmlData() {}

  virtual const char *GetClassName() { return "CIccTagXmlData"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlDateTime : public CIccTagDateTime, public CIccTagXml
{
public:
  virtual ~CIccTagXmlDateTime() {}

  virtual const char *GetClassName() { return "CIccTagXmlDateTime"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlColorantOrder : public CIccTagColorantOrder, public CIccTagXml
{
public:
  virtual ~CIccTagXmlColorantOrder() {}

  virtual const char *GetClassName() { return "CIccTagXmlColorantOrder"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlColorantTable : public CIccTagColorantTable, public CIccTagXml
{
public:
  virtual ~CIccTagXmlColorantTable() {}

  virtual const char *GetClassName() { return "CIccTagXmlColorantTable"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlViewingConditions : public CIccTagViewingConditions, public CIccTagXml
{
public:
  virtual ~CIccTagXmlViewingConditions() {}

  virtual const char *GetClassName() { return "CIccTagXmlViewingConditions"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlProfileSeqDesc : public CIccTagProfileSeqDesc, public CIccTagXml
{
public:
  virtual ~CIccTagXmlProfileSeqDesc() {}

  virtual const char *GetClassName() { return "CIccTagXmlProfileSeqDesc"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlResponseCurveSet16 : public CIccTagResponseCurveSet16, public CIccTagXml
{
public:
  virtual ~CIccTagXmlResponseCurveSet16() {}

  virtual const char *GetClassName() { return "CIccTagXmlResponseCurveSet16"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlCurve : public CIccTagCurve, public CIccTagXml
{
public:
  virtual ~CIccTagXmlCurve() {}

  virtual const char *GetClassName() { return "CIccTagXmlCurve"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlParametricCurve : public CIccTagParametricCurve, public CIccTagXml
{
public:
  virtual ~CIccTagXmlParametricCurve() {}

  virtual const char *GetClassName() { return "CIccTagXmlParametricCurve"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlLutAtoB : public CIccTagLutAtoB, public CIccTagXml
{
public:
  virtual ~CIccTagXmlLutAtoB() {}

  virtual const char *GetClassName() { return "CIccTagXmlLutAtoB"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlLutBtoA : public CIccTagLutBtoA, public CIccTagXml
{
public:
  virtual ~CIccTagXmlLutBtoA() {}

  virtual const char *GetClassName() { return "CIccTagXmlLutBtoA"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlLut8 : public CIccTagLut8, public CIccTagXml
{
public:
  virtual ~CIccTagXmlLut8() {}

  virtual const char *GetClassName() { return "CIccTagXmlLut8"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlLut16 : public CIccTagLut16, public CIccTagXml
{
public:
  virtual ~CIccTagXmlLut16() {}

  virtual const char *GetClassName() { return "CIccTagXmlLut16"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlMultiProcessElement : public CIccTagMultiProcessElement, public CIccTagXml
{
public:
  virtual ~CIccTagXmlMultiProcessElement() {}

  virtual const char *GetClassName() { return "CIccTagXmlMultiProcessElement"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

class CIccTagXmlProfileSequenceId : public CIccTagProfileSequenceId, public CIccTagXml
{
public:
  virtual ~CIccTagXmlProfileSequenceId() {}

  virtual const char *GetClassName() { return "CIccTagXmlProfileSequenceId"; }

  virtual IIccExtensionTag *GetExtension() { return this; }

  virtual bool ToXml(std::string &xml, std::string blanks="");
};

#endif //_ICCTAGXML_H
