#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "types.h"
#include "afl-fuzzserver.h"

volatile uint64_t val = 0;

static void fuzzer_init(){
  int ret;
  printf("Initializing the AFL fuzzer\n");
  ret = init_server(NULL);	
  
  // signal barrier
  // make sure initialization is completed
  // properly before the first run
  barrier_afl_client();
}

static void fuzzer_exit(){
  exit_server();
  printf("Exiting the fuzzer\n");
  fflush(stdout);
}

// for the first run client target has not
// run yet
static bool client_target_comp = false;
static void fuzzer_next() {
  // sync with server output
  barrier_afl_client();
  if(client_target_comp){
    // wait until client complete
    // client has completed each time fuzzer_next is called
    // except for the very first run 
    // --> thus r==0 --> client_target_comp=false
    barrier_afl_client();
  }
  client_target_comp = true;
  printf("Next the fuzzer input is : %lu\n", val);
  fflush(stdout);
  // do some stuff
}


int main(int argc, char *argv[])
{
  int i = 100;
  int ret = 0;
  fuzzer_init();

  while(i--){
    fuzzer_next();	  
    sleep(3);
  }

  fuzzer_exit();
  return ret;
}
