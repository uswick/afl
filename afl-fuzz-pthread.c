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

static afl_server_config_t aflGlobalConf;
static bool init_done = 0;
static sem_t sem_wait_clientreq; 
static sem_t sem_wait_runfuzzed; 
static pthread_t server_th;


/*
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
*/

void* server_thread(void*  arg){
  int ret;
  afl_server_config_t *conf = arg;
  ret = init_afl_server(conf);
  return NULL;
}

int init_server(afl_server_config_t *conf){
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
   
   aflGlobalConf.input_dir = "/mytrees/myafl/afl/test_in";
   aflGlobalConf.output_dir = "/mytrees/myafl/afl/test_out";
   aflGlobalConf.target_name = "test-afl-server";
   aflGlobalConf.not_on_tty = 1;

   pthread_create(&server_th, NULL, server_thread, &aflGlobalConf);  
   init_done = 1;
  }
exit_server:
  return ret;
}


int exit_server(){
  pthread_cancel(server_th);
}

int barrier_afl_server(afl_server_cmd_t cmd){
  sem_wait(&sem_wait_clientreq);
  sem_post(&sem_wait_runfuzzed);
  return 0;
}

int wait_afl_fuzz_ready(afl_server_cmd_t cmd){
  // sync with client request start
  barrier_afl_server(cmd);
  // wait for client completion
  barrier_afl_server(cmd);
  return 0;
}

/******************************
 * 
 *  Client Code
 * ******************************/

int barrier_afl_client(){
  sem_post(&sem_wait_clientreq);
  sem_wait(&sem_wait_runfuzzed);
  return 0;
}


