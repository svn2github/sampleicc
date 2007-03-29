/*
 File:	createRSRPretransformCurve.cpp
 
 Contains:   Command-line app that writes to standard output a 'pre-transform'
						 curve using formulae and parameters typical for a facility that
 						 might be trying to work with an 8-bit-TIFF pipeline that must
 						 handle 10-bit-log data as best it can, by some lossy compression
 						 and imperfectly reversible function.  This is example code;
 						 any facility using this will probably need to copy it and work
 						 to make that copy reflect the realities of their production.
 
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
#include <cmath>
using namespace std;

void
usage(char* my_name)
{
	cerr << my_name << ": usage is " << my_name << " N OUT.txt"<< endl
	<< " where N is the number of curve samples to generate and OUT.txt" << endl
	<< " is the file into which they will be written." << endl;
}

int
main(int argc, char* argv[])
{
	double cineon_black = 95;
	double cineon_white = 685;
	double dGamma = 1.7;
	double film_gamma = 0.6;
	double enc_gamma = 2.2;
	double K = (0.002 / film_gamma) * (1.7 / dGamma);
	double A = 1 / (pow(10.0, K * (cineon_white - cineon_black)) - 1);
	
	if (argc != 3)
	{
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	int N = atoi(argv[1]);
	ofstream s(argv[2]);
	
	s << 1023 << endl;
	for (int in = 0; in < N; ++in)
	{
		double stim = static_cast<double>(in) / (N - 1);
		double gammaed_stim = pow(stim, enc_gamma);
		// double out = (cineon_black + log10((gammaed_stim + A) / A) / K) * (255.0 / 1023.0);
		double out = cineon_black + log10((gammaed_stim + A) / A) / K;
		s << out << " " << out << " " << out << endl;
	}
	return EXIT_SUCCESS;
}


