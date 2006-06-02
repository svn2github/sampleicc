/*
    File:       SampleICCInterface.h

    Contains:   Implementation of utility classes/functions

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
// -Initial implementation by David Hasler 27.11.2003
//
//////////////////////////////////////////////////////////////////////

#ifndef sample_ICC_cmm_interface
#define sample_ICC_cmm_interface

extern "C" {

void* iccCMM_NewInstance();
void* iccCMM_DeleteInstance(void* cmm);
short iccCMM_AddProfile(void* cmm, const char *szProfilePath, short nIntent=0, short nInterp=0);
short iccCMM_PrepareForComputation(void* cmm); //Must be called before calling Apply()
short iccCMM_ApplyProfiles(void* cmm, double *DstPixel, const double *SrcPixel);
unsigned long iccCMM_GetSourceSpace(void*);
unsigned long iccCMM_GetDestSpace(void*);


//the remaining of the file is just for DLL loading:

//pointers to functions definition to be used to dynamically load a dll
typedef void* (*iccCMM_NewInstanceProc)();
typedef void* (*iccCMM_DeleteInstanceProc)(void* cmm);
typedef short (*iccCMM_AddProfileProc)(void* cmm, const char *szProfilePath, short nIntent, short nInterp);
typedef short (*iccCMM_PrepareForComputationProc)(void* cmm); //Must be called before calling Apply()
typedef short (*iccCMM_ApplyProfilesProc)(void* cmm, double *DstPixel, const double *SrcPixel);
typedef unsigned long (*iccCMM_GetSourceSpaceProc)(void*);
typedef unsigned long (*iccCMM_GetDestSpaceProc)(void*);

typedef struct  {
  void *CMM;
  iccCMM_NewInstanceProc              NewInstance;
  iccCMM_DeleteInstanceProc           DeleteInstance;
  iccCMM_AddProfileProc               AddProfile;
  iccCMM_PrepareForComputationProc    PrepareForComputation;
  iccCMM_ApplyProfilesProc            ApplyProfiles;
  iccCMM_GetSourceSpaceProc           GetSourceSpace;
  iccCMM_GetDestSpaceProc             GetDestSpace;
} sampleICCCMMCollection;
 

};

#endif