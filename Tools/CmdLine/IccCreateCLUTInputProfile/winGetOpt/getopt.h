#ifndef _GETOPT_H
#define _GETOPT_H

#ifdef WIN32
#include <stdio.h>
#include <assert.h>

struct option {
  const char *fullOption;
  int argType;
  void *something;
  char shortOption;
};

extern int optind;
extern char *optarg;

#define optional_argument 1
#define required_argument 2

int getopt_long(int argc, char * const argv[], const char *argIntrest, struct option *opts, void *unknown);

#endif


#endif
