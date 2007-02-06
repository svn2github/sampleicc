/*
 File:       iccFlattenAToB0Tag.cpp
 
 Contains:   Utility to flatten out AToB0 contents into a file, suitable as
             the input file to iccCreateCLUTInputProfile.  This is pretty much
             scaffolding for the creation & debugging of
             iccCreateCLUTInputProfile for the moment, but is also not a bad way
             to see how one would probe the CMM.
 
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
// -Initial implementation by Joseph Goldstone 11 May 2006
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "IccProfile.h"
// OpenICCProfile

#include "IccUtil.h"
// icLabFromPCS

#include "IccCmm.h"
// CIccCmm

typedef vector<icFloatNumber> ResultTuple;
typedef vector<ResultTuple> ResultTuples;

void
printTuples(ostream& oS, const ResultTuples& tuples)
{
  for (ResultTuples::const_iterator
       iter = tuples.begin(), endIter = tuples.end();
       iter != endIter; ++iter)
    oS << (*iter)[0] << " " << (*iter)[1] << " " << (*iter)[2] << endl;
}

int
main(const int argc, const char* argv[])
{
  if (argc != 4)
  {  
    cout << "iccFlattenAToBTag: usage is"
   << " iccFlattenAToBTag src_profile granularity results_file"
   << endl;
    return EXIT_FAILURE;
  }

  string srcProfilePath(argv[1]);
  unsigned int N = atoi(argv[2]);
  string flattenedContentsPath(argv[3]);

  CIccProfile* srcProfile = OpenIccProfile(srcProfilePath.c_str());
  if (srcProfile == NULL)
  {
    cerr << "Error opening source profile `" << srcProfile << "'" << endl;
    return EXIT_FAILURE;
  }
  
  CIccTagXYZ* mediaWhitePointTag
    = static_cast<CIccTagXYZ*>(srcProfile->FindTag(icSigMediaWhitePointTag));
  if (mediaWhitePointTag == NULL)
  {
      cerr  << "no white point tag found in source profile `" << srcProfile
            << "'" << endl;
    return EXIT_FAILURE;
  }
  icFloatNumber whiteXYZ[3];
  whiteXYZ[0] = icFtoD((*mediaWhitePointTag)[0].X);
  whiteXYZ[1] = icFtoD((*mediaWhitePointTag)[0].Y);
  whiteXYZ[2] = icFtoD((*mediaWhitePointTag)[0].Z);
  
  ResultTuples resultTuples(N * N * N);

  CIccCmm cmm;

  cmm.AddXform(srcProfile, icAbsoluteColorimetric);
  if (cmm.Begin() != icCmmStatOk) {
    cerr << "error initializing CMM" << endl;
    return EXIT_FAILURE;
  }
  
  for (unsigned int i = 0; i < N; ++i)
    for (unsigned int j = 0; j < N; ++j)
      for (unsigned int k = 0; k < N; ++k)
        {
          icFloatNumber dstPixel[3];
          icFloatNumber srcPixel[3];
          srcPixel[0] = (icFloatNumber) (i / (N - 1.0));
          srcPixel[1] = (icFloatNumber) (j / (N - 1.0));
          srcPixel[2] = (icFloatNumber) (k / (N - 1.0));
          cmm.Apply(dstPixel, srcPixel);
          if (srcProfile->m_Header.pcs == icSigLabData)
          {
            icLabFromPcs(dstPixel);
            icLabtoXYZ(dstPixel, NULL, whiteXYZ);
          }
          else
            icXyzFromPcs(dstPixel);
          ResultTuple resultTuple(dstPixel, dstPixel + 3);
          resultTuples[i * N * N + j * N + k] = resultTuple;
        }

  if (flattenedContentsPath == "-")
    printTuples(cout, resultTuples);
  else
  {
    ofstream oFS(flattenedContentsPath.c_str());
    printTuples(oFS, resultTuples);
  }

  return EXIT_SUCCESS;
}
