/*
 File:       iccExtractPrimaryRamps.cpp
 
 Contains:   iccExtractPrimaryRamps command-line tool:
						 reconstruct values that "would have been measured" for red, green,
             blue and grey ramps, and print on standard output.
 
 Version:    V1
 
 Copyright:  (c) see below
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
// -originally written by Joseph Goldstone fall 2006
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "IccCmm.h"
#include "IccUtil.h"
#include "CLUT.h"
#include "IccToolException.h"

void
usage(const char* const myName)
{
	cerr << myName << ": usage is " << myName << " in.icc N out.txt" << endl;
}

icColorSpaceSignature
getPCSSpace(CIccProfile* profile)
{
	return profile->m_Header.pcs;
}

icFloatNumber
getIlluminantY(CIccProfile* profile)
{
	CIccTagXYZ* luminanceTag
		= static_cast<CIccTagXYZ*>(profile->FindTag(icSigLuminanceTag));
	if (! luminanceTag)
		throw IccToolException("No luminance tag found");
	return icFtoD((*luminanceTag)[0].Y);
}

CAT*
getInverseCAT(CIccProfile* profile)
{
	CIccTagS15Fixed16* CATTag = static_cast<CIccTagS15Fixed16*>
		(profile->FindTag(icSigChromaticAdaptationTag));
	if (! CATTag)
		throw IccToolException("No chromaticAdaptation tag found");
	icFloatNumber CATContents[9];
	for(int i = 0; i < 9; ++i)
		CATContents[i] = icFtoD((*CATTag)[i]);
	CAT forward(CATContents);
	return forward.Inverse();
}

void
getAdaptedMediaWhite(icFloatNumber* white, CIccProfile* profile)
{
	CIccTagXYZ* mediaWhiteTag
		= static_cast<CIccTagXYZ*>(profile->FindTag(icSigMediaWhitePointTag));
	if (! mediaWhiteTag)
		throw IccToolException("No media white point tag found");
	white[0] = icFtoD((*mediaWhiteTag)[0].X);
	white[1] = icFtoD((*mediaWhiteTag)[0].Y);
	white[2] = icFtoD((*mediaWhiteTag)[0].Z);
}

void
initParamsFromProfile(bool& isLabPCS, icFloatNumber& illuminantY,
	CAT*& inverseCAT, icFloatNumber* adaptedMediaWhite,
	const char * const profileName)
{
	// Pluck out of the profile the things we will need to invert the 
	// transformations that were made to take raw measurements to the PCS.
	CIccProfile* profile = ReadIccProfile(profileName);
	
	isLabPCS = getPCSSpace(profile) == icSigLabData;
	
	// +++ make provision for no luminance data in profile - get from command
	// line, with appropriate warning if specified value differs from value
	// that we then find in the file.  For the moment, we just throw an error
	// if there's no illuminant there.
	illuminantY = getIlluminantY(profile);
	
	inverseCAT = getInverseCAT(profile);
	
	getAdaptedMediaWhite(adaptedMediaWhite, profile);
	
	delete profile;
}

void
unstuff(icFloatNumber* measuredXYZ,
	const icFloatNumber* flare,
	const icFloatNumber illuminantY,
	const CAT* inverseCAT,
	const icFloatNumber* adaptedMediaWhite,
	const icFloatNumber* adjustedPCSXYZ)
{
	icFloatNumber adaptedXYZ[3];
	for (unsigned int i = 0; i < 3; ++i)
		adaptedXYZ[i] = adjustedPCSXYZ[i] * adaptedMediaWhite[i] / icD50XYZ[i];
	
	CLUT::adaptedXYZToMeasuredXYZ(measuredXYZ, adaptedXYZ, flare, illuminantY,
		inverseCAT);
}

int
main(int argc, char* argv[])
{
	char* myName = argv[0];
	if (argc != 4)
	{
		usage(myName);
		return EXIT_FAILURE;
	}
	
	char* profileFilename				= argv[1];
	int N												= atoi(argv[2]);
	char* extractedTextFilename = argv[3];
	try
	{
		bool isLabPCS;
		icFloatNumber illuminantY;
		CAT* inverseCAT;
		icFloatNumber adaptedMediaWhite[3];
		
		initParamsFromProfile(isLabPCS, illuminantY, inverseCAT, adaptedMediaWhite,
			profileFilename);
		
		CIccCmm cmm;
		if (cmm.AddXform(profileFilename, icAbsoluteColorimetric))
		{
			ostringstream s;
			s << "Can't set profile `" << profileFilename
				<< "' as initial CMM profile";
			throw IccToolException(s.str());
		}
		
		if (cmm.Begin() != icCmmStatOk)
			throw IccToolException("Error starting CMM");
		
		ofstream s(extractedTextFilename);
		for (int i = 0; i < N; ++i)
		{
			double stim = i / (N - 1.0);
			icFloatNumber   redPCSStim[3];
			icFloatNumber greenPCSStim[3];
			icFloatNumber  bluePCSStim[3];
			icFloatNumber  greyPCSStim[3];
			  redPCSStim[0] = stim;   redPCSStim[1] =    0;   redPCSStim[2] =    0;
			greenPCSStim[0] =    0; greenPCSStim[1] = stim; greenPCSStim[2] =    0;
			 bluePCSStim[0] =    0;  bluePCSStim[1] =    0;  bluePCSStim[2] = stim;
			 greyPCSStim[0] = stim;  greyPCSStim[1] = stim;  greyPCSStim[2] = stim;
			icFloatNumber   redPCSPixel[3];
			icFloatNumber greenPCSPixel[3];
			icFloatNumber  bluePCSPixel[3];
			icFloatNumber  greyPCSPixel[3];
			cmm.Apply(  redPCSPixel,   redPCSStim);
			cmm.Apply(greenPCSPixel, greenPCSStim);
			cmm.Apply( bluePCSPixel,  bluePCSStim);
			cmm.Apply( greyPCSPixel,  greyPCSStim);
			icFloatNumber   redAdjustedPCSXYZ[3];
			icFloatNumber greenAdjustedPCSXYZ[3];
			icFloatNumber  blueAdjustedPCSXYZ[3];
			icFloatNumber  greyAdjustedPCSXYZ[3];
			if (isLabPCS)
			{
				icLabFromPcs(  redPCSPixel);
				icLabFromPcs(greenPCSPixel);
				icLabFromPcs( bluePCSPixel);
				icLabFromPcs( greyPCSPixel);
				icLabtoXYZ(  redAdjustedPCSXYZ,   redPCSPixel, icD50XYZ);
				icLabtoXYZ(greenAdjustedPCSXYZ, greenPCSPixel, icD50XYZ);
				icLabtoXYZ( blueAdjustedPCSXYZ,  bluePCSPixel, icD50XYZ);
				icLabtoXYZ( greyAdjustedPCSXYZ,  greyPCSPixel, icD50XYZ);
			}
			else
			{
				for (unsigned int i = 0; i < 3; ++i)
				{
					  redAdjustedPCSXYZ[i] =   redPCSPixel[i];
					greenAdjustedPCSXYZ[i] = greenPCSPixel[i];
					 blueAdjustedPCSXYZ[i] =  bluePCSPixel[i];
					 greyAdjustedPCSXYZ[i] =  greyPCSPixel[i];
				}
				icXyzFromPcs(  redAdjustedPCSXYZ);
				icXyzFromPcs(greenAdjustedPCSXYZ);
				icXyzFromPcs( blueAdjustedPCSXYZ);
				icXyzFromPcs( greyAdjustedPCSXYZ);
			}				
			icFloatNumber measuredRed[3];
			icFloatNumber measuredGreen[3];
			icFloatNumber measuredBlue[3];
			icFloatNumber measuredGrey[3];
			
			// someday take this from the command line as an optional argument.
			icFloatNumber flare[3] = { 0, 0, 0 };
			
			unstuff(measuredRed,   flare, illuminantY, inverseCAT, adaptedMediaWhite,
				  redAdjustedPCSXYZ);
			unstuff(measuredGreen, flare, illuminantY, inverseCAT, adaptedMediaWhite,
				greenAdjustedPCSXYZ);
			unstuff(measuredBlue,  flare, illuminantY, inverseCAT, adaptedMediaWhite,
				 blueAdjustedPCSXYZ);
			unstuff(measuredGrey,  flare, illuminantY, inverseCAT, adaptedMediaWhite,
				 greyAdjustedPCSXYZ);
			
			s	<< stim
				<< " " <<   measuredRed[0]
				<< " " <<   measuredRed[1]
				<< " " <<   measuredRed[2]
				<< " " <<   measuredGreen[0]
				<< " " <<   measuredGreen[1]
				<< " " <<   measuredGreen[2]
				<< " " <<   measuredBlue[0]
				<< " " <<   measuredBlue[1]
				<< " " <<   measuredBlue[2]
				<< " " <<   measuredGrey[0]
				<< " " <<   measuredGrey[1]
				<< " " <<   measuredGrey[2]
				<< endl;
		}
		return EXIT_SUCCESS;
	}
	catch (const exception& e)
	{
		cerr << myName << ": error: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}


