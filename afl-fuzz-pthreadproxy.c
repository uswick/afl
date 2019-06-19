#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "config.h"
#include "types.h"
#include "debug.h"
#include "alloc-inl.h"
#include "afl-fuzzserver.h"

afl_server_config_t aflGlobalConf;
int server_pid;
bool init_done = 0;
sem_t sem_wait_clientreq; 
sem_t sem_wait_runfuzzed; 
pthread_t server_th;
volatile u64 val = 0;


static u8 wait_client_copy_fuzzed(afl_server_cmd_t cmd, void* mem, u32 len) {
  if(cmd == AFL_SERVER_INIT) {
    barrier_afl_server(cmd);	
  } else if (cmd == AFL_SERVER_NEXT) {
    // TODO some stuff
    // fuzzed input ready
    wait_afl_fuzz_ready(cmd);
  }
  return 0;
}

int init_afl_server(afl_server_config_t *config){
  printf("initializing afl-server\n");
  fflush(stdout);

  // init point
  wait_client_copy_fuzzed(AFL_SERVER_INIT, 0, 0);
  while(1){
    val++;
    wait_client_copy_fuzzed(AFL_SERVER_NEXT, (void*)&val, 8);
    sleep(1);
  }
  return 0;
}

void* server_thread(void*  arg){
  int ret;
  afl_server_config_t *conf = arg;
  ret = init_afl_server(conf);
  return NULL;
}

int init_server(){
  int ret = 0;
  // only create one server thread
  if(!init_done){
   ret = sem_init(&sem_wait_clientreq, 0, 0);
   ret = sem_init(&sem_wait_runfuzzed, 0, 0);
   if(ret){
     WARNF("AFL Server #init(). Unable to setup sync");
     ret = 1;
     goto exit_server;
   }
   
   pthread_create(&server_th, NULL, server_thread, &aflGlobalConf);  
   init_done = 1;
  }
exit_server:
  return ret;
}

int barrier_afl_server(afl_server_cmd_t cmd){
  sem_wait(&sem_wait_clientreq);
  sem_post(&sem_wait_runfuzzed);
}

int wait_afl_fuzz_ready(afl_server_cmd_t cmd){
  // sync with client request start
  barrier_afl_server(cmd);
  // wait for client completion
  barrier_afl_server(cmd);
}

/******************************
 * 
 *  Client Code
 * ******************************/

static int barrier_afl_client(){
  sem_post(&sem_wait_clientreq);
  sem_wait(&sem_wait_runfuzzed);
}

static void fuzzer_init(){
  int ret;
  printf("Initialized the fuzzer\n");
  ret = init_server();	
  
  // signal barrier
  // make sure initialization is completed
  // properly before the first run
  barrier_afl_client();
}

static void fuzzer_exit(){
  int ret;
  ret = pthread_cancel(server_th);
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
  printf("Next the fuzzer input is : %llu\n", val);
  fflush(stdout);
  // do some stuff
}

/*
  This is a program to test AFL server mode

  cmd == init
  -----------
          
   | AFL    |  _____ init ____ | Client |  
   | Server | 

    - start server loop
                                 - create server thread
	                         - wait until reasonably initialized ?      


  cmd == next
  -----------
   | AFL    |  _____ init ____ | Client |  
   | Server | 
   				- client req next
    - wait for client 
      once fuzz is 
      generated                             	           					
    - wait for target complete
                                - client routine exits

   				- client req next
				  -now client run should have completed

 * */
int main(int argc, char *argv[])
{
  int i = 10;
  int ret = 0;
  fuzzer_init();

  while(i--){
    fuzzer_next();	  
    sleep(10);
  }

  fuzzer_exit();
  return ret;
}
