#ifndef __H_AFL_FUZZ_PROXY__H__
#define __H_AFL_FUZZ_PROXY__H__

typedef struct afl_server_config {
  char *input_dir;
  char *output_dir;
  char *target_name;
  int dumb_mode;
  int t_argc;
  char **t_argv;
  u8 not_on_tty;
  u8 afl_no_cpu_red;
  u8 afl_no_arith;
  u8 afl_shuffle_q;
  u8 afl_fast_calc;
  u8* afl_sync_id;
} afl_server_config_t;

typedef enum {
  AFL_SERVER_INIT,
  AFL_SERVER_NEXT,
  AFL_SERVER_MAX
} afl_server_cmd_t;

//typedef struct afl_server_session {
  //u8* afl_fuzzed_mem;
//} afl_server_session_t;


//struct afl_server_session;
/*
 * entry point for starting AFL server
 * */
int init_afl_server(afl_server_config_t *config);

u8* get_afl_fuzzed();
/*
 * let an AFL client know that fuzzed output is ready for consumption
 * And also waits until end of client target completion
 * */
int wait_afl_fuzz_ready(afl_server_cmd_t cmd);

/*
 *  wait until AFL server and client are
 *  in lockstep
 * * */
int barrier_afl_server(afl_server_cmd_t cmd);

#endif
