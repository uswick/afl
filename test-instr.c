/*
   american fuzzy lop - a trivial program to test the build
   --------------------------------------------------------

   Written and maintained by Michal Zalewski <lcamtuf@google.com>

   Copyright 2014 Google Inc. All rights reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at:

     http://www.apache.org/licenses/LICENSE-2.0

 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define MAP_SZ (1UL << 16)

char buff[MAP_SZ];

uint64_t AFLTraceUserVPN ;

static inline void PrintStats(){
  printf("SHM buffer=%p size=%lu map[0xbeef]=%u\n", buff, 
		  MAP_SZ, buff[0xbeef]);
}

int main(int argc, char** argv) {

  char buf[8];
  PrintStats();
  if (read(0, buf, 8) < 1) {
    printf("Hum?\n");
    exit(1);
  }

  if (buf[0] == '0') {
    PrintStats();
    printf("Looks like a zero to me!\n");
  }
  else{
    printf("A non-zero value? How quaint!\n");
    PrintStats();
  }

  PrintStats();
  exit(0);

}
