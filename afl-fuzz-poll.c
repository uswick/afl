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

/*static afl_server_config_t aflGlobalConf;*/
static bool init_done = 0;
/*static sem_t sem_wait_clientreq; */
/*static sem_t sem_wait_runfuzzed; */
static pthread_t server_th;

static volatile u8 poll_wait_clientreq  = 0;
static volatile u8 poll_wait_runnfuzzed = 0;

void* server_thread(void*  arg){
  afl_server_config_t *conf = arg;
  init_afl_server(conf);
  return NULL;
}

int init_server(afl_server_config_t *conf){
  int ret = 0;
  // only create one server thread
  if(!init_done){
   ret = pthread_create(&server_th, NULL, server_thread, conf);  
   if(ret){
     WARNF("AFL Server #init(). Unable to setup server");
     ret = 1;
     goto exit_server;
   }
   barrier_afl_client();
   init_done = 1;
  }
exit_server:
  return ret;
}


int exit_server(){
  if(init_done){
    pthread_cancel(server_th);
  }
  return init_done;
}

int barrier_afl_server(afl_server_cmd_t cmd){
  /*sem_wait(&sem_wait_clientreq);*/
  /*sem_post(&sem_wait_runfuzzed);*/
  while(!poll_wait_clientreq);
  MEM_BARRIER();
  poll_wait_clientreq = 0;
  poll_wait_runnfuzzed = 1;
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
  poll_wait_clientreq = 1;
  MEM_BARRIER();
  while(!poll_wait_runnfuzzed);
  poll_wait_runnfuzzed = 0;
  /*sem_post(&sem_wait_clientreq);*/
  /*sem_wait(&sem_wait_runfuzzed);*/
  return 0;
}


