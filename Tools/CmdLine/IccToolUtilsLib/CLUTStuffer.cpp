/*
 File:       CLUTStuffer.cpp
 
 Contains:   part of iccCreateCLUTInputProfile command-line tool:
 create aCLUT tag data
 
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
// -broken out of CLUT.h by Joseph Goldstone fall 2006
//
//////////////////////////////////////////////////////////////////////

#include "CLUTStuffer.h"

#include <list>
using std::list;

#include "IccProfLibConf.h"
#include "IccDefs.h"
#include "IccUtil.h"
#include "IccIO.h"
#include "IccTagBasic.h"
#include "IccTagLut.h"

CLUTStuffer::CLUTStuffer(unsigned int edgeN,
	const icFloatNumber* const measuredXYZ,
	const icFloatNumber* flare,
	const icFloatNumber illuminantY,
	const CAT* const CATToPCS,
	const icFloatNumber* adaptedMediaWhite)
	:	m_EdgeN(edgeN),
		m_MeasuredXYZ(measuredXYZ),
		m_IlluminantY(illuminantY),
		m_CAT(CATToPCS)
{
		for (int i = 0; i < 3; ++i)
		{
			m_Flare[i] = flare[i];
			m_AdaptedMediaWhite[i] = adaptedMediaWhite[i];
		}
}

void
CLUTStuffer::PixelOp(icFloatNumber* pGridAdr, icFloatNumber* pData)
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
		icFloatNumber measuredXYZ[3];
		for (unsigned int i = 0; i < 3; ++i)
		measuredXYZ[i] = m_MeasuredXYZ[flattenedIdx + i];
		
		// 2. Remove flare from the measured XYZ values as needed to match
		// the PCS measurement conditions, creating flare-free XYZ values (XYZflare-free).
		
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
		CLUT::measuredXYZToAdaptedXYZ(adaptedPCSXYZ, measuredXYZ, m_Flare,
			m_IlluminantY, m_CAT);
		
		icXYZtoLab(pData, adaptedPCSXYZ, m_AdaptedMediaWhite);
	
		icLabToPcs(pData);
}
