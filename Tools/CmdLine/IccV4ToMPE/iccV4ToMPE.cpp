// IccFloat.cpp : Defines the entry point for the console application.
//

#include "IccCmm.h"
#include "IccTagMpe.h"
#include "IccMpeBasic.h"

#define minXYZ 0.0
#define maxXYZ (1.0 + 32767.0/32768)
#define minL 0.0
#define maxL 100.0
#define minAB -128.0
#define maxAB 127.0


CIccMultiProcessElement *ConvertCLUT(CIccCLUT *pCLUT)
{
  CIccMpeCLUT *pNewCLUT = new CIccMpeCLUT();

  pNewCLUT->SetCLUT(new CIccCLUT(*pCLUT));

  return pNewCLUT;
}

CIccMultiProcessElement *ConvertMatrix(CIccMatrix *pMatrix)
{
  CIccMpeMatrix *pNewMatrix = new CIccMpeMatrix();

  if (pNewMatrix) {
    pNewMatrix->SetSize(3, 3);

    icFloatNumber *toMatrix = pNewMatrix->GetMatrix();
    icFloatNumber *toConstants = pNewMatrix->GetConstants();

    memcpy(toMatrix, pMatrix->m_e, 9*sizeof(icFloatNumber));
    if (pMatrix->m_bUseConstants) {
      memcpy(toConstants, &pMatrix->m_e[9], 3*sizeof(icFloatNumber));
    }
    else {
      toConstants[0] = 0.0;
      toConstants[1] = 0.0;
      toConstants[2] = 0.0;
    }

    return pNewMatrix;
  }
  return NULL;
}

CIccMultiProcessElement *ConvertCurves(LPIccCurve *pCurves, int nCurves, bool bStrict)
{
  CIccMpeCurveSet *pCurveSet = new CIccMpeCurveSet(nCurves);
  static icFloatNumber clipZeroParams[4] = {1.0, 0.0, 0.0, 0.0};
  static icFloatNumber clipOneParams[4] = {1.0, 0.0, 0.0, 1.0};
  
  if (pCurveSet) {
    int i;
    icFloatNumber params[10], a, b, d;

    for (i=0; i<nCurves; i++) {
      CIccSegmentedCurve *pCurve = new CIccSegmentedCurve();
      CIccFormulaCurveSegment *pFormula;
      CIccSampledCurveSegment *pSegment;
      icFloatNumber startPos, endPos;

      if (bStrict) {
        pFormula = new CIccFormulaCurveSegment(icMinFloat32Number, 0.0);
        pFormula->SetFunction(0, 4, clipZeroParams);
        pCurve->Insert(pFormula);
        startPos = 0.0;
        endPos = 1.0;
      }
      else {
        startPos = icMinFloat32Number;
        endPos = icMaxFloat32Number;
      }

      if (pCurves[i]->GetType()==icSigParametricCurveType) {
        CIccTagParametricCurve *pParCurve = (CIccTagParametricCurve*)pCurves[i];
        icS15Fixed16Number *parParams = pParCurve->GetParams();

        switch(pParCurve->GetFunctionType()) {
        case 0x0000:
          pFormula = new CIccFormulaCurveSegment(startPos, endPos);

          params[0] = icFtoD(parParams[0]);
          params[1] = 1.0;
          params[2] = 0.0;
          params[3] = 0.0;

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          break;

        case 0x0001:
          a = icFtoD(parParams[1]);
          b = icFtoD(parParams[2]);

          pFormula = new CIccFormulaCurveSegment(startPos, -b / a);
          params[0] = 1.0;
          params[1] = 0.0;
          params[2] = 0.0;
          params[3] = 0.0;

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          pFormula = new CIccFormulaCurveSegment(-b / a, endPos);
          params[0] = icFtoD(parParams[0]);
          params[1] = a;
          params[2] = b;
          params[3] = 0.0;

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          break;

        case 0x0002:
          a = icFtoD(parParams[1]);
          b = icFtoD(parParams[2]);

          pFormula = new CIccFormulaCurveSegment(startPos, -b / a);
          params[0] = 1.0;
          params[1] = 0.0;
          params[2] = 0.0;
          params[3] = icFtoD(parParams[3]);

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          pFormula = new CIccFormulaCurveSegment(-b / a, endPos);
          params[0] = icFtoD(parParams[0]);
          params[1] = a;
          params[2] = b;
          //params[3] = icFtoD(parParams[3]);  //Already set

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          break;

        case 0x0003:
          d = icFtoD(parParams[4]);

          pFormula = new CIccFormulaCurveSegment(startPos, d);
          params[0] = 1.0;
          params[1] = 0.0;
          params[2] = 0.0;
          params[3] = icFtoD(parParams[3]);

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          pFormula = new CIccFormulaCurveSegment(-b / a, endPos);
          params[0] = icFtoD(parParams[0]);
          params[1] = icFtoD(parParams[1]);
          params[2] = icFtoD(parParams[2]);
          params[3] = 0.0;

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          break;

        case 0x0004:
          d = icFtoD(parParams[4]);

          pFormula = new CIccFormulaCurveSegment(startPos, d);
          params[0] = 1.0;
          params[1] = icFtoD(parParams[3]);
          params[2] = icFtoD(parParams[6]);
          params[3] = 0.0;

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          pFormula = new CIccFormulaCurveSegment(d, endPos);
          params[0] = icFtoD(parParams[0]);
          params[1] = icFtoD(parParams[1]);
          params[2] = icFtoD(parParams[2]);
          params[3] = icFtoD(parParams[5]);
          
          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);

          break;
        }
      }
      else if (pCurves[i]->GetType()==icSigCurveType) {
        CIccTagCurve *pLutCurve = (CIccTagCurve*)pCurves[i];
        icUInt32Number nSamples = pLutCurve->GetSize();

        if (!nSamples) {
          pFormula = new CIccFormulaCurveSegment(startPos, endPos);

          params[0] = 1.0;
          params[1] = 1.0;
          params[2] = 0.0;
          params[3] = 0.0;

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);
        }
        else if (nSamples==1) {
          icFloatNumber *pData = pLutCurve->GetData(0);
          icFloatNumber gamma = (icFloatNumber)(pData[0] * 65535.0 / 256.0);

          pFormula = new CIccFormulaCurveSegment(startPos, endPos);
          params[0] = gamma;
          params[1] = 1.0;
          params[2] = 0.0;
          params[3] = 0.0;

          pFormula->SetFunction(0, 4, params);
          pCurve->Insert(pFormula);
        }
        else {
          if (!bStrict) {
            pFormula = new CIccFormulaCurveSegment(icMinFloat32Number, 0.0);
            pFormula->SetFunction(0, 4, clipZeroParams);
            pCurve->Insert(pFormula);
          }

          pSegment = new CIccSampledCurveSegment(0.0, 1.0);

          pSegment->SetSize(pLutCurve->GetSize(), false);
          memcpy(pSegment->GetSamples(), pLutCurve->GetData(0), pLutCurve->GetSize()*sizeof(icFloatNumber));

          pCurve->Insert(pSegment);

          if (!bStrict) {
            pFormula = new CIccFormulaCurveSegment(1.0, icMaxFloat32Number);
            pFormula->SetFunction(0, 4, clipOneParams);
            pCurve->Insert(pFormula);
          }
        }

      }

      if (bStrict) {
        pFormula = new CIccFormulaCurveSegment(1.0, icMaxFloat32Number);
        pFormula->SetFunction(0, 4, clipOneParams);
        pCurve->Insert(pFormula);
      }

      pCurveSet->SetCurve(i, pCurve);
    }
      
    return pCurveSet;
  }

  return NULL;
}

CIccMultiProcessElement *ScaleInput(icFloatNumber min1, icFloatNumber max1,
                               icFloatNumber min2, icFloatNumber max2,
                               icFloatNumber min3, icFloatNumber max3)
{
  CIccMpeCurveSet *pCurveSet = new CIccMpeCurveSet(3);

  if (pCurveSet) {
    CIccSegmentedCurve *pCurve;
    CIccFormulaCurveSegment *pFormula;
    icFloatNumber params[10];

    params[0] = (icFloatNumber)1.0;
    params[1] = (icFloatNumber)1.0 / (max1-min1);
    params[2] = -min1 / (max1-min1);
    params[3] = (icFloatNumber)0;
    pFormula = new CIccFormulaCurveSegment(icMinFloat32Number, icMaxFloat32Number);
    pFormula->SetFunction(0, 4, params);
    pCurve = new CIccSegmentedCurve();
    pCurve->Insert(pFormula);
    pCurveSet->SetCurve(0, pCurve);

    params[0] = (icFloatNumber)1.0;
    params[1] = (icFloatNumber)1.0 / (max2-min2);
    params[2] = -min2 / (max2-min2);
    params[3] = (icFloatNumber)0.0;
    pFormula = new CIccFormulaCurveSegment(icMinFloat32Number, icMaxFloat32Number);
    pFormula->SetFunction(0, 4, params);
    pCurve = new CIccSegmentedCurve();
    pCurve->Insert(pFormula);
    pCurveSet->SetCurve(1, pCurve);

    params[0] = (icFloatNumber)1.0;
    params[1] = (icFloatNumber)1.0 / (max3-min3);
    params[2] = min3 / (max3-min3);
    params[3] = (icFloatNumber)0.0;
    pFormula = new CIccFormulaCurveSegment(icMinFloat32Number, icMaxFloat32Number);
    pFormula->SetFunction(0, 4, params);
    pCurve = new CIccSegmentedCurve();
    pCurve->Insert(pFormula);
    pCurveSet->SetCurve(2, pCurve);
  }

  return pCurveSet;
}

CIccMultiProcessElement *ScaleOutput(icFloatNumber min1, icFloatNumber max1,
                                icFloatNumber min2, icFloatNumber max2,
                                icFloatNumber min3, icFloatNumber max3)
{
  CIccMpeCurveSet *pCurveSet = new CIccMpeCurveSet(3);

  if (pCurveSet) {
    CIccSegmentedCurve *pCurve;
    CIccFormulaCurveSegment *pFormula;
    icFloatNumber params[10];

    params[0] = 1.0;
    params[1] = max1-min1;
    params[2] = min1;
    params[3] = 0;
    pFormula = new CIccFormulaCurveSegment(icMinFloat32Number, icMaxFloat32Number);
    pFormula->SetFunction(0, 4, params);
    pCurve = new CIccSegmentedCurve();
    pCurve->Insert(pFormula);
    pCurveSet->SetCurve(0, pCurve);

    params[0] = 1.0;
    params[1] = max2-min2;
    params[2] = min2;
    params[3] = 0;
    pFormula = new CIccFormulaCurveSegment(icMinFloat32Number, icMaxFloat32Number);
    pFormula->SetFunction(0, 4, params);
    pCurve = new CIccSegmentedCurve();
    pCurve->Insert(pFormula);
    pCurveSet->SetCurve(1, pCurve);

    params[0] = 1.00;
    params[1] = max3-min3;
    params[2] = min3;
    params[3] = 0;
    pFormula = new CIccFormulaCurveSegment(icMinFloat32Number, icMaxFloat32Number);
    pFormula->SetFunction(0, 4, params);
    pCurve = new CIccSegmentedCurve();
    pCurve->Insert(pFormula);
    pCurveSet->SetCurve(2, pCurve);
  }

  return pCurveSet;
}

CIccTag* ConvertTag(CIccTag *pTag, bool bStrict, icColorSpaceSignature pcs)
{
  if (pTag && pTag->IsMBBType() && pTag->GetType()!=icSigLut16Type) {
    CIccMBB *lutTag = (CIccMBB*)pTag;
    CIccTagMultiProcessElement *pMPE = new CIccTagMultiProcessElement(lutTag->InputChannels(), lutTag->OutputChannels());
    LPIccCurve *pCurves;
    CIccMatrix *pMatrix;
    CIccCLUT *pCLUT;
    CIccMultiProcessElement *pElem;

    if (!pMPE)
      return NULL;

    if (lutTag->IsInputMatrix()) {
      if (pcs==icSigXYZData) {
        pElem = ScaleInput(minXYZ, maxXYZ, minXYZ, maxXYZ, minXYZ, maxXYZ );
        if (pElem)
          pMPE->Attach(pElem);
      }
      else if (pcs==icSigLabData) {
        pElem = ScaleInput(minL, maxL, minAB, maxAB, minAB, maxAB );
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pCurves = lutTag->GetCurvesB()) {
        pElem = ConvertCurves(pCurves, lutTag->InputChannels(), bStrict);
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pMatrix = lutTag->GetMatrix()) {
        pElem = ConvertMatrix(pMatrix);
        if (pElem)
          pMPE->Attach(pElem);
      } 

      if (pCurves = lutTag->GetCurvesM()) {
        pElem = ConvertCurves(pCurves, lutTag->InputChannels(), bStrict);
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pCLUT = lutTag->GetCLUT()) {
        pElem = ConvertCLUT(pCLUT);
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pCurves = lutTag->GetCurvesA()) {
        pElem = ConvertCurves(pCurves, lutTag->OutputChannels(), bStrict);
        if (pElem)
          pMPE->Attach(pElem);
      }

    }
    else {
      if (pCurves = lutTag->GetCurvesA()) {
        pElem = ConvertCurves(pCurves, lutTag->InputChannels(), bStrict);
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pCLUT = lutTag->GetCLUT()) {
        pElem = ConvertCLUT(pCLUT);
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pCurves = lutTag->GetCurvesM()) {
        pElem = ConvertCurves(pCurves, lutTag->OutputChannels(), bStrict);
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pMatrix = lutTag->GetMatrix()) {
        pElem = ConvertMatrix(pMatrix);
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pCurves = lutTag->GetCurvesB()) {
        pElem = ConvertCurves(pCurves, lutTag->OutputChannels(), bStrict);
        if (pElem)
          pMPE->Attach(pElem);
      }

      if (pcs==icSigXYZData) {
        pElem = ScaleOutput(minXYZ, maxXYZ, minXYZ, maxXYZ, minXYZ, maxXYZ );
        if (pElem)
          pMPE->Attach(pElem);
      }
      else if (pcs==icSigLabData) {
        pElem = ScaleOutput(minL, maxL, minAB, maxAB, minAB, maxAB );
        if (pElem)
          pMPE->Attach(pElem);
      }
    }
    return pMPE;
  }
  return NULL;
}


int main(int argc, char* argv[])
{
  CIccProfile *pProfile;
  bool bStrict = true;

  if (argc>2) {
    pProfile = ReadIccProfile(argv[1]);

    if (argc>3)
      bStrict = (atoi(argv[3])!=0);

    if (pProfile) {
      if (pProfile->m_Header.version>=icVersionNumberV4) {
        CIccTag *pTagLut, *pTagMBE;
        icColorSpaceSignature connectSig;

        if (pProfile->m_Header.deviceClass!=icSigLinkClass && 
            pProfile->m_Header.deviceClass!=icSigColorSpaceClass) {
          connectSig = pProfile->m_Header.pcs;
        }
        else
          connectSig = icSigUnknownData;

        if (pTagLut=pProfile->FindTag(icSigAToB0Tag)) {
          pTagMBE = ConvertTag(pTagLut, bStrict, connectSig);
          if (pTagMBE) {
            pProfile->AttachTag(icSigDToB0Tag, pTagMBE);
          }
        }

        if (pTagLut=pProfile->FindTag(icSigAToB1Tag)) {
          pTagMBE = ConvertTag(pTagLut, bStrict, connectSig);
          if (pTagMBE) {
            pProfile->AttachTag(icSigDToB1Tag, pTagMBE);
          }
        }

        if (pTagLut=pProfile->FindTag(icSigAToB2Tag)) {
          pTagMBE = ConvertTag(pTagLut, bStrict, connectSig);
          if (pTagMBE) {
            pProfile->AttachTag(icSigDToB2Tag, pTagMBE);
          }
        }

        if (pTagLut=pProfile->FindTag(icSigBToA0Tag)) {
          pTagMBE = ConvertTag(pTagLut, bStrict, connectSig);
          if (pTagMBE) {
            pProfile->AttachTag(icSigBToD0Tag, pTagMBE);
          }
        }

        if (pTagLut=pProfile->FindTag(icSigBToA1Tag)) {
          pTagMBE = ConvertTag(pTagLut, bStrict, connectSig);
          if (pTagMBE) {
            pProfile->AttachTag(icSigBToD1Tag, pTagMBE);
          }
        }

        if (pTagLut=pProfile->FindTag(icSigBToA2Tag)) {
          pTagMBE = ConvertTag(pTagLut, bStrict, connectSig);
          if (pTagMBE) {
            pProfile->AttachTag(icSigBToD2Tag, pTagMBE);
          }
        }

        std::string report;
        if (pProfile->Validate(report)<=icValidateWarning) {
          SaveIccProfile(argv[2], pProfile);

          CIccProfile *p2 = ReadIccProfile(argv[2]);
          delete p2;
        }
        delete pProfile;
      }
    }
  }
  else {
    printf("Usage IccV4ToMPE from_profile to_profile {strict=0/1}\n");
    return -1;
  }
  
	return 0;
}

