#include <iostream>
#include <string>
#include <sstream>
#include <limits>
using namespace std;

void
usage(ostream& s, const string& myName)
{
  s << myName << ": usage is " << myName << " -lattice|-r|-g|-b|-w N" << endl;
}

int
main(int argc, char* argv[])
{
  string myName(argv[0]);
  if (argc != 3)
    {
      usage(cerr, myName);
      return EXIT_FAILURE;
    }
  string pattern(argv[1]);
  if (pattern != "-lattice"
      && pattern != "-r"
      && pattern != "-g"
      && pattern != "-b"
      && pattern != "-w")
    {
      usage(cerr, myName);
      return EXIT_FAILURE;
    }
  int N = numeric_limits<int>::max();
  istringstream s(argv[2]);
  s >> N;
  if (N == numeric_limits<int>::max())
    {
      usage(cerr, myName);
      return EXIT_FAILURE;
    }

  if (pattern == "-lattice")
    {
      for (unsigned int i = 0; i < N; ++i)
	for (unsigned int j = 0; j < N; ++j)
	  for (unsigned int k = 0; k < N; ++k)
	    cout << static_cast<float>(i) / (N - 1) << " "
		 << static_cast<float>(j) / (N - 1) << " "
		 << static_cast<float>(k) / (N - 1) << endl;

    }
  else if (pattern == "-r")
      for (int i = 0; i < N; ++i)
	cout << static_cast<float>(i) / (N - 1) << " 0 0" << endl;
  else if (pattern == "-g")
      for (int i = 0; i < N; ++i)
	cout << "0 " << static_cast<float>(i) / (N - 1) << " 0" << endl;
  else if (pattern == "-b")
      for (int i = 0; i < N; ++i)
	cout << "0 0 " << static_cast<float>(i) / (N - 1) << endl;
  else if (pattern == "-w")
      for (int i = 0; i < N; ++i)
  	  cout << static_cast<float>(i) / (N - 1) << " "
  	       << static_cast<float>(i) / (N - 1) << " "
  	       << static_cast<float>(i) / (N - 1) << endl;
  else
    {
      cerr << "Error: request to generate device codes in pattern other than"
	   << " -lattice, -r, -g, -b or -w" << endl;
      return EXIT_FAILURE;
    }
      
  return EXIT_SUCCESS;
}
