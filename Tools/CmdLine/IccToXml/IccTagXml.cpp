#include "IccTagXml.h"
#include "IccUtil.h"

#ifdef WIN32
#include <windows.h>
#ifdef GetClassName
#undef GetClassName
#endif
#endif

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

char *icFixXml(char *szDest, const char *szStr)
{
  char *ptr = szDest; 

  while (*szStr) {
    switch (*szStr) {
    case '\'':
      strcpy(ptr, "&apos;");
      ptr += 6;
      break;
    case '&':
      strcpy(ptr, "&amp;");
      ptr += 5;
      break;
    case '\"':
      strcpy(ptr, "&quot;");
      ptr += 6;
      break;
    case '<':
      strcpy(ptr, "&lt;");
      ptr += 4;
      break;
    case '>':
      strcpy(ptr, "&gt;");
      ptr += 4;
      break;
    default:
      *ptr++ = *szStr;
    }
    szStr++;
  }
  *ptr = '\0';

  return szDest;
}

char *icUtf16ToUtf8(char *szDest, int sizeDest, const icUInt16Number *szSrc, int sizeSrc=0) 
{
#ifdef WIN32
  if (!sizeSrc)
    sizeSrc = (int)wcslen(szSrc)+1;
  int n = WideCharToMultiByte(CP_UTF8, 0, szSrc, sizeSrc, szDest, sizeDest, 0, NULL);
  szDest[n] = '\0';
#endif
  return szDest;
}


bool CIccTagXmlUnknown::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  icUInt8Number *ptr = m_pData;
  char buf[15];
  int i;

  xml += blanks + "<UnknownData>\n";
  for (i=0; i<(int)m_nSize; i++, ptr++) {
    if (!(i%16)) {
      if (i)
        xml += "</H>\n";
      xml += blanks + "<H>";
    }
    sprintf(buf, "%02x", *ptr);
    xml += buf; 
  }
  if (i) {
    xml += "</H>\n";
    xml += blanks + "</UnknownData>\n";
  }
  return true;
}

bool CIccTagXmlText::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  if (strstr(m_szText, "]]>")) {
    char *ptr = m_szText;
    char buf[15];
    int i;

    for (i=0; i<(int)m_nBufSize; i++, ptr++) {
      xml += blanks + "<TextData>";
      if (!(i%16)) {
        if (i)
          xml += "</H>\n";
        xml += blanks + "<H>";
      }
      sprintf(buf, "%02x", *ptr);
      xml += buf; 
    }
    if (i) {
      xml += "</H>\n";
        xml += blanks + "</TextData>\n";
    }
  }
  else {
    xml += blanks + "<Text>";
    xml += "<![CDATA]";
    xml += m_szText;
    xml += "]]>\n";
    xml += blanks + "</Text>\n"; 
  }

  return true;
}

bool CIccTagXmlTextDescription::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char fix[256];
  char buf[256];
  char data[256];

  xml += blanks + "<TextDescription\n";
  
  sprintf(buf, "  text=\"%s\"", icFixXml(fix, m_szText));
  xml += blanks + buf;
  
  if (m_uzUnicodeText[0]) {
    sprintf(buf, "\n  unicodeRegion=\"%s\"\n", icFixXml(fix, icGetSigStr(data, m_nUnicodeLanguageCode)));
    xml += blanks + buf;

    sprintf(buf, "  UnicodeText=\"%s\"", icFixXml(fix, icUtf16ToUtf8(data, sizeof(data), m_uzUnicodeText)));
    xml += blanks + buf;
  }

  if (m_nScriptSize) {
    sprintf(buf, "\n  scriptCode=\"%08x\"\n", m_nScriptCode);
    xml += blanks + buf;

    int i;
    for (i=0; i<m_nScriptSize; i++) {
      sprintf(buf + i*2, "%02X", (unsigned char)m_szScriptText[i]);
    }
    xml += blanks + "  scriptText=\"" + buf + "\"";
  }
  xml += "\n";

  return true;
}

bool CIccTagXmlSignature::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char fix[40];
  char line[256];
  char buf[40];

  sprintf(line, "<SignatureData value=\"%s\"/>\n", icFixXml(fix, icGetSigStr(buf, m_nSig)));

  xml += blanks + line;
  return true;
}

bool CIccTagXmlNamedColor2::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char fix[256];
  char line[256];
  char buf[256];
  int i, j;

  sprintf(line, "<NamedColors vendorFlags=\"%08x\" deviceCoords=\"%d\" deviceEncodig=\"int16\"\n", m_nVendorFlags, m_nDeviceCoords);
  xml += blanks + line;
  
  sprintf(line, "  prefix=\"%s\"\n", icFixXml(fix, m_szPrefix));
  xml += blanks + line;

  sprintf(line, "  sufix=\"%s\"/>\n", icFixXml(fix, m_szPrefix));
  xml += blanks + line;

  for (i=0; i<(int)m_nSize; i++) {
    SIccNamedColorEntry *pEntry= GetEntry(i);

    if (m_csPCS==icSigLabData) {
      icFloatNumber lab[3];
      
      Lab2ToLab4(lab, pEntry->pcsCoords);
      icLabFromPcs(lab);
      sprintf(line, "  <NammedColor name=\"%s\" L=\"%.8f\" a=\"%.8f\" b=\"%.8f/>\n", 
                    icFixXml(fix, pEntry->rootName), lab[0], lab[1], lab[2]);
      xml += blanks + line;
    }
    else {
      icFloatNumber xyz[3];

      memcpy(xyz, pEntry->pcsCoords, 3*sizeof(icFloatNumber));
      icXyzFromPcs(xyz);
      sprintf(line, "  <NammedColor name=\"%s\" X=\"%.8f\" Y=\"%.8f\" Z=\"%.8f/>\n", 
        icFixXml(fix, pEntry->rootName), xyz[0], xyz[1], xyz[2]);
      xml += blanks + line;
    }

    xml += blanks + "    ";
      for (j=0; j<(int)m_nDeviceCoords; j++) {
        sprintf(buf, "<n>%d</n>", (int)(pEntry->deviceCoords[j] * 65535.0 + 0.5));
        xml += buf;
      }
      xml += "\n";

      xml += blanks + "  </NamedColor>\n";
  }
  return true;
}

bool CIccTagXmlXYZ::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[256];
  int i;

  for (i=0; i<(int)m_nSize; i++) {
    sprintf(buf, "<XYZNumber X=\"%.8f\" Y=\"%.8f\" Z=\"%.8f\"/>\n", i, (float)icFtoD(m_XYZ[i].X),
                                                                      (float)icFtoD(m_XYZ[i].Y),
                                                                      (float)icFtoD(m_XYZ[i].Z));
    xml += blanks + buf;
  }
  return true;
}

bool CIccTagXmlChromaticity::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[256];
  int i;

  for (i=0; i<(int)m_nChannels; i++) {
    sprintf(buf, "<Chromaticity x=\"%.8f\" y=\"%.8f\"/>\n", i, (float)icUFtoD(m_xy[i].x),
                                                               (float)icUFtoD(m_xy[i].y));
    xml += blanks + buf;
  }
  return true;
}

template <class T, icTagTypeSignature Tsig>
const icChar* CIccTagXmlFixedNum<T, Tsig>::GetClassName()
{
  if (Tsig==icSigS15Fixed16ArrayType)
    return "CIccTagXmlS15Fixed16";
  else 
    return "CIccTagXmlU16Fixed16";
}

template <class T, icTagTypeSignature Tsig>
bool CIccTagXmlFixedNum<T, Tsig>::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[256];
  int i;

  if (Tsig==icSigS15Fixed16ArrayType) {
    for (i=0; i<(int)m_nSize; i++) {
      sprintf(buf, "<F>%.8f</F>\n", (float)icFtoD(m_Num[i]));
      xml += blanks + buf;
    }
  }
  else {
    for (i=0; i<(int)m_nSize; i++) {
      sprintf(buf, "<F>%.8f</F>\n", (float)icUFtoD(m_Num[i]));
      xml += blanks + buf;
    }
  }
  return true;
}

//Make sure typedef classes get built
template class CIccTagXmlFixedNum<icS15Fixed16Number, icSigS15Fixed16ArrayType>;
template class CIccTagXmlFixedNum<icU16Fixed16Number, icSigU16Fixed16ArrayType>;

template <class T, icTagTypeSignature Tsig>
const icChar *CIccTagXmlNum<T, Tsig>::GetClassName()
{
  if (sizeof(T)==sizeof(icUInt8Number))
    return "CIccTagXmlUInt8";
  else if (sizeof(T)==sizeof(icUInt16Number))
    return "CIccTagXmlUInt16";
  else if (sizeof(T)==sizeof(icUInt32Number))
    return "CIccTagXmlUInt32";
  else if (sizeof(T)==sizeof(icUInt64Number))
    return "CIccTagXmlUInt64";
  else
    return "CIccTagXmlNum<>";
}

template <class T, icTagTypeSignature Tsig>
bool CIccTagXmlNum<T, Tsig>::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[256];
  int i;

  if (sizeof(T)==1) {
    for (i=0; i<(int)m_nSize; i++) {
      sprintf(buf, "<N>%02X</N>\n", m_Num[i]);
      xml += blanks + buf;
    }
  }
  else if (sizeof(T)==2) {
    for (i=0; i<(int)m_nSize; i++) {
      sprintf(buf, "<N>%04x</N>\n", m_Num[i]);
      xml += blanks + buf;
    }
  }
  else if (sizeof(T)==4) {
    for (i=0; i<(int)m_nSize; i++) {
      sprintf(buf, "<N>%08x</N>\n", m_Num[i]);
      xml += blanks + buf;
    }
  }
  else if (sizeof(T)==8) {
    for (i=0; i<(int)m_nSize; i++) {
      sprintf(buf, "<N>%016lx</N>\n", m_Num[i]);
      xml += blanks + buf;
    }
  }
  return true;
}

//Make sure typedef classes get built
template class CIccTagXmlNum<icUInt8Number, icSigUInt8ArrayType>;
template class CIccTagXmlNum<icUInt16Number, icSigUInt16ArrayType>;
template class CIccTagXmlNum<icUInt32Number, icSigUInt32ArrayType>;
template class CIccTagXmlNum<icUInt64Number, icSigUInt64ArrayType>;

bool CIccTagXmlMeasurement::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[256];
  sprintf(buf, "<Measurement stdObserver=\"%d\" backingX=\"%.8f\" backingY=\"%.8f\" backingZ=\"%.8f\"",
               m_Data.stdObserver, m_Data.backing.X, m_Data.backing.Y, m_Data.backing.Z);
  xml += blanks + buf;

  sprintf(buf, "  geometry=\"%d\" flare=\"%d\" illuminant=\"%d\" seconds=\"%d\"/>\n",
               m_Data.geometry, m_Data.flare, m_Data.illuminant, m_Data.illuminant);
  xml += blanks + buf;
  return true;
}

bool CIccTagXmlMultiLocalizedUnicode::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[256];
  char data[256];
  char fix[256];
  CIccMultiLocalizedUnicode::iterator i;

  if (!m_Strings)
    return false;

  for (i=m_Strings->begin(); i!=m_Strings->end(); i++) {
    sprintf(buf, "<LocalizedText languageCountry=\"%s\"", icFixXml(fix, icGetSigStr(data, (i->m_nLanguageCode<<16) + i->m_nCountryCode)));
    xml += blanks + buf;
    
    sprintf(buf, " text=\"%s\"/>\n", icFixXml(fix, icUtf16ToUtf8(buf, sizeof(buf), i->GetBuf(), i->GetLength())));
    xml += buf;
  }
  return true;
}

bool CIccTagXmlData::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  icUInt8Number *ptr = m_pData;
  char buf[15];
  int i;

  sprintf(buf, "%08x", m_nDataFlag);
  xml += blanks + "<Data flag=\"" + buf + "\">\n";
  for (i=0; i<(int)m_nSize; i++, ptr++) {
    if (!(i%16)) {
      if (i)
        xml += "</H>\n";
      xml += blanks + "<H>";
    }
    sprintf(buf, "%02x", *ptr);
    xml += buf; 
  }
  if (i) {
    xml += "</H>\n";
    xml += blanks + "</Data>\n";
  }
  return true;
}

bool CIccTagXmlDateTime::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[256];
  sprintf(buf, "<DateTime year=\"%d\" month=\"%d\" day=\"%d\" hour=\"%d\" minutes=\"%d\" seconds=\"%d\"/>\n",
               m_DateTime.year, m_DateTime.month, m_DateTime.day, m_DateTime.hours, m_DateTime.minutes, m_DateTime.seconds);
  xml += blanks + buf;
  return true;
}

bool CIccTagXmlColorantOrder::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[40];
  int i;

  xml += blanks + "<ColorantOrder>\n" + blanks + "  ";
  for (i=0; i<(int)m_nCount; i++) {
    sprintf(buf, "<n>%d</n>\n", m_pData[i]);
    xml += buf;
  }
  xml += "\n";
  xml += blanks + "</ColorantOrder>\n";

  return true;
}

bool CIccTagXmlColorantTable::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[80];
  char fix[80];
  int i;

  xml += blanks + "<ColorantTable>\n";
  for (i=0; i<(int)m_nCount; i++) {
    icFloatNumber lab[3];
    memcpy(lab, m_pData[i].data, 3*sizeof(icFloatNumber));
    icLabFromPcs(lab);
    sprintf(buf, "  <Colorant name=\"%s\" L=\"%.8f\" a=\"%.8f\" b=\"%f\"/n>\n",
                    icFixXml(fix, m_pData[i].name), lab[0], lab[1], lab[2]);
    xml += blanks + buf;
  }
  xml += "\n";
  xml += blanks + "</ColorantOrder>\n";

  return true;
}

bool CIccTagXmlViewingConditions::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[256];
  xml += blanks + "<ViewingConditions\n";

  sprintf(buf, "  illumX=\"%.8f\" illumY=\"%.8f\" illumZ=\"%.8f\"",
               icFtoD(m_XYZIllum.X), icFtoD(m_XYZIllum.Y), icFtoD(m_XYZIllum.Z));
  xml += blanks + buf;

  sprintf(buf, "  surroundX=\"%.8f\" surroundY=\"%.8f\" surroundZ=\"%.8f\"",
    icFtoD(m_XYZSurround.X), icFtoD(m_XYZSurround.Y), icFtoD(m_XYZSurround.Z));
  xml += blanks + buf;

  sprintf(buf, "  illumType=\"%d\"/>\n", m_illumType);
  xml += blanks + buf;

  return true;
}

bool icProfDescToXml(std::string &xml, CIccProfileDescStruct &p, std::string blanks = "")
{
  char fix[256];
  char buf[256];
  char data[256];

  sprintf(buf, "<ProfileDesc deviceMfg=\"%s\"", icFixXml(fix, icGetSigStr(data, p.m_deviceMfg)));
  xml += blanks + buf;

  sprintf(buf,  " deviceModel=\"%s\"", icFixXml(fix, icGetSigStr(data, p.m_deviceModel)));
  xml += buf;

  sprintf(buf, "  attributes=\"%016lX\" technology=\"%s\"/>\n", p.m_attributes, icFixXml(fix, icGetSigStr(data, p.m_technology)));
  xml += buf;

  CIccTag *pTag = p.m_deviceMfgDesc.GetTag();

  if (!pTag)
    return false;

  CIccTagXml *pExt = (CIccTagXml*)(pTag->GetExtension());

  if (!pExt || !pExt->GetExtClassName() || strcmp(pExt->GetExtClassName(), "CIccTagExt"))
    return false;

  sprintf(buf, "  <DeviceMfgDesc type=\"%s\">\n", pTag->GetType());
  xml += blanks + buf;;

  if (!pExt->ToXml(xml, blanks + "    "))
    return false;

  xml += blanks + "  </DeviceMfgDesc>\n";

  pTag = p.m_deviceModelDesc.GetTag();

  if (!pTag)
    return false;

  pExt = (CIccTagXml*)(pTag->GetExtension());

  if (!pExt || !pExt->GetExtClassName() || strcmp(pExt->GetExtClassName(), "CIccTagExt"))
    return false;

  sprintf(buf, "  <DeviceModelDesc type=\"%s\">\n", pTag->GetType());
  xml += blanks + buf;;

  if (!pExt->ToXml(xml, blanks + "    "))
    return false;

  xml += blanks + "  </DeviceModelDesc>\n";

  xml += blanks + "</ProfileDesc>\n";

  return true;
}

bool CIccTagXmlProfileSeqDesc::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  CIccProfileSeqDesc::iterator i;
  if (!m_Descriptions) 
    return false;

  xml += blanks + "<ProfileSeqDesc>\n";
  for (i=m_Descriptions->begin(); i!=m_Descriptions->end(); i++) {
    if (!icProfDescToXml(xml, *i, blanks + "  "))
      return false;
  }
  xml += blanks + "</ProfileSeqDesc>\n";
  return true;
}

bool CIccTagXmlResponseCurveSet16::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char fix[80];
  char line[80];
  char data[80];
  int i;

  sprintf(line, "<ResponseCurveSet channels=\"%d\">\n", m_nChannels);
  xml += blanks + line;

  CIccResponseCurveStruct *pCurves=GetFirstCurves();
  while (pCurves) {
    sprintf(line, "  <ResponseCurves measurementType=\"%s\">\n", icFixXml(fix, icGetSigStr(data, pCurves->GetMeasurementType())));
    xml += blanks + line;
    for (i=0; i<pCurves->GetNumChannels(); i++) {
      CIccResponse16List *pResponseList = pCurves->GetResponseList(i);
      sprintf(line, "    <ChannelResponses index=\"%d\">", i);
      xml += blanks + line;

      CIccResponse16List::iterator j;
      for (j=pResponseList->begin(); j!=pResponseList->end(); j++) {
        sprintf(line, "      <Measurement deviceCode=\"%d\" value=\"%.8f\"", j->deviceCode, icFtoD(j->measurementValue));
        xml += blanks + line;

        if (j->reserved) {
          sprintf(line, " reserved=\"%d\"", j->reserved);
          xml += line;
        }
        xml += "/>\n";
      }

      xml += blanks + "    </ChannelRespones>\n";
    }
    xml += blanks + "  </ResponseCurves>\n";
    pCurves = GetNextCurves();
  }

  xml += blanks + "</ResponseCurveSet\n";
  return true;
}

bool CIccTagXmlCurve::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[40];
  int i;

  xml += blanks + "<Curve type=\"int16\">\n" + blanks + "  ";
  for (i=0; i<(int)m_nSize; i++) {
    if (i && !(i%10)) {
      xml += "\n";
      xml += blanks + "  ";
    }
    sprintf(buf, "<n>%d</n>", (int)(m_Curve[i] * 65535.0 + 0.5));
  }
  xml += "\n";
  xml += blanks = "</Curve>\n";
  return true;
}

bool CIccTagXmlParametricCurve::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  char buf[80];
  int i;

  sprintf(buf, "<ParametricCurve functionType=\"%d\"", m_nFunctionType);
  xml += blanks + buf;

  if (m_nReserved2) {
    sprintf(buf, " reserved=\"%d\"", m_nReserved2);
    xml += buf;
  }
  xml += ">\n";
  xml += blanks + "  ";

  for (i=0; i<(int)m_nNumParam; i++) {
     sprintf(buf, "<F>%.8f</F>", m_dParam[i]);
     xml += buf;
  }

  sprintf(buf, "</ParametricCurve>\n");
  xml += blanks + buf;

  return true;
}

typedef enum {
  icConvert8Bit=0,
  icConvert16Bit,
  icConvertFloat,
  icConvertVariable
} icConvertType;

class CIccDumpXmlCLUT : public IIccCLUTExec
{
public:
  CIccDumpXmlCLUT(std::string *xml, icConvertType nType, std::string blanks, icUInt8Number nSamples)
  {
    m_xml = xml;
    m_nType = nType;
    m_blanks = blanks;
    m_nSamples = nSamples;
  }

  virtual void PixelOp(icFloatNumber* pGridAdr, icFloatNumber* pData)
  {
    int i;
    char buf[128];

    switch(m_nType) {
      case icConvert8Bit:
        *m_xml += m_blanks;
        for (i=0; i<m_nSamples; i++) {
          sprintf(buf, "<N>%d</N>", (icUInt8Number)(pData[i]*255.0 + 0.5));
          *m_xml += buf;
        }
        *m_xml += "\n";
        break;
      case icConvert16Bit:
        *m_xml += m_blanks;
        for (i=0; i<m_nSamples; i++) {
          sprintf(buf, "<N>%d</N>", (icUInt8Number)(pData[i]*65535.0 + 0.5));
          *m_xml += buf;
        }
        *m_xml += "\n";
        break;
      case icConvertFloat:
      default:
        *m_xml += m_blanks;
        for (i=0; i<m_nSamples; i++) {
          sprintf(buf, "<F>%.8f</F>", pData[i]);
          *m_xml += buf;
        }
        *m_xml += "\n";
        break;
    }
  }

  std::string *m_xml;
  icConvertType m_nType;
  std::string m_blanks;
  icUInt8Number m_nSamples;
};

bool icCLUTToXml(std::string &xml, CIccCLUT *pCLUT, icConvertType nType, std::string blanks, bool bSaveGridPrecision=false)
{
  char buf[128];
  if (nType == icConvertVariable) {
    nType = pCLUT->GetPrecision()==1 ? icConvert8Bit : icConvert16Bit;
  }

  xml += blanks + "<CLUT\n";

  switch (nType) 
  {
    case icConvert8Bit:
      xml += blanks + "  Type=\"8Bit\"\n";
      break;

    case icConvert16Bit:
      xml += blanks + "  Type=\"16Bit\"\n";
      break;

    case icConvertFloat:
      xml += blanks + "  Type=\"Float\"\n";
      break;

    default:
      break;
  }

  sprintf(buf, "  InputChannels=\"%d\"\n", pCLUT->GetInputDim());
  xml += blanks + buf;

  sprintf(buf, "  OutputChannels=\"%d\"/>\n", pCLUT->GetOutputChannels());
  xml += blanks + buf;

  if (bSaveGridPrecision) {
    xml += blanks + "  <GridPoints>";
    int i;

    for (i=0; i<pCLUT->GetInputDim(); i++) {
      if (i)
        xml += " ";
      sprintf(buf, "%d", pCLUT->GridPoint(i));
      xml += buf;
    }
    xml += "</GridPoints>\n";
  }
  
  CIccDumpXmlCLUT dumper(&xml, nType, blanks + "    ", pCLUT->GetOutputChannels());
  
  xml += "  <TableData>\n";
  pCLUT->Iterate(&dumper);
  xml += "  </TableData\n";

  xml += blanks + "</CLUT>\n";
  return true;
}

bool icCurvesToXml(std::string &xml, const char *szName, CIccCurve **pCurves, int numCurve, icConvertType nType, std::string blanks)
{
  if (pCurves) {
    int i;
    
    xml += blanks + "<" + szName + ">\n";
    for (i=0; i<numCurve; i++) {
      IIccExtensionTag *pTag = pCurves[i]->GetExtension();
      if (!pTag || strcmp(pTag->GetExtClassName(), "IIccExtensionTag"))
        return false;

      if (!((CIccTagXml *)pTag)->ToXml(xml, blanks + "  "))
        return false;
    }
    xml += blanks + "</" + szName + ">\n";
  }
  return true;
}

bool icMatrixToXml(std::string &xml, CIccMatrix *pMatrix, std::string blanks)
{
  char buf[128];
  xml += blanks + "<Matrix\n";

  sprintf(buf, "  e00=\"%.8f\" e01=\"%.8f\" e02=\"%.8f\"\n", pMatrix->m_e[0], pMatrix->m_e[1], pMatrix->m_e[2]);
  xml += blanks + buf;

  sprintf(buf, "  e10=\"%.8f\" e11=\"%.8f\" e12=\"%.8f\"\n", pMatrix->m_e[3], pMatrix->m_e[4], pMatrix->m_e[5]);
  xml += blanks + buf;

  sprintf(buf, "  e20=\"%.8f\" e21=\"%.8f\" e22=\"%.8f\"/>\n", pMatrix->m_e[6], pMatrix->m_e[7], pMatrix->m_e[8]);
  xml += blanks + buf;

  if (pMatrix->m_bUseConstants) {
    sprintf(buf, "</MatrixConstants c0=\"%.8f\" c1=\"%.8f\" c2=\"%.8f\"/>\n", pMatrix->m_e[9], pMatrix->m_e[10], pMatrix->m_e[11]);
    xml += blanks + buf;

  }
  return true;
}

bool icMBBToXml(std::string &xml, CIccMBB *pMBB, icConvertType nType, std::string blanks="", bool bSaveGridPrecision=false)
{
  blanks += "  ";
  if (pMBB->IsInputMatrix()) {
    if (pMBB->GetCurvesB()) {
      icCurvesToXml(xml, "BCurves", pMBB->GetCurvesB(), 3, nType, blanks);
    }

    if (pMBB->GetMatrix()) {
      icMatrixToXml(xml, pMBB->GetMatrix(), blanks);
    }

    if (pMBB->GetCurvesM()) {
      icCurvesToXml(xml, "MCurves", pMBB->GetCurvesM(), 3, nType, blanks);
    }

    if (pMBB->GetCLUT()) {
      icCLUTToXml(xml, pMBB->GetCLUT(), nType, blanks, bSaveGridPrecision);
    }

    if (pMBB->GetCurvesA()) {
      icCurvesToXml(xml, "ACurves", pMBB->GetCurvesA(), pMBB->OutputChannels(), nType, blanks);
    }
  }
  else {
    if (pMBB->GetCurvesA()) {
      icCurvesToXml(xml, "ACurves", pMBB->GetCurvesA(), pMBB->InputChannels(), nType, blanks);
    }

    if (pMBB->GetMatrix()) {
      icMatrixToXml(xml, pMBB->GetMatrix(), blanks);
    }

    if (pMBB->GetCurvesM()) {
      icCurvesToXml(xml, "MCurves", pMBB->GetCurvesM(), 3, nType, blanks);
    }

    if (pMBB->GetCLUT()) {
      icCLUTToXml(xml, pMBB->GetCLUT(), nType, blanks, bSaveGridPrecision);
    }

    if (pMBB->GetCurvesB()) {
      icCurvesToXml(xml, "BCurves", pMBB->GetCurvesB(), 3, nType, blanks);
    }
  }
  return true;
}

bool CIccTagXmlLutAtoB::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  std::string info;
  
  xml += blanks + "<LutAtoB>\n";

  bool rv = icMBBToXml(xml, this, icConvertVariable, blanks + "  ", true);

  xml += blanks + "</LutAtoB>\n";

  return rv;
}

bool CIccTagXmlLutBtoA::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  std::string info;

  xml += blanks + "<LutBtoA>\n";

  bool rv = icMBBToXml(xml, this, icConvertVariable, blanks + "  ", true);

  xml += blanks + "</LutBtoA>\n";

  return rv;
}

bool CIccTagXmlLut8::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  std::string info;

  xml += blanks + "<Lut8>\n";

  bool rv = icMBBToXml(xml, this, icConvert8Bit, blanks + "  ");

  xml += blanks + "</Lut8>\n";

  return rv;
}

bool CIccTagXmlLut16::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  std::string info;

  xml += blanks + "<Lut16>\n";

  bool rv = icMBBToXml(xml, this, icConvert16Bit, blanks + "  ");

  xml += blanks + "</Lut16>\n";

  return rv;
}

bool CIccTagXmlMultiProcessElement::ToXml(std::string &xml, std::string blanks/* = ""*/)
{
  std::string info;

  xml += blanks + "<Class name=\"" + GetClassName() + "\"/>\n";
  return true;
}


#ifdef USESAMPLEICCNAMESPACE
}
#endif
