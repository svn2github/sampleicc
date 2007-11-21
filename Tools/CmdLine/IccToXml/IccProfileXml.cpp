#include "stdio.h"
#include "IccProfileXml.h"
#include "IccTagXml.h"
#include "IccUtil.h"
#include <set>

bool CIccProfileXml::ToXml(std::string &xml)
{
  CIccInfo info;
  char line[256];
  char buf[256];
  char fix[256];
  int n;
  bool nonzero;
  
  xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  xml += "<IccProfileXml\n";
  sprintf(line, "  Version=\"%s\"\n", info.GetVersionName(m_Header.version));
  xml += line;
  sprintf(line, "  cmmId=\"%s\"\n", icFixXml(fix, icGetSigStr(buf, m_Header.cmmId)));
  xml += line;
  sprintf(line, "  deviceClass=\"%s\"\n", icFixXml(fix, icGetSigStr(buf, m_Header.deviceClass)));
  xml += line;
  sprintf(line, "  colorSpace=\"%s\"\n", icFixXml(fix, icGetSigStr(buf, m_Header.colorSpace)));
  xml += line;
  sprintf(line, "  pcs=\"%s\"\n",  icFixXml(fix, icGetSigStr(buf, m_Header.pcs)));
  xml += line;
  sprintf(line, "  creationDate=\"%d-%d-%d %02d:%02d:%02d\"\n", m_Header.date.day,
                                                                m_Header.date.month,
                                                                m_Header.date.year,
                                                                m_Header.date.hours,
                                                                m_Header.date.minutes,
                                                                m_Header.date.seconds);
  xml += line;
  sprintf(line, "  platform=\"%s\"\n", icFixXml(fix, icGetSigStr(buf, m_Header.platform)));
  xml += line;
  sprintf(line, "  flags=\"%08X\"\n", m_Header.flags);
  xml += line;
  sprintf(line, "  manufacturer=\"%s\"\n", icFixXml(fix, icGetSigStr(buf, m_Header.manufacturer)));
  xml += line;
  sprintf(line, "  model=\"%s\"\n", icFixXml(fix, icGetSigStr(buf, m_Header.model)));
  xml += line;
  sprintf(line, "  attributes=\"%08X\"\n", m_Header.attributes);
  xml += line;
  sprintf(line, "  suggestedIntent=\"%08X\"\n", m_Header.renderingIntent);
  xml += line;
  sprintf(line, "  illuminant=\"X=%.8f, Y=%.8f, Z=%.8f\"\n", (float)icFtoD(m_Header.illuminant.X),
                                                             (float)icFtoD(m_Header.illuminant.Y),
                                                             (float)icFtoD(m_Header.illuminant.Z));
  xml += line;
  sprintf(line, "  creator=\"%s\"", icFixXml(fix, icGetSigStr(buf, m_Header.creator)));
  xml += line;

  if (m_Header.profileID.ID32[0] || m_Header.profileID.ID32[1] || 
      m_Header.profileID.ID32[2] || m_Header.profileID.ID32[3]) {
    for (n=0; n<16; n++) {
      sprintf(buf + n*2, "%02X", m_Header.profileID.ID8[n]);
    }
    buf[n*2]='\0';
    xml += "\n  profileID=\"";
    xml += buf;
    xml += "\"";
  }
  for (n=0; n<16; n++) {
    sprintf(buf + n*2, "%02X", m_Header.profileID.ID8[n]);
  }
  nonzero = false;

  for (n=0; n<sizeof(m_Header.reserved); n++) {
    if (m_Header.reserved[n])
      nonzero = true;
    sprintf(buf + n*2, "%02X", m_Header.reserved[n]);
  }
  buf[n*2]='\0';
  if (nonzero) {
    xml += "\n  reserved=\"";
    xml += buf;
  }
  xml += ">\n";

  TagEntryList::iterator i, j;
  std::set<icTagSignature> sigSet;

  for (i=m_Tags->begin(); i!=m_Tags->end(); i++) {
    if (sigSet.find(i->TagInfo.sig)==sigSet.end()) {
      CIccTag *pTag = FindTag(i->TagInfo.sig);

      if (pTag) {
        CIccTagXml *pTagXml = (CIccTagXml*)(pTag->GetExtension());
        if (pTagXml) {
          int n;
          j=i;
          sprintf(line, "  <Tag type=\"%s\"", icFixXml(fix, icGetSigStr(buf, pTag->GetType())));
          xml += line;
          sprintf(line, " signature=\"%s\"", icFixXml(fix, icGetSigStr(buf, i->TagInfo.sig)));
          xml += line;

          sigSet.insert(i->TagInfo.sig);

          for (j++, n=2; j!=m_Tags->end(); j++) {
            if (j->pTag == i->pTag || j->TagInfo.offset == i->TagInfo.offset) {
              sprintf(line, " signature%d=\"%s\"", n++, icFixXml(fix, icGetSigStr(buf, j->TagInfo.sig)));
              xml += line;
              sigSet.insert(j->TagInfo.sig);        
            }
          }
          if (pTag->m_nReserved) {
            sprintf(line, " reserved=\"%08x\"", pTag->m_nReserved);
            xml += line;
          }
          xml += "/>\n";

          if (!pTagXml->ToXml(xml, "    "))
            return false;
          xml += "  </Tag>\n";
        }
        else {
          return false;
        }
      }
      else {
        return false;
      }
    }
  }
  xml += "</IccProfileXml>\n";

  return true;
}
