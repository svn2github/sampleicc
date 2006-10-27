#include <fstream>

#include "IccCmm.h"
#include "IccUtil.h"
#include "IccDefs.h"

//----------------------------------------------------
// Function Declarations
//----------------------------------------------------
#define IsSpacePCS(x) ((x)==icSigXYZData || (x)==icSigLabData)
bool ParseNumbers(icFloatNumber* pData, icChar* pString, icUInt32Number nSamples);
bool ParseName(icChar* pName, icChar* pString);

//----------------------------------------------------
// Function Definitions
//----------------------------------------------------

bool ParseNumbers(icFloatNumber* pData, icChar* pString, icUInt32Number nSamples)
{
  icUInt32Number nNumbersRead;

  switch(nSamples) {

  case 1:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT, &pData[0]);
      break;
    }

  case 2:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1]);
      break;
    }

  case 3:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1], &pData[2]);
      break;
    }

  case 4:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1], &pData[2], &pData[3]);
      break;
    }

  case 5:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1], &pData[2],
                            &pData[3], &pData[4]);
      break;
    }

  case 6:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1], &pData[2],
                            &pData[3], &pData[4], &pData[5]);
      break;
    }

  case 7:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1], &pData[2],
                            &pData[3], &pData[4], &pData[5], &pData[6]);
      break;
    }

  case 8:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1], &pData[2],
                            &pData[3], &pData[4], &pData[5], &pData[6], &pData[7]);
      break;
    }

  case 9:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1], &pData[2],
                            &pData[3], &pData[4], &pData[5], &pData[6], &pData[7], &pData[8]);
      break;
    }

  case 10:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1],
                            &pData[2],  &pData[3], &pData[4], &pData[5], &pData[6], &pData[7], 
                            &pData[8], &pData[9]);
      break;
    }

  case 11:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1],
                            &pData[2],  &pData[3], &pData[4], &pData[5], &pData[6], &pData[7], 
                            &pData[8], &pData[9], &pData[10]);
      break;
    }

  case 12:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0], &pData[1],
                            &pData[2],  &pData[3], &pData[4], &pData[5], &pData[6], &pData[7], 
                            &pData[8], &pData[9], &pData[10], &pData[11]);
      break;
    }

  case 13:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0],
                            &pData[1],  &pData[2],  &pData[3], &pData[4], &pData[5], &pData[6], &pData[7], 
                            &pData[8], &pData[9], &pData[10], &pData[11], &pData[12]);
      break;
    }

  case 14:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0],
                            &pData[1],  &pData[2],  &pData[3], &pData[4], &pData[5], &pData[6], &pData[7], 
                            &pData[8], &pData[9], &pData[10], &pData[11], &pData[12], &pData[13]);
      break;
    }

  case 15:
    {
      nNumbersRead = sscanf(pString, ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT " " ICFLOATFMT, &pData[0],
                            &pData[1],  &pData[2],  &pData[3], &pData[4], &pData[5], &pData[6], &pData[7], 
                            &pData[8], &pData[9], &pData[10], &pData[11], &pData[12], &pData[13], &pData[14]);
      break;
    }

  default:
    printf("\n%lu Color Samples not supported.\n", nSamples);
    return false;
  }

  if(nNumbersRead != nSamples) {
    return false;
  }

  return true;
}

//===================================================

bool ParseName(icChar* pName, icChar* pString)
{
  if(strncmp(pString, "{ \"", 3))
    return false;

  icChar *ptr = strstr(pString, "\" }");

  if(!ptr)
    return false;

  icUInt32Number nNameLen = ptr - (pString+3);

  if(!nNameLen)
    return false;

  strncpy(pName, pString+3, nNameLen);
  pName[nNameLen] = '\0';
  
  return true;
}


//===================================================

int main(int argc, icChar* argv[])
{
  if(argc<3) {
    printf("Usage: iccApplyNamedCmm data_file_path final_data_encoding {profile_file_path Rendering_intent}\n\n");
    printf("  For final_data_encoding:\n");
    printf("    0 - icEncodeValue\n");
    printf("    1 - icEncodePercent\n");
    printf("    2 - icEncodeFloat\n");
    printf("    3 - icEncode8Bit\n");
    printf("    4 - icEncode16Bit\n\n");

    printf("  For Rendering_intent:\n");
    printf("    0 - Perceptual\n");
    printf("    1 - Relative Colorimetric\n");
    printf("    2 - Saturation\n");
    printf("    3 - Absolute Colorimetric\n");

    return -1;
  }

  int nNumProfiles, temp;
  temp = argc - 3;

  if(temp%2 != 0) {
    printf("\nMissing arguments!\n");
    printf("Usage: ApplyNamedCmm data_file_path final_data_encoding {profile_file_path Rendering_Transform}\n\n");
    printf("  For final_data_encoding:\n");
    printf("    0 - icEncodeValue\n");
    printf("    1 - icEncodePercent\n");
    printf("    2 - icEncodeFloat\n");
    printf("    3 - icEncode8Bit\n");
    printf("    4 - icEncode16Bit\n\n");

    printf("  For Rendering_Transform:\n");
    printf("    0 - Perceptual\n");
    printf("    1 - Relative Colorimetric\n");
    printf("    2 - Saturation\n");
    printf("    3 - Absolute Colorimetric\n");
    printf("    20 - Preview Perceptual\n");
    printf("    21 - Preview Relative Colorimetric\n");
    printf("    22 - Preview Saturation\n");
    printf("    23 - Preview Absolute Colorimetric\n");
    printf("    30 - Gamut\n");

    return -1;
  }

  nNumProfiles = temp/2;

  std::ifstream InputData(argv[1]);

  if(!InputData) {
    printf("\nFile [%s] cannot be opened.\n", argv[1]);
    return false;
  }

  icChar COlorSig[5], tempBuf[512];
  InputData.getline(tempBuf, sizeof(tempBuf));

  int i;
  for (i = 0; i < 4; i++) {
    COlorSig[i] = tempBuf[i+1];
  }
  COlorSig[4] = '\0';


  icColorSpaceSignature SrcspaceSig = (icColorSpaceSignature)icGetSigVal(COlorSig);
  int nSamples = icGetSpaceSamples(SrcspaceSig);

  if(SrcspaceSig != icSigNamedData) {
    if(!nSamples) {
      printf("Source color space signature not recognized.\n");
      return -1;
    }
  }

  InputData.getline(tempBuf, sizeof(tempBuf));
  sscanf(tempBuf, "%s", tempBuf);

  icFloatColorEncoding srcEncoding, destEncoding;

  srcEncoding = CIccCmm::GetFloatColorEncoding(tempBuf);

  if(srcEncoding == icEncodeUnknown) {
    printf("Source color data encoding not recognized.\n");
    return false;
  }

  destEncoding = (icFloatColorEncoding)atoi(argv[2]);

  int nIntent, nType;
  CIccNamedColorCmm namedCmm(SrcspaceSig, icSigUnknownData, !IsSpacePCS(SrcspaceSig));

  int nCount;
  for(i = 0, nCount=3; i<nNumProfiles; i++, nCount+=2) {
    nIntent = atoi(argv[nCount+1]);
    nType = abs(nIntent) / 10;
    nIntent = nIntent % 10;

    if (namedCmm.AddXform(argv[nCount], nIntent<0 ? icUnknownIntent : (icRenderingIntent)nIntent, icInterpLinear, (icXformLutType)nType)) {
      printf("Invalid Profile:  %s\n", argv[nCount]);
      return -1;
    }
  }

  if(namedCmm.Begin()) {
    printf("One or more invalid profiles added.\n");
    return -1;
  }

  icColorSpaceSignature DestspaceSig = namedCmm.GetDestSpace();
  int nDestSamples = icGetSpaceSamples(DestspaceSig);
  std::string OutPutData;
  char SrcNameBuf[256], DestNameBuf[256];
  icFloatNumber SrcPixel[16], DestPixel[16], Pixel[16];

  sprintf(tempBuf,"%s\t; ", icGetSig(tempBuf, DestspaceSig, false));
  OutPutData += tempBuf;
  OutPutData += "Data Format\n";

  if(DestspaceSig==icSigNamedData)
    destEncoding = icEncodeValue;
  sprintf(tempBuf, "%s\t; ", CIccCmm::GetFloatColorEncoding(destEncoding));
  OutPutData += tempBuf;
  OutPutData += "Encoding\n\n";

  OutPutData += ";Source Data Format: ";
  sprintf(tempBuf,"%s\n", icGetSig(tempBuf, SrcspaceSig, false));
  OutPutData += tempBuf;

  if(SrcspaceSig==icSigNamedData)
    srcEncoding = icEncodeValue;
  OutPutData += ";Source Data Encoding: ";
  sprintf(tempBuf, "%s\n", CIccCmm::GetFloatColorEncoding(srcEncoding));
  OutPutData += tempBuf;

  OutPutData += ";Source data is after semicolon\n";
  
  OutPutData += "\n;Profiles applied\n";
  for(i = 0, nCount=3; i<nNumProfiles; i++, nCount+=2) {
    OutPutData += "; ";
    sprintf(tempBuf, "%s\n", argv[nCount]);
    OutPutData += tempBuf;
  }
  OutPutData += "\n";
  
  fwrite(OutPutData.c_str(), 1, OutPutData.length(), stdout);

  while(!InputData.eof()) {

    if(SrcspaceSig==icSigNamedData) {
      InputData.getline(tempBuf, sizeof(tempBuf));
      if(!ParseName(SrcNameBuf, tempBuf))
        continue;

      OutPutData.erase();

      switch(namedCmm.GetInterface()) {
        case icApplyNamed2Pixel:
          {
            if(namedCmm.Apply(DestPixel, SrcNameBuf)) {
              printf("Profile application failed.\n");
              return -1;
            }

            if(CIccCmm::FromInternalEncoding(DestspaceSig, destEncoding, DestPixel, DestPixel)) {
              printf("Invalid final data encoding\n");
              return -1;
            }

            for(i = 0; i<nDestSamples; i++) {
              sprintf(tempBuf, "%9.4lf ", DestPixel[i]);
              OutPutData += tempBuf;
            }
            OutPutData += "\t; ";
            
            break;
          }
        case icApplyNamed2Named:
          {
            if(namedCmm.Apply(DestNameBuf, SrcNameBuf)) {
              printf("Profile application failed.\n");
              return -1;
            }
            
            sprintf(tempBuf, "{ \"%s\" }\t; ", DestNameBuf);
            OutPutData += tempBuf;

            break;
          }
        case icApplyPixel2Pixel:
        case icApplyPixel2Named:
        default:
          printf("Incorrect interface.\n");
          return -1;
      }      

      sprintf(tempBuf, "{ \"%s\" }\n", SrcNameBuf);
      OutPutData += tempBuf;
    }
    else {
      InputData.getline(tempBuf, sizeof(tempBuf));
      if(!ParseNumbers(Pixel, tempBuf, nSamples))
        continue;

      OutPutData.erase();
      if(CIccCmm::ToInternalEncoding(SrcspaceSig, srcEncoding, SrcPixel, Pixel)) {
        printf("Invalid source data encoding\n");
        return -1;
      }

      switch(namedCmm.GetInterface()) {
        case icApplyPixel2Pixel:
          {
            if(namedCmm.Apply(DestPixel, SrcPixel)) {
              printf("Profile application failed.\n");
              return -1;
            }
            if(CIccCmm::FromInternalEncoding(DestspaceSig, destEncoding, DestPixel, DestPixel)) {
              printf("Invalid final data encoding\n");
              return -1;
            }

            for(i = 0; i<nDestSamples; i++) {
              sprintf(tempBuf, "%9.4lf ", DestPixel[i]);
              OutPutData += tempBuf;
            }
            OutPutData += "\t; ";
            break;
          }
        case icApplyPixel2Named:
          {
            if(namedCmm.Apply(DestNameBuf, SrcPixel)) {
              printf("Profile application failed.\n");
              return -1;
            }
            sprintf(tempBuf, "{ \"%s\" }\t; ", DestNameBuf);
            OutPutData += tempBuf;
            break;
          }
        case icApplyNamed2Pixel:
        case icApplyNamed2Named:
        default:
          printf("Incorrect interface.\n");
          return -1;
      }      


      for(i = 0; i<nSamples; i++) {
        sprintf(tempBuf, "%9.4lf ", Pixel[i]);
        OutPutData += tempBuf;
      }

      OutPutData += "\n";
    }

    fwrite(OutPutData.c_str(), 1, OutPutData.length(), stdout);
  }


  return 0;
}
