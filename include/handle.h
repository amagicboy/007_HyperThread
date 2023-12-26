#ifndef __HANDLE_H__
#define __HANDLE_H__
#endif

#define _GNU_SOURCE
// #define __USE_GNU
/* If the first argument of `dlsym' or `dlvsym' is set to RTLD_NEXT
   the run-time address of the symbol called NAME in the next shared
   object is returned.  The "next" relation is defined by the order
   the shared objects were loaded.  */
# define RTLD_NEXT	((void *) -1l)
#define T0_CPU 15
#define T1_CPU 47
#ifndef MAX_THREADS
#define MAX_THREADS 1024
#endif
#define __NR_gettid 186

#undef pthread_create
//取消对 pthread_create 宏的定义，准备重新定义它

#include <stdio.h>
#include <stdint.h>
#include <bits/pthreadtypes.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>
#include "numap.h"
#include<time.h>
#include <errno.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/stat.h>


#ifndef __pid_t_defined
typedef __pid_t pid_t;
# define __pid_t_defined
#endif

extern struct numap_sampling_measure gSm;
extern struct numap_counting_measure gCm;
extern __thread int fd;

extern __thread struct numap_sampling_measure sm;
extern __thread struct numap_sampling_measure sm_wr;
extern struct thread_info *thread_ranks;

extern pthread_mutex_t thread_lock;
/* array describing the binding of each thread */
extern int thread_bindings[MAX_THREADS];
extern __thread unsigned thread_rank;

static int  (*real_create) (pthread_t *, pthread_attr_t *, void * (*start)(void *), void * arg);
void store_id(pthread_t *id);
static void __thread_cleanup_function(void* arg);
static void *__pthread_new_thread(void *arg);
int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start)(void *), void * arg);
void profiling_thread_init();
static struct thread_info * get_thread_info(pid_t pid);
static void register_thread_pid(pid_t pid,struct numap_sampling_measure *sm,struct numap_sampling_measure *sm_wr) ;
void sampling_start(struct thread_info *measure_sampling);
void sampling_thread_init();
void sampling_init();
void finalize();


extern _Atomic int nthreads;
extern int rank;

struct __pthread_create_info_t {
  void *(*func)(void *);
  void *arg;
  int thread_rank;
};

struct thread_info {
  struct numap_sampling_measure *sm;
  struct numap_sampling_measure* sm_wr;
  pid_t tid;
  int rank;
  FILE *thread_fp; 
};

enum thread_status_t {
  thread_status_none,
  thread_status_created,
  thread_status_finalized
};

struct first_thread_info {
  pthread_t tid;
  enum thread_status_t status;
  FILE *thread_fp; 
};

