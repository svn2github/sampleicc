#ifdef WIN32
#include <string.h>
#include "getopt.h"

int optind=1;
char *optarg=NULL;

int getopt_long(int argc, char * const argv[], const char *argIntrest, struct option *opts, void *unknown)
{
  int i;
  int shortOpt = -1;

  if (optind>=argc) {
    optind = 0;
    return -1;
  }

  for (i=0; opts[i].fullOption; i++) {
    if (argv[optind][0]=='-') {
      if (argv[optind][1]=='-') {
        if (!strcmp(argv[optind]+2, opts[i].fullOption)) {
          shortOpt = opts[i].shortOption;       
        }
        else if (argv[optind][1] == opts[i].shortOption)
          shortOpt = opts[i].shortOption;
      }
    }
  }

  if (shortOpt!=-1)
    ++optind;

  return shortOpt;
}
#endif
