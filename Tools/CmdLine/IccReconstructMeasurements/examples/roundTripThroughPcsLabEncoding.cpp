#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;

#include "IccUtil.h"
#include "IccToolException.h"

icFloatNumber
deltaE(icFloatNumber* first_LAB, icFloatNumber* second_LAB)
{
	icFloatNumber dL = second_LAB[0] - first_LAB[0];
	icFloatNumber da = second_LAB[1] - first_LAB[1];
	icFloatNumber db = second_LAB[2] - first_LAB[2];
  return sqrt(dL * dL + da * da + db * db);
}

int
main(int argc, char* argv[])
{
  char* LAB_pre_encoding_filename = argv[1];
  ifstream in_s(LAB_pre_encoding_filename);
  if (! in_s)
  {
    ostringstream s;
    s << "Could not open file `" << LAB_pre_encoding_filename << "' for output";
    throw IccToolException(s.str());
  }
  
  while (! in_s.eof())
	{
		string line("");
		getline(in_s, line);
		if (line == "")
			break;
		istringstream l_s(line);
		icFloatNumber orig_LAB[3];
		icFloatNumber pre_round_trip_LAB[3];
		icFloatNumber post_round_trip[3];
		l_s >> orig_LAB[0] >> orig_LAB[1] >> orig_LAB[2];
		for (unsigned int i = 0; i < 3; ++i)
			pre_round_trip_LAB[i] = orig_LAB[i];
		icLabToPcs(pre_round_trip_LAB);
		for (unsigned int i = 0; i < 3; ++i)
		{
			icUInt16Number as_in_file
				= (icUInt16Number)(max((icFloatNumber)0.0,
															 min((icFloatNumber)1.0,
																	 pre_round_trip_LAB[i])) * 65535 + 0.0);
			post_round_trip[i]
				= (icFloatNumber)((icFloatNumber)as_in_file / 65535.0);
		}
		icLabFromPcs(post_round_trip);
		cout << deltaE(orig_LAB, post_round_trip) << " "
				 << orig_LAB[0] << " "
				 << orig_LAB[1] << " "
				 << orig_LAB[2] << " "
				 << post_round_trip[0] << " "
				 << post_round_trip[1] << " "
				 << post_round_trip[2] << endl;
	}
}

