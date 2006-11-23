/*
 File:       iccCreateTRCMatrixDisplayProfile.cpp
 
 Contains:   Command-line app that takes external primary and TRC data and
 creates a display profile with that data stuffed into matrix column tags
 and TRCs.
 
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
// -Initial implementation by Joseph Goldstone fall 2006
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
using namespace std;

#include "IccUtil.h"
#include "IccProfile.h"

#include "CAT.h"
#include "CLUT.h"

void
usage(const char* my_name)
{
	cerr << my_name << ": usage is " << my_name << " IN.txt N description"
		<< " copyright out.ICC" << endl
		<< " where IN.txt is a file whose contents are:" << endl
		<< "  red XYZ primary (triplet of floats)" << endl
		<< "  green XYZ primary (triplet of floats)" << endl
		<< "  blue XYZ primary (triplet of floats)" << endl
		<< "  N lines being the tone curve for red (floats)" << endl
		<< "  N lines being the tone curve for green (floats)" << endl
		<< "  N lines being the tone curve for blue (floats)" << endl
		<< " N is the number of sample points per tone curve," << endl
		<< " description is a string which will be used to identify the profile,"
		<< " copyright is a string identifying the profile content owner,"
		<< " and" << endl
		<< " out.ICC is the file which will contain the created monitor profile"
		<< endl;
}

int
main(int argc, char* argv[])
{
	const char* myName = argv[0];
	
	if (argc != 6)
	{
		usage(myName);
		return EXIT_FAILURE;
	}
	
	try
	{
		string inFilename(argv[1]);
		int N = atoi(argv[2]);
		string description(argv[3]);
		string copyright(argv[4]);
		string outFilename(argv[5]);

		icFloatNumber measuredRed[3];
		icFloatNumber measuredGreen[3];
		icFloatNumber measuredBlue[3];
		icFloatNumber measuredBlack[3];
		icFloatNumber measuredWhite[3];
		ifstream in(inFilename.c_str());
		if (! in)
		{
			cerr << "Can't open input file `" << inFilename << "'" << endl;
			return EXIT_FAILURE;
		}
		in >>   measuredRed[0] >>   measuredRed[1] >>   measuredRed[2];
		in >> measuredGreen[0] >> measuredGreen[1] >> measuredGreen[2];
		in >>  measuredBlue[0] >>  measuredBlue[1] >>  measuredBlue[2];
		in >> measuredBlack[0] >> measuredBlack[1] >> measuredBlack[2];
		in >> measuredWhite[0] >> measuredWhite[1] >> measuredWhite[2];
		icFloatNumber*   redTRC = new icFloatNumber[N];
		icFloatNumber* greenTRC = new icFloatNumber[N];
		icFloatNumber*  blueTRC = new icFloatNumber[N];
		for (int i = 0; i < N; ++i)
			in >> redTRC[i] >> greenTRC[i] >> blueTRC[i];
		
		CIccProfile profile;
		profile.InitHeader();
		profile.m_Header.deviceClass = icSigDisplayClass;
		profile.m_Header.colorSpace = icSigRgbData;
		profile.m_Header.pcs = icSigXYZData;
		profile.m_Header.platform = icSigMacintosh;
		profile.m_Header.attributes = static_cast<icUInt64Number>(icTransparency);
		profile.m_Header.renderingIntent = icRelativeColorimetric;
		
		// Required tags for a three-component matrix-based display profile, as layed
		// out in the ICC spec [sections 8.2 and 8.4.3] are:
		//   profileDescriptionTag
		//   copyrightTag
		//   mediaWhitePointTag
		//   chromaticAdaptationTag
		//   redMatrixColumnTag
		//   greenMatrixColumnTag
		//   blueMatrixColumnTag
		//   redTRCTag
		//   greenTRCTag
		//   blueTRCTag
		
		// profileDescriptionTag
		CIccLocalizedUnicode USAEnglishDesc;
		USAEnglishDesc.SetText(description.c_str());
		CIccTagMultiLocalizedUnicode* descriptionTag = new CIccTagMultiLocalizedUnicode;
		descriptionTag->m_Strings = new CIccMultiLocalizedUnicode; // dtor does deletion
		descriptionTag->m_Strings->push_back(USAEnglishDesc);
		profile.AttachTag(icSigProfileDescriptionTag, descriptionTag);
		
		// copyrightTag
		CIccLocalizedUnicode USAEnglishCopyright;
		USAEnglishCopyright.SetText(copyright.c_str());
		CIccTagMultiLocalizedUnicode* copyrightTag = new CIccTagMultiLocalizedUnicode;
		copyrightTag->m_Strings = new CIccMultiLocalizedUnicode; // dtor does deletion
		copyrightTag->m_Strings->push_back(USAEnglishCopyright);
		profile.AttachTag(icSigCopyrightTag, copyrightTag);
		
		// For displays, and for projected imagery in cinema, we assume complete
		// adaptation on the part of the viewer, and we treat the medium white
		// as the illuminant.
		icFloatNumber illuminantY = measuredWhite[1];
		icFloatNumber normalizedIlluminant[3];
		for (unsigned int i = 0; i < 3; ++i)
			normalizedIlluminant[i] = measuredWhite[i] / illuminantY;
		CAT* CATToD50 = new CAT(icD50XYZ, normalizedIlluminant);
		
		icFloatNumber flare[3] = { 0, 0, 0 }; // perhaps oversimplified but...
		
		// mediaWhitePointTag
		CIccTagXYZ* whitePointTag = new CIccTagXYZ;
		icFloatNumber adaptedMediaWhite[3];
		CLUT::measuredXYZToAdaptedXYZ(adaptedMediaWhite, measuredWhite,
			flare, illuminantY, CATToD50);
		(*whitePointTag)[0].X = icDtoF(adaptedMediaWhite[0]);
		(*whitePointTag)[0].Y = icDtoF(adaptedMediaWhite[1]);
		(*whitePointTag)[0].Z = icDtoF(adaptedMediaWhite[2]);
		profile.AttachTag(icSigMediaWhitePointTag, whitePointTag);
		
		// mediaBlackPointTag
		CIccTagXYZ* blackPointTag = new CIccTagXYZ;
		icFloatNumber adaptedMediaBlack[3];
		CLUT::measuredXYZToAdaptedXYZ(adaptedMediaBlack, measuredBlack,
			flare, illuminantY, CATToD50);
		(*blackPointTag)[0].X = icDtoF(adaptedMediaBlack[0]);
		(*blackPointTag)[0].Y = icDtoF(adaptedMediaBlack[1]);
		(*blackPointTag)[0].Z = icDtoF(adaptedMediaBlack[2]);
		profile.AttachTag(icSigMediaBlackPointTag, blackPointTag);
		
		// chromaticAdaptationTag  
		CIccTagS15Fixed16* chromaticAdaptationTag = CATToD50->makeChromaticAdaptationTag();
		profile.AttachTag(icSigChromaticAdaptationTag, chromaticAdaptationTag);
		
		CIccTagXYZ* redMatrixColumnTag = new CIccTagXYZ;
		icFloatNumber adaptedRed[3];
		CLUT::measuredXYZToAdaptedXYZ(adaptedRed, measuredRed,
			flare, illuminantY, CATToD50);
		(*redMatrixColumnTag)[0].X = icDtoF(adaptedRed[0]);
		(*redMatrixColumnTag)[0].Y = icDtoF(adaptedRed[1]);
		(*redMatrixColumnTag)[0].Z = icDtoF(adaptedRed[2]);
		profile.AttachTag(icSigRedMatrixColumnTag, redMatrixColumnTag);
		
		CIccTagXYZ* greenMatrixColumnTag = new CIccTagXYZ;
		icFloatNumber adaptedGreen[3];
		CLUT::measuredXYZToAdaptedXYZ(adaptedGreen, measuredGreen,
			flare, illuminantY, CATToD50);
		(*greenMatrixColumnTag)[0].X = icDtoF(adaptedGreen[0]);
		(*greenMatrixColumnTag)[0].Y = icDtoF(adaptedGreen[1]);
		(*greenMatrixColumnTag)[0].Z = icDtoF(adaptedGreen[2]);
		profile.AttachTag(icSigGreenMatrixColumnTag, greenMatrixColumnTag);
		
		CIccTagXYZ* blueMatrixColumnTag = new CIccTagXYZ;
		icFloatNumber adaptedBlue[3];
		CLUT::measuredXYZToAdaptedXYZ(adaptedBlue, measuredBlue,
			flare, illuminantY, CATToD50);
		(*blueMatrixColumnTag)[0].X = icDtoF(adaptedBlue[0]);
		(*blueMatrixColumnTag)[0].Y = icDtoF(adaptedBlue[1]);
		(*blueMatrixColumnTag)[0].Z = icDtoF(adaptedBlue[2]);
		profile.AttachTag(icSigBlueMatrixColumnTag, blueMatrixColumnTag);
		
		CIccTagCurve* redTRCTag = new CIccTagCurve(N);
		for (int i = 0; i < N; ++i)
			(*redTRCTag)[i] = redTRC[i];
		profile.AttachTag(icSigRedTRCTag, redTRCTag);
		
		CIccTagCurve* greenTRCTag = new CIccTagCurve(N);
		for (int i = 0; i < N; ++i)
			(*greenTRCTag)[i] = greenTRC[i];
		profile.AttachTag(icSigGreenTRCTag, greenTRCTag);
		
		CIccTagCurve* blueTRCTag = new CIccTagCurve(N);
		for (int i = 0; i < N; ++i)
			(*blueTRCTag)[i] = blueTRC[i];
		profile.AttachTag(icSigBlueTRCTag, blueTRCTag);
		
		//Verify things
		string validationReport;
		icValidateStatus validationStatus = profile.Validate(validationReport);
		
		switch (validationStatus)
		{
		case icValidateOK:
			break;
			
		case icValidateWarning:
			clog << "Profile validation warning" << endl
			<< validationReport;
			break;
			
		case icValidateNonCompliant:
			clog << "Profile non compliancy" << endl
			<< validationReport;
			break;
			
		case icValidateCriticalError:
		default:
			clog << "Profile Error" << endl
			<< validationReport;
		}
		
		// Out it goes
		CIccFileIO out;
		out.Open(outFilename.c_str(), "w+");
		profile.Write(&out);
		out.Close();
		
		return EXIT_SUCCESS;
	}
	catch (const exception& e)
	{
		cerr << myName << ": error: " << e.what() << endl;
		return EXIT_FAILURE;
	}
	
}

