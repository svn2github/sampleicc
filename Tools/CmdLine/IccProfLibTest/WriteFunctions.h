#include <stdio.h>
#include <iostream>

#include "IccProfile.h"
#include "IccTag.h"
#include "IccUtil.h"
#include "IccDefs.h"

//----------------------------------------------------
// Function Declarations
//----------------------------------------------------

bool WriteData(CIccProfile *pIcc, CIccIO *pIO);
bool WriteTag(CIccProfile *pIcc, icSignature sig);

