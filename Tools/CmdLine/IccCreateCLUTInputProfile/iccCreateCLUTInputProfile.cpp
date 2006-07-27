/*
    File:       iccCreateCLUTInputProfile.cpp

    Contains:   Command-line app that takes external CLUT data and creates
                an input profile with that CLUT data stuffed into an A2B0 tag.

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


#include <iostream>
using std::istream;
using std::cerr;
using std::cout;
using std::flush;
using std::clog;
using std::endl;

#include <ostream>
using std::ostream;

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

#include <sstream>
using std::istringstream;

#include <list>

#include "getopt.h" // for getopt_long

// At the moment this is a hack; later we will figure out how to set HAS_GETOPT_LONG
// in the configure script.  Feel free to extend this to support your favorite platform.
#if defined(__APPLE__)
#define HAS_GETOPT_LONG
#endif
// (told you it was a hack)

#include "IccProfLibConf.h"
#include "IccProfile.h"
#include "IccIO.h"
#include "IccUtil.h"
#include "IccTagBasic.h"

#include "CAT.h"
#include "CLUT.h"

void
readXYZFromString(icFloatNumber* const XYZ, char * s)
{
  istringstream iSS(s);
  iSS >> XYZ[0];
  iSS >> XYZ[1];
  iSS >> XYZ[2];
}

void
createProfile(const string& inFilename,
              const string& outFilename,
              const int size,
              const string& description,
              const string& copyright,
              const icFloatNumber* flare,
              const icFloatNumber* mediaWhite,
              const icFloatNumber* illuminant)
{                   
       unsigned int i;

  ifstream iFS(inFilename.c_str());
  unsigned int numData = 3 * size * size * size;
  icFloatNumber* rawXYZ = new icFloatNumber[numData];
  for (i = 0; i < numData; ++i)
    iFS >> rawXYZ[i];
  
  CIccProfile profile;
  profile.InitHeader();
  profile.m_Header.deviceClass = icSigInputClass;
  profile.m_Header.colorSpace = icSigRgbData;
  profile.m_Header.pcs = icSigLabData;
  profile.m_Header.platform = icSigMacintosh;
  profile.m_Header.attributes = static_cast<icUInt64Number>(icTransparency);
  profile.m_Header.renderingIntent = icPerceptual;

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

  // mediaWhitePointTag  
  CAT* CATToD50 = new CAT(icD50XYZ, illuminant);

  CIccTagXYZ* whitePointTag = new CIccTagXYZ;
  icFloatNumber illuminantRelativeMediaWhite[3];
  for (i = 0; i < 3; ++i)
    illuminantRelativeMediaWhite[i] = mediaWhite[i];

  illuminantRelativeMediaWhite[0] /= illuminant[1];
  illuminantRelativeMediaWhite[1] /= illuminant[1];
  illuminantRelativeMediaWhite[2] /= illuminant[1];
  icFloatNumber adaptedWhite[3];
  CATToD50->Apply(adaptedWhite, illuminantRelativeMediaWhite);
  (*whitePointTag)[0].X = icDtoF(adaptedWhite[0]);
  (*whitePointTag)[0].Y = icDtoF(adaptedWhite[1]);
  (*whitePointTag)[0].Z = icDtoF(adaptedWhite[2]);
  profile.AttachTag(icSigMediaWhitePointTag, whitePointTag);

  // chromaticAdaptationTag  
  CIccTagS15Fixed16* chromaticAdaptationTag = CATToD50->makeChromaticAdaptationTag();
  profile.AttachTag(icSigChromaticAdaptationTag, chromaticAdaptationTag);

  // A2B0 tag
  CLUT* AToB0CLUT = new CLUT();
  CIccTagLut16* AToB0Tag = AToB0CLUT->makeAToB0Tag(size, rawXYZ, CATToD50,
                                                   flare, illuminant, mediaWhite);
  profile.AttachTag(icSigAToB0Tag, AToB0Tag);

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
  
  delete CATToD50;
  delete[] rawXYZ;

}

void
usage(ostream& oS, const string& myName)
{
  string myShortName(myName);
  string::size_type lastSlash = myName.rfind('/');
  if (lastSlash != string::npos)
  {
    // assert(lastSlash != myName.size() - 1); // not likely, but you never know
    myShortName = myName.substr(lastSlash + 1);
  }
  oS << "Usage: " << myShortName
     << " [OPTION]... DESCRIPTION MEDIA_WHITE SIZE IN_FILE OUT_FILE"                                  << endl
                                                                                                      << endl
     << "Examples:"                                                                                   << endl
     << "  iccCreateCLUTInputProfile \"sample profile\" \"0.21 0.24 0.52\" 11"                        << endl
     << "  iccCreateCLUTInputProfile -f \"0.01 0.005 0.02\" \"sample profile\" \"0.21 0.24 0.52\" 11" << endl
                                                                                                      << endl
     << "where"                                                                                       << endl
     << "  DESCRIPTION is a string used to identify the profile, often set to the same as the"        << endl
     << "    filename of the profile less any trailing extension,"                                    << endl
                                                                                                      << endl
     << "  MEDIA_WHITE is a string containing the CIE XYZ coordinates of the media white point"       << endl
     << "    with embedded spaces between the X, Y and Z components"                                  << endl
                                                                                                      << endl
     << "  SIZE is an unsigned integer indicating the number of samples along each edge of the"       << endl
     << "    3D lookup table being loaded into the profile"                                           << endl
                                                                                                      << endl
     << "and where the OPTION values are as follows:"                                                 << endl
     << "   -h"
#if defined(HAS_GETOPT_LONG)
     << ", --help"
#endif
                                                                                                      << endl
     << " (prints this help text and exits)"                                                          << endl
                                                                                                      << endl
     << "   -f"
#if defined(HAS_GETOPT_LONG)
     << ", --flare"
#endif
     << " \"Xf Yf Zf\" (default \"0 0 0\")"                                                           << endl
     << " (indicates flare to be subtracted from measurements as a first stage in converting raw"     << endl
     << "  XYZ measurements to the ICC PCS)"                                                          << endl
                                                                                                      << endl
     << "   -i"
#if defined(HAS_GETOPT_LONG)
     << ", --illuminant"
#endif
     << "  \"Xi Yi Zi\" (default same as mediaWhite)"                                                 << endl
     << " (indicates illuminant used in converting raw XYZ measurements to the ICC PCS -- for"        << endl
     << "  projection displays, this usually has a value identical to that of the mandatory"          << endl
     << "  MEDIA_WHITE argument)"                                                                     << endl
                                                                                                      << endl
     << "   -c"
#if defined(HAS_GETOPT_LONG)
     << ", --copyright"
#endif
     << " copyright (default \"\")"                                                                   << endl
     << " (indicates owner, for purposes of legal copyright, of this profile)"                        << endl;
}

int
main(int argc, char * const argv[])
{
  icFloatNumber flare[3] = { 0, 0, 0 };
  bool sawExplicitFlare = false;

  icFloatNumber illuminant[3] = { 0, 0, 0 };
  bool sawExplicitIlluminant = false;

  string copyright("");

  string description("");

  icFloatNumber mediaWhite[3] = { 0, 0, 0 };

  int size = 0;

  string inFilename("");

  string outFilename("");
  
#if defined(HAS_GETOPT_LONG)
  static struct option longopts[] = {
    { "flare",       optional_argument, NULL, 'f' },
    { "illuminant",  optional_argument, NULL, 'i' },
    { "copyright",   optional_argument, NULL, 'c' },
    { "description", required_argument, NULL, 'd' },
    { "mediaWhite",  required_argument, NULL, 'w' },
    { "size",        required_argument, NULL, 's' },
    { NULL,          0,                 NULL,  0  }
  };
#endif

  while (true) {
#if defined(HAS_GETOPT_LONG)
    int shortOpt = getopt_long(argc, argv, "hf:i:c:", longopts, NULL);
#else
    int shortOpt = getopt(argc, argv, "hf:i:c:");
#endif
    if (shortOpt == -1)
      break;
    switch (shortOpt) 
      {
      case 'h':
	usage(cout, argv[0]);
	return EXIT_SUCCESS;
      case 'f':
	readXYZFromString(flare, optarg);
	sawExplicitFlare = true;
	break;
      case 'i':
	readXYZFromString(illuminant, optarg);
	sawExplicitIlluminant = true;
	break;
      case 'c':
	copyright = optarg;
	break;
      default:
	usage(cout, argv[0]);
	return EXIT_FAILURE;
      }
    }

  argc -= optind;
  argv += optind;

  if (argc != 5)
  {
    usage(cout, argv[0]);
    return EXIT_FAILURE;
  }
  
  description                 = argv[0];
  readXYZFromString(mediaWhite, argv[1]);
  size                   = atoi(argv[2]);
  inFilename                  = argv[3];
  outFilename                 = argv[4];

  if (! sawExplicitIlluminant)
    for (unsigned int i = 0; i < 3; ++i)
      illuminant[i] = mediaWhite[i];

  clog << "creating " << size << "x" << size << "x" << size
       << " CLUT-based ICC input profile `" << outFilename << "'" << endl
       << " from data in file " << inFilename << endl
       << " with an explicitly-specified media white point of "
       << mediaWhite[0] << " " << mediaWhite[1] << " " << mediaWhite[2] << endl
       << " an " << (sawExplicitIlluminant ? "explicit" : "implicit") << " illuminant of "
       << illuminant[0] << " " << illuminant[1] << " " << illuminant[2] << endl
       << " an " << (sawExplicitFlare ? "explicit" : "implicit") << " measurement flare level of "
       << flare[0] << " " << flare[1] << " " << flare[2] << endl;
  if (copyright == "")
    clog << " no copyright" << endl;
  else
    clog << " a copyright '" << copyright << "'" << endl;
  clog << " and the description `" << description << "'" << endl;

  createProfile(inFilename, outFilename, size,
                description, copyright,
                flare, illuminant, mediaWhite);

  return EXIT_SUCCESS;
}
