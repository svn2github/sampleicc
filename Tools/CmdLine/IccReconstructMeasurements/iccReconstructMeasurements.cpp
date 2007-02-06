/*
 File:       iccReconstructMeasurements.cpp
 
 Contains:   .
 
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

#include "IccToolException.h"
#include "MeasurementExtractor.h"

void
processMeasurements(istream& in_s,
		    MeasurementExtractor& extractor,
		    const char* const XYZMeasurementsFilename)
{
  ofstream out_s(XYZMeasurementsFilename);
  if (! out_s)
    {
      ostringstream s;
      s << "Could not open file `" << XYZMeasurementsFilename << "' for output";
      throw IccToolException(s.str());
    }
  
  while (! in_s.eof())
    {
      string line("");
      getline(in_s, line);
      if (line == "")
	break;
      istringstream l_s(line);
      icFloatNumber RGBStimulus[3];
      l_s >> RGBStimulus[0] >> RGBStimulus[1] >> RGBStimulus[2];
      icFloatNumber measuredXYZ[3];
      extractor.reconstructMeasurement(measuredXYZ, RGBStimulus);
      out_s << measuredXYZ[0] << " "
	    << measuredXYZ[1] << " "
	    << measuredXYZ[2] << endl;
    }
}

int
main(int argc, char* argv[])
{
  try
    {
      char* RGBStimuliFilename = argv[1];
      ifstream in_s(RGBStimuliFilename);
      if (! in_s)
	{
	  ostringstream s;
	  s << "Could not open file `" << RGBStimuliFilename << "'";
	  throw IccToolException(s.str());
	}
      char* ICCProfileFilename = argv[2];
      icFloatNumber flare[3] = {0, 0, 0};
      
      if (argc == 5)
	{
	  icFloatNumber illuminantY = atof(argv[3]);
	  char* XYZMeasurementsFilename = argv[4];
	  MeasurementExtractor extractor(ICCProfileFilename,
					 illuminantY,
					 flare);
	  processMeasurements(in_s, extractor, XYZMeasurementsFilename);
	}
      else
	{
	  char* XYZMeasurementsFilename = argv[3];
	  MeasurementExtractor extractor(ICCProfileFilename,
					 flare);
	  processMeasurements(in_s, extractor, XYZMeasurementsFilename);
	}
      return EXIT_SUCCESS;
    }
  catch (const exception& e)
    {
      cerr << "Error: " << e.what() << endl;
      return EXIT_FAILURE;
    }
}
