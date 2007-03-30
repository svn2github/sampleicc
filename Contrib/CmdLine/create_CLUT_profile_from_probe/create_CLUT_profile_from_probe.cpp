/*
 File:       iccExtractValuesFromGrabbedProbeImage.cpp
 
 Contains:   Command-line app that takes the pathname of a screen grab
	     of a probe image, and the pixels coordinates within that image
	     of the white border of the content area, and extracts from the
	     pixels within the border area the non-ICC-color-managed values
	     of the (known) probe pixel values, establishing a relationship
	     between that non-ICC system's un-color-managed and color-managed
	     pixel values.
 
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
// -Initial implementation by Joseph Goldstone spring 2007
//
//////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <limits>
using namespace std;

#include "Stubs.h"

#include "IccDefs.h"
#include "IccProfile.h"
#include "IccTagLut.h"
#include "IccUtil.h"
#include "IccCMM.h"

void
usage(ostream& s, const char* const myName)
{
  s << myName << ": usage is `" << myName
    << " <N> <grabbed_probe_RGB_values_path> <monitor_profile_path>"
    << " <input_profile_description> <input_profile_path>"
    << " [<pretransform_path>]" << endl;
}

vector<DeviceRGB>
RGBs_from_probe(const char* const grabbed_probe_RGB_values_path)
{
  ifstream iS(grabbed_probe_RGB_values_path);
  if (! iS)
  {
    ostringstream oSS;
    oSS << "Couldn't open RSR grabbed probe RGB values path `"
	<< grabbed_probe_RGB_values_path << "'";
    throw runtime_error(oSS.str());
  }
  double uninitialized = numeric_limits<double>::max();
  string line("");
  vector<DeviceRGB> triplets;
  while(getline(iS, line))
    {
      if (iS.eof()) break;
      double r = uninitialized;
      double g = uninitialized;
      double b = uninitialized;
      if (string("") == line)
	continue;
      istringstream iSS(line);
      iSS >> r >> g >> b;
      if (r == uninitialized || g == uninitialized || b == uninitialized)
	throw runtime_error("malformed input line.");
      triplets.push_back(DeviceRGB(r, g, b));
    }
  return triplets;
}

void
loadInputShaperLUTs(CIccTagCurve** inputShaperLUTs,
		    const std::string& inputShaperFilename)
{
  ifstream s(inputShaperFilename.c_str());
  if (! s)
  {
    ostringstream os;
    os << "Could not load input shaper LUTs from `" << inputShaperFilename
       << "'";
    throw runtime_error(os.str());
  }
	
  string maxChannelValueAsString;
  s >> maxChannelValueAsString;
  int maxChannelValue = atoi(maxChannelValueAsString.c_str());
  vector<double> redVals;
  vector<double> greenVals;
  vector<double> blueVals;
  string line("");
  while (getline(s, line))
  {
    if (line == "")
			continue;
    double redVal;
    double greenVal;
    double blueVal;
    istringstream is(line);
    is >> redVal;
    is >> greenVal;
    is >> blueVal;
    redVals.push_back(redVal);
    greenVals.push_back(greenVal);
    blueVals.push_back(blueVal);
  }
  unsigned int numEntries = (unsigned int)redVals.size();
  // now make the LUT objects of the appropriate length and stuff them.  
  CIccTagCurve*   redCurve = inputShaperLUTs[0];
  CIccTagCurve* greenCurve = inputShaperLUTs[1];
  CIccTagCurve*  blueCurve = inputShaperLUTs[2];
    redCurve->SetSize(numEntries, icInitIdentity);
  greenCurve->SetSize(numEntries, icInitIdentity);
   blueCurve->SetSize(numEntries, icInitIdentity);
  for (unsigned int j = 0, N = numEntries; j < N; ++j)
  {
    (  *redCurve)[j] = static_cast<icFloatNumber>(  redVals[j] / maxChannelValue);
    (*greenCurve)[j] = static_cast<icFloatNumber>(greenVals[j] / maxChannelValue);
    ( *blueCurve)[j] = static_cast<icFloatNumber>( blueVals[j] / maxChannelValue);
  }
}

class Simple_CLUT_stuffer : public IIccCLUTExec
{
public:
	Simple_CLUT_stuffer(int edge_N, const vector<CIEXYZ>& PCS_XYZs)
		: edge_N_(edge_N), PCS_XYZs_(PCS_XYZs)
	{
	}
	
	void
	PixelOp(icFloatNumber* pGridAdr, icFloatNumber* pData)
	{
		int r_idx = static_cast<int>(pGridAdr[0] * (edge_N_ - 1) + 0.5);
    int g_idx = static_cast<int>(pGridAdr[1] * (edge_N_ - 1) + 0.5);
    int b_idx = static_cast<int>(pGridAdr[2] * (edge_N_ - 1) + 0.5);
    int flat_idx = r_idx * edge_N_ * edge_N_ + g_idx * edge_N_ + b_idx;
		pData[0] = static_cast<icFloatNumber>(PCS_XYZs_[flat_idx].X());
		pData[1] = static_cast<icFloatNumber>(PCS_XYZs_[flat_idx].Y());
		pData[2] = static_cast<icFloatNumber>(PCS_XYZs_[flat_idx].Z());
//		cout << "r_idx " << r_idx << " g_idx " << g_idx << " b_idx " << b_idx 
//			<< " flat_idx " << flat_idx
//			<< " CIEXYZ(" << pData[0] << ", " << pData[1] << ", " << pData[2] << ")\n";
		icXyzToPcs(pData);
//		icFloatNumber PCS_XYZ[3];
//		PCS_XYZ[0] = PCS_XYZs_[flat_idx].X();
//		PCS_XYZ[1] = PCS_XYZs_[flat_idx].Y();
//		PCS_XYZ[2] = PCS_XYZs_[flat_idx].Z();
//		icXYZtoLab(pData, PCS_XYZ, icD50XYZ);
//		icLabToPcs(pData);
//		CIccPCS::Lab4ToLab2(pData, pData);
	}

private:
	const int edge_N_;
	const vector<CIEXYZ> PCS_XYZs_;
};

CIccTagLut16*
make_A2Bx_tag(int edge_N, const vector<CIEXYZ> PCS_XYZs,
							const char* const input_shaper_filename)
{
	CIccTagLut16* lut16 = new CIccTagLut16();
  lut16->Init(3, 3);
  lut16->SetColorSpaces(icSigRgbData, icSigLabData);
	
  lut16->NewMatrix();
	
  LPIccCurve* iLUT = lut16->NewCurvesA();
  for (int i = 0; i < 3; ++i)
  {
    CIccTagCurve* pCurve = new CIccTagCurve(0);
    pCurve->SetSize(2, icInitIdentity);
    iLUT[i] = pCurve;
  }
  
  CIccCLUT* CLUT = lut16->NewCLUT(edge_N);
	Simple_CLUT_stuffer* stuffer = new Simple_CLUT_stuffer(edge_N, PCS_XYZs);
	CLUT->Iterate(stuffer);
	
  LPIccCurve* oLUT = lut16->NewCurvesB();
	if (strcmp("", input_shaper_filename) == 0)
		for (int i = 0; i < 3; ++i)
		{
			CIccTagCurve* pCurve = new CIccTagCurve(0);
			pCurve->SetSize(2, icInitIdentity);
			oLUT[i] = pCurve;
		}
	else
	{
		CIccTagCurve* inputShaperLUTs[3];
		for (int i = 0; i < 3; ++i)
		{
			inputShaperLUTs[i] = new CIccTagCurve(0);
			inputShaperLUTs[i]->SetSize(2, icInitIdentity);
		}
		loadInputShaperLUTs(inputShaperLUTs, input_shaper_filename);
		for (int i = 0; i < 3; ++i)
			oLUT[i] = inputShaperLUTs[i];
	}
			return lut16;
}

vector<CIEXYZ>
XYZs_from_monitor_RGBs(const vector<DeviceRGB>& RGBs, CIccCmm* cmm)
{
	vector<CIEXYZ> XYZs;
	for (vector<DeviceRGB>::const_iterator
		iter = RGBs.begin(), end_iter = RGBs.end(); iter != end_iter; ++iter)
	{
		icFloatNumber RGB[3];
		RGB[0] = static_cast<icFloatNumber>(iter->R());
		RGB[1] = static_cast<icFloatNumber>(iter->G());
		RGB[2] = static_cast<icFloatNumber>(iter->B());
		icFloatNumber XYZ[3];
		cmm->Apply(XYZ, RGB);
		icXyzFromPcs(XYZ);
//		cout << "RGB(" << RGB[0] << ", " << RGB[1] << ", " << RGB[2] << ") -> XYZ("
//			<< XYZ[0] << ", " << XYZ[1] << ", " << XYZ[2] << ")\n";
		XYZs.push_back(CIEXYZ(XYZ[0], XYZ[1], XYZ[2]));
	}
	return XYZs;
}

int
main(int argc, char* argv[])
{
  try
    {
      int N = atoi(argv[1]);
      const char* const grabbed_probe_values_path = argv[2];
      vector<DeviceRGB> device_RGBs(RGBs_from_probe(grabbed_probe_values_path));
      const char* const monitor_profile_path = argv[3];
      const char* const input_profile_description = argv[4];
      const char* const input_profile_path = argv[5];
      char*       pretransform_path = "";
      if (argc == 7)
	pretransform_path = argv[6];
      const char* const copyright("copyright (c) Tippett Studios 2007");
		
      CIccProfile* monitor_profile = ReadIccProfile(monitor_profile_path);
      // +++ check result here
		
      CIccCmm* cmm = new CIccCmm();
      icRenderingIntent monitor_rendering_intent
	= static_cast<icRenderingIntent>(monitor_profile->m_Header.renderingIntent);
      if (cmm->AddXform(monitor_profile_path, monitor_rendering_intent) != icCmmStatOk)
	{
	  ostringstream s;
	  s << "Can't set profile `" << monitor_profile_path
	    << "' as initial CMM profile";
	  throw runtime_error(s.str());
	}
      if (cmm->Begin() != icCmmStatOk)
	{
	  throw runtime_error("Can't start CMM");
	}
		
      vector<CIEXYZ> PCS_XYZs_from_probe(XYZs_from_monitor_RGBs(device_RGBs, cmm));
		
      CIccProfile input_profile;
      input_profile.InitHeader();
      input_profile.m_Header.deviceClass     = icSigInputClass;
      input_profile.m_Header.platform        = icSigMacintosh;
      input_profile.m_Header.colorSpace      = icSigRgbData;
      input_profile.m_Header.pcs             = icSigXYZData;
      input_profile.m_Header.attributes      = static_cast<icUInt64Number>(icTransparency);
      input_profile.m_Header.renderingIntent = monitor_profile->m_Header.renderingIntent;
		
      // Required tags for an N-component LUT-based input profile, as layed out in
      // the ICC spec [sections 8.2 and 8.3.2] are:
      //   profileDescriptionTag
      //   copyrightTag
      //   mediaWhitePointTag
      //   chromaticAdaptationTag
      //   A2B0 tag
      // As it happens, not only are those ordered by their appearance in section
      // 8.2 and 8.3.2, they are pretty much also ordered in increasing complexity.
		
      // profileDescriptionTag
      CIccLocalizedUnicode USAEnglishDesc;
      USAEnglishDesc.SetText((string("(grabbed) ") + input_profile_description).c_str());
      CIccTagMultiLocalizedUnicode* descriptionTag = new CIccTagMultiLocalizedUnicode;
      descriptionTag->m_Strings = new CIccMultiLocalizedUnicode; // dtor does deletion
      descriptionTag->m_Strings->push_back(USAEnglishDesc);
      input_profile.AttachTag(icSigProfileDescriptionTag, descriptionTag);
		
      // copyrightTag
      CIccLocalizedUnicode USAEnglishCopyright;
      USAEnglishCopyright.SetText(copyright);
      CIccTagMultiLocalizedUnicode* copyrightTag = new CIccTagMultiLocalizedUnicode;
      copyrightTag->m_Strings = new CIccMultiLocalizedUnicode; // dtor does deletion
      copyrightTag->m_Strings->push_back(USAEnglishCopyright);
      input_profile.AttachTag(icSigCopyrightTag, copyrightTag);
		
      CIccTagXYZ* white_point_tag
	= static_cast<CIccTagXYZ*>(monitor_profile->FindTag(icSigMediaWhitePointTag));
      input_profile.AttachTag(icSigMediaWhitePointTag, white_point_tag);
		
      CIccTagXYZ* black_point_tag
	= static_cast<CIccTagXYZ*>(monitor_profile->FindTag(icSigMediaBlackPointTag));
      // This is not a required tag, but if it's there, carry it over.
      if (black_point_tag != NULL)
	input_profile.AttachTag(icSigMediaBlackPointTag, black_point_tag);
		
      CIccTagXYZ* luminance_tag
	= static_cast<CIccTagXYZ*>(monitor_profile->FindTag(icSigLuminanceTag));
      if (luminance_tag != NULL)
	input_profile.AttachTag(icSigLuminanceTag, luminance_tag);
			
		
      CIccTagS15Fixed16* monitor_chromatic_adaptation_tag
	= static_cast<CIccTagS15Fixed16*>(monitor_profile->FindTag(icSigChromaticAdaptationTag));
      input_profile.AttachTag(icSigChromaticAdaptationTag,
			      monitor_chromatic_adaptation_tag);
		
      if (PCS_XYZs_from_probe.size() != N * N * N)
	{
	  ostringstream s;
	  s << "Edge size " << N << " implies a probe file with " << N *N * N
	    << " entries, but the file provided contains "
	    << PCS_XYZs_from_probe.size();
	  throw runtime_error(s.str());
	}
      // +++ input shaper LUT someday
      CIccTagLut16* A2B0_tag = make_A2Bx_tag(N, PCS_XYZs_from_probe,
					     pretransform_path);
      input_profile.AttachTag(icSigAToB0Tag, A2B0_tag);
		
		
      //	CLUT* AToB0CLUT = new CLUT();
      //  CIccTagLut16* AToB0Tag
      //    = AToB0CLUT->makeAToBxTag(size, measuredXYZ, flare, illuminant, CATToD50,
      //															inputShaperGamma, inputShaperFilename,
      //															adaptedMediaWhite, LABPCS);
      //  BlackScaler blackScaler(size, measuredXYZ, adaptedMediaBlack, adaptedMediaWhite);
      //  AToB0CLUT->Iterate(&blackScaler);
      //  profile.AttachTag(icSigAToB0Tag, AToB0Tag); // the A2B0 tag
		
      //Verify things
      string validationReport;
      icValidateStatus validationStatus = input_profile.Validate(validationReport);
		
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
      out.Open(input_profile_path, "w+");
      input_profile.Write(&out);
      out.Close();
      return EXIT_SUCCESS;
    }
  catch (const exception& e)
    {
      cerr << "error: " << e.what() << endl;
      return EXIT_FAILURE;
    }
}
