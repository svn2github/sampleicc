// IccToXml.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "IccTagXmlFactory.h"
#include "IccProfileXml.h"
#include "IccIO.h"

int main(int argc, char* argv[])
{
  if (argc<=1) {
    printf("Usage: IccToXml profile");
    return -1;
  }
  
  CIccTagCreator::PushFactory(new CIccTagXmlFactory());

  CIccProfileXml profile;
  CIccFileIO IO;

  if (!IO.Open(argv[1], "r")) {
    printf("Unable to open '%s'\n", argv[1]);
    return -1;
  }

  if (!profile.Read(&IO)) {
    printf("Unable to read '%s'\n", argv[1]);
    return -1;
  }

  std::string xml;

  if (!profile.ToXml(xml)) {
    printf("Unable to convert '%s' to xml\n", argv[1]);
    return -1;
  }

  fwrite((char*)xml.c_str(), xml.size(), 1, stdout);

	return 0;
}

