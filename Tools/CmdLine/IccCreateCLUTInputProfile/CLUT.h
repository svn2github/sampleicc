/*
    File:       CLUT.cpp

    Contains:   part of iccCreateCLUTInputProfile command-line tool:
                create and write CLUT tag data

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
// -Initial implementation by Joseph Goldstone spring 2006
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEFINED_CLUT_H__
#define __DEFINED_CLUT_H__

#include <iostream>
using std::cout;
using std::endl;

#include <list>
using std::list;

#include <math.h>

#include "IccProfLibConf.h"
#include "IccDefs.h"
#include "IccUtil.h"
#include "IccIO.h"
#include "IccTagBasic.h"
#include "IccTagLut.h"

#include "CAT.h"

class CLUT
{
 public:
  class Stuffer : public IIccCLUTExec
    {
    public:

      Stuffer(unsigned int edgeN,
              const icFloatNumber* const rawXYZ,
              const CAT* const CATToPCS,
              const icFloatNumber* flare,
              const icFloatNumber* illuminant,
              const icFloatNumber* mediaWhite)
        :
        m_EdgeN(edgeN),
        m_RawXYZ(rawXYZ),
        m_Offset(new icFloatNumber[3]),
        m_Matrix(new icFloatNumber[9]),
        m_AdaptedMediaWhite(new icFloatNumber[3])
        {
                unsigned int i;
          for (i = 0; i < 3; ++i)
            m_Offset[i] = -flare[i];
          m_Matrix[0] = CATToPCS->m_CAT[0] / illuminant[1];
          m_Matrix[4] = CATToPCS->m_CAT[4] / illuminant[1];
          m_Matrix[8] = CATToPCS->m_CAT[8] / illuminant[1];
          icFloatNumber illuminantRelativeMediaWhite[3];
          for (i = 0; i < 3; ++i)
              illuminantRelativeMediaWhite[i] = mediaWhite[i] / illuminant[1];
          CATToPCS->Apply(m_AdaptedMediaWhite, illuminantRelativeMediaWhite);
        }

      ~Stuffer()
        {
          delete[] m_Offset;
          delete[] m_Matrix;
          delete[] m_AdaptedMediaWhite;
        }

      void PixelOp(icFloatNumber* pGridAdr, icFloatNumber* pData)
        {
          // 1.  Obtain CIE Tristimulus values for a set of colour patches on the device
          // or media to be profiled.  More information about measurement procedures is 
          // provided in clause D.3.  There should be at least one measurement of the
          // "media white" and the tristimulus values of the illumination source or
          // perfect reflecting diffuser should be specified.

          // [Presumably, this was done in order to get the XYZ values which are being
          // stuffed into this CLUT input profile, so there's no processing done here,

          unsigned int rIdx = static_cast<unsigned int>(pGridAdr[0] * (m_EdgeN - 1) + 0.5);
          unsigned int gIdx = static_cast<unsigned int>(pGridAdr[1] * (m_EdgeN - 1) + 0.5);
          unsigned int bIdx = static_cast<unsigned int>(pGridAdr[2] * (m_EdgeN - 1) + 0.5);
          unsigned int flattenedIdx = (rIdx * m_EdgeN * m_EdgeN + gIdx * m_EdgeN + bIdx) * 3;
          // 2. Remove flare from the measured XYZ values as needed to match
          // the PCS measurement conditions, creating flare-free XYZ values (XYZflare-free).

          for (unsigned int i = 0; i < 3; ++i)
            pData[i] = m_RawXYZ[flattenedIdx + i] + m_Offset[i];

          // 3. If necessary, scale the flare-free measurement values so they
          // are relative to the actual illumination source by dividing all
          // values by the measured Y value of the perfect diffuser.

          // 4. If the chromaticity of the illumination source is different
          // from that of D50, convert the illuminant-relative XYZ values
          // from the illumination source white point chromaticity to the
          // PCS white point chromaticity using an appropriate chromatic
          // adaptation transform and equation D.9 (which is the same as
          // D.1).  This may be done by applying one of the transformations
          // described in clause D.4 and Annex E.  The transform used must
          // be specified in the chromaticAdaptationTag.

          icFloatNumber adaptedPCSXYZ[3];
          adaptedPCSXYZ[0] = m_Matrix[0] * pData[0] + m_Matrix[1] * pData[1] + m_Matrix[2] * pData[2];
          adaptedPCSXYZ[1] = m_Matrix[3] * pData[0] + m_Matrix[4] * pData[1] + m_Matrix[5] * pData[2];
          adaptedPCSXYZ[2] = m_Matrix[6] * pData[0] + m_Matrix[7] * pData[1] + m_Matrix[8] * pData[2];

          icXYZtoLab(pData, adaptedPCSXYZ, m_AdaptedMediaWhite);
          icLabToPcs(pData);

        }
      const unsigned int m_EdgeN;
      const icFloatNumber* const m_RawXYZ;
      icFloatNumber* m_Offset;
      icFloatNumber* m_Matrix;
      icFloatNumber* m_AdaptedMediaWhite;
    };

  CLUT() {}

  CIccTagLut16*
  makeAToB0Tag(const unsigned int edgeN,
               const icFloatNumber* const rawXYZ,
               const CAT* const CATToPCS,
               const icFloatNumber* const flare,
               const icFloatNumber* const illuminant,
               const icFloatNumber* const mediaWhite) const;
};

#endif
