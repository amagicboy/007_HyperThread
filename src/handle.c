// #include "handle.h"

// __thread long long Count[3] = {0};
// long long ReadSize = sizeof(long long);
// __thread long long count = 0;

// extern int __alarm_interval;
// extern int alarm_enabled;
// extern int enter;
// extern int count11;
// extern int enter_numap_generic_handler;

// __thread pthread_t thread_ids[MAX_THREADS];
// __thread int thread_count = 0;
// struct first_thread_info thread_array[MAX_THREADS];

// extern pthread_mutex_t sample_list_lock;
// int rank;
// _Atomic int nthreads;
// __thread unsigned thread_rank;
// int thread_bindings[MAX_THREADS];
// struct numap_sampling_measure gSm;
// struct numap_counting_measure gCm;
// __thread int fd;

// __thread struct numap_sampling_measure sm;
// __thread struct numap_sampling_measure sm_wr;

// static int  (*real_create) (pthread_t *, pthread_attr_t *, void * (*start)(void *), void * arg) = NULL;
// void store_id(pthread_t *id) {
//     // 使用%lu格式说明符将pthread_t打印为十进制数
//     printf("new thread created with id  %lu\n", (unsigned long)(*id));
// }

// int nb_threads = 0;
// int cpu_core = 0;
// /* set to 1 if thread binding is activated */
// int bind_threads = 1;
// int _verbose = 1;

// /* number of valid entries in the array */
// int nb_thread_max = 1024;

// static void __thread_cleanup_function(void* arg) {
//   struct first_thread_info* me = arg;
//   me->status = thread_status_finalized;
// }

// static void *__pthread_new_thread(void *arg) {
//   void* res = NULL;
//   struct __pthread_create_info_t *p_arg = (struct __pthread_create_info_t*) arg;
//   void *(*f)(void *) = p_arg->func;
//   void *__arg = p_arg->arg;
//   int thread_rank = p_arg->thread_rank;
//   free(p_arg);

//   profiling_thread_init();
//   //此处加入对线程开始初始化
//   thread_array[thread_rank].status = thread_status_created;

//   int oldtype;
//   pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
//   pthread_cleanup_push(__thread_cleanup_function,&thread_array[thread_rank]);
//   res = (*f)(__arg);
//   pthread_cleanup_pop(0);
//   __thread_cleanup_function(&thread_array[thread_rank]);

//   return res;
// }

// int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start)(void *), void * arg){
//     void *(*start_routine)(void *) = start;

//     int thread_rank = __sync_fetch_and_add( &nb_threads, 1 );
//     struct __pthread_create_info_t * __args = (struct __pthread_create_info_t*) malloc(sizeof(struct __pthread_create_info_t));
//     __args->func = start_routine;
//     __args->arg = arg;
//     __args->thread_rank = thread_rank;

//     if (!real_create){
//         real_create = dlsym(RTLD_NEXT, "pthread_create");
//     }

//     pthread_attr_t local_attr;
//     if (attr) {
//         memcpy(&local_attr, attr, sizeof(local_attr));
//     } else {
//         pthread_attr_init(&local_attr);
//     }
  
//     int retval = real_create(&thread_array[thread_rank].tid, &local_attr, __pthread_new_thread, __args);
//     memcpy(thread, &thread_array[thread_rank].tid, sizeof(pthread_t));

//     return retval;
// }

// unsigned next_thread_rank = 0;
// void profiling_thread_init(){
//     thread_rank = __sync_fetch_and_add( &next_thread_rank, 1 );
//     sampling_thread_init();
// }

// int allocated_threads = 0;
// struct thread_info *thread_ranks = NULL;
// _Atomic int nthreads = 0;

// static struct thread_info * get_thread_info(pid_t pid) {
//   for(int i=0; i<nthreads; i++)
//     if(thread_ranks[i].tid == pid)
//       return &thread_ranks[i];
//   return NULL;
// }

// static void register_thread_pid(pid_t pid,
// 				struct numap_sampling_measure *sm,
// 				struct numap_sampling_measure *sm_wr) {
//   if(allocated_threads == 0) {
//     thread_ranks = malloc(sizeof(struct thread_info)* 128);
//     allocated_threads = 128;
//   }
//   while(nthreads >= allocated_threads) {
//     allocated_threads *= 2;
//     thread_ranks = realloc(thread_ranks, sizeof(struct thread_info)*allocated_threads);
//   }
//   rank = nthreads++;
//   pthread_mutex_lock(&thread_lock);
//   thread_ranks[rank].tid = pid;
//   // printf("\nnow thread is %lu\n",thread_ranks[rank].tid);
//   thread_ranks[rank].rank = rank;
//   // set_affinity(pid,rank);
//   // printf("\nnow rank is %lu\n",thread_ranks[rank].rank);
//   thread_ranks[rank].sm = sm;
//   thread_ranks[rank].sm_wr = sm_wr;
//   //在此直接创建一个文件描述符
//   char filename[50];
//   snprintf(filename, sizeof(filename), "virtualAddr_%lu.txt", thread_ranks[rank].tid);

//   // if (thread_ranks[rank].thread_fp == NULL)
//   //   {
//   //       printf("Error opening file for thread %lu!\n", thread_ranks[rank].tid);
//   //       return 1;
//   //   }
//   thread_ranks[rank].thread_fp = fopen(filename, "w");
//   if (thread_ranks[rank].thread_fp == NULL) {
//     perror("fopen");
//     fprintf(stderr, "Error opening file for thread %lu: %s\n", thread_ranks[rank].tid, strerror(errno));
//     return;
//  }
//   printf("Thread %lu successfully registered!\n", thread_ranks[rank].tid);
//   pthread_mutex_unlock(&thread_lock);
//   // perror("perror");
// }
// //结构化数组用于存储pfrofiling线程的线程号以及其采样的指针

// void sampling_start(struct thread_info *measure_sampling){
//     // int res1 = numap_sampling_write_start(&sm_wr);
//     printf("\nenter sampling_start\n");
//     //check  if sm sm_wr are not NULL
//     if (measure_sampling->sm == NULL || measure_sampling->sm_wr == NULL) {
//     fprintf(stderr, "Error: Sampling measures are NULL for thread %lu\n", thread_ranks[rank].tid);
//     return;
//     }

//     printf("Initialized sm_wr at address: %p\n", measure_sampling->sm_wr);

//     int res1 = numap_sampling_write_start(measure_sampling->sm_wr);
//     if (res1 < 0)
//     {
//         fprintf(stderr, " -> numap_sampling_start error : %s\n", numap_error_message(res1));
//         if(res1 ==  ERROR_PERF_EVENT_OPEN && errno == EACCES) {
//       fprintf(stderr, "try running 'echo 1 > /proc/sys/kernel/perf_event_paranoid' to fix the problem\n");
//      }
//       abort();
//     }
//     printf("\nsampling_start111!!!\n");

//     // int res2 = numap_sampling_read_start(&sm);
//     // int res2 = numap_sampling_read_start(measure_sampling->sm);
//     // if (res2 < 0)
//     // {
//     //     fprintf(stderr, " -> numap_sampling_start error : %s\n", numap_error_message(res2));
//     //     abort();
//     // }
// }

// int sampling_rate =3000;
// size_t numap_page_count = 32;
// pthread_mutex_t thread_lock;


// void sampling_thread_init(){
//   pid_t tid = syscall(SYS_gettid);
//   printf("\ntid is :%d;rank is :%d\n",tid,rank);
//   register_thread_pid(tid, &sm, &sm_wr);

//   if(thread_ranks[rank].sm_wr ==NULL)
//   {
//     fprintf(stderr, "Failed to allocate memory for numap_sampling_measure\n");
//     exit(EXIT_FAILURE); 
//   }

//   //暂时不用对读采样  
//   // int res = numap_sampling_init_measure(&sm, 1, sampling_rate, numap_page_count);
//   // int res = numap_sampling_init_measure(thread_ranks[rank].sm, 1, sampling_rate, numap_page_count);
//   // if(res < 0) {
//   //   fprintf(stderr, "numap_sampling_init error : %s\n", numap_error_message(res));
//   //   abort();
//   // }

//   // res = numap_sampling_init_measure(&sm_wr, 1, sampling_rate, numap_page_count);
//   int res = numap_sampling_init_measure(thread_ranks[rank].sm_wr, 1, sampling_rate, numap_page_count);
//   if(res < 0) {
//     fprintf(stderr, "numap_sampling_init error : %s\n", numap_error_message(res));
//     abort();
//   }
  
//   //打印初始化后的地址
//   printf("Initialized sm_wr at address: %p\n", (void*)thread_ranks[rank].sm_wr);

//   /* for now, only collect info on the current thread */
//   // sm.tids[0] = tid;
//   // sm_wr.tids[0] = tid;
//   thread_ranks[rank].sm->tids[0] = tid;
//   thread_ranks[rank].sm_wr->tids[0] = tid;

//   //设置中断函数和中断信号
//   struct sigaction s;  
//   s.sa_handler = sig_handler;  
//   int signo=SIGALRM;
//   int ret = sigaction(signo, &s, NULL);
//   if(ret<0) {  
//       perror("sigaction failed");  
//       abort();  
//   }

//   int page_size=4096;
//     /* number of samples that fit in one sample buffer */
//   int nsamples = numap_page_count * page_size / (sizeof(struct mem_sample)+sizeof(struct perf_event_header));
//   // if(numap_sampling_set_measure_handler(&sm, numap_read_handler, nsamples) != 0)
  
//   //暂时不对读进行采样
//   // if(numap_sampling_set_measure_handler(thread_ranks[rank].sm, numap_read_handler, nsamples) != 0)
//   //     printf("numap_sampling_set_measure_handler failed\n");
//   // if(numap_sampling_set_measure_handler(&sm_wr, numap_write_handler, nsamples) != 0)
//   if(numap_sampling_set_measure_handler(thread_ranks[rank].sm_wr, numap_write_handler, nsamples) != 0)
//       printf("numap_sampling_set_measure_handler failed\n");
  
//   __set_alarm();
//   sampling_start(&thread_ranks[rank]);
//   printf("\n sampling start!!\n");

// }

// extern struct mem_counters global_counters[2];

// void sampling_init(){
//   pthread_mutex_init(&thread_lock, NULL);
//   int err = numap_init();
//    if(err != 0) {
//     fprintf(stderr, "Error while initializing numap: %s\n", numap_error_message(err));
//     abort();
//   }
//   // pthread_mutex_init(&thread_lock, NULL);
//   pthread_mutex_init(&sample_list_lock, NULL);
//   init_mem_counter(&global_counters[0]);
//   init_mem_counter(&global_counters[1]);

// }

// //当前传递到暂停采样并回收的sm和sm_wr并不是thread_ranks里面的
// void finalize(){
//   printf("\nwihch threas:%d\n",thread_ranks[rank].tid);
//   for(int a=0;a<rank;a++)
//   {
//     if(thread_ranks[rank].sm == &sm)
//     {
//     printf("\n true!\n");
//     }else
//     {
//       if (thread_ranks[rank].sm == NULL || thread_ranks[rank].sm_wr == NULL) 
//       {
//     fprintf(stderr, "Error: Sampling measures are NULL for thread %lu\n", thread_ranks[rank].tid);
//     return;
//       }
  
//     //暂时不用采样读
//     // int res = numap_sampling_read_stop(thread_ranks[rank].sm);
//     // if(res < 0) {
//     //   printf("numap_sampling_stop error : %s\n", numap_error_message(res));
//     //   abort();
//     // }

//     if (numap_sampling_write_supported()) {
//      int res = numap_sampling_write_stop(thread_ranks[a].sm_wr);
//      if(res < 0) 
//      {
//       printf("numap_sampling_stop error : %s\n", numap_error_message(res));
//       abort();
//      }
//     }
  
//     // int rank=nthreads;
//     // printf("\nnthread is %d\n",nthreads);

//     printf("Initialized sm_wr at address: %p\n", (void*)thread_ranks[a].sm_wr);
//     //暂时不用采样读
//     // _process_samples(thread_ranks[rank].thread_fp, thread_ranks[rank].sm, ACCESS_READ);
//     if (numap_sampling_write_supported()) 
//     {
//       _process_samples(thread_ranks[a].thread_fp,thread_ranks[a].sm_wr, ACCESS_WRITE);
//     }

//     // numap_sampling_end(thread_ranks[rank].sm);
//     numap_sampling_end(thread_ranks[a].sm_wr);
//     printf("\nFinalize!!!\n");
//     }
//   }

// //   if (thread_ranks[rank].sm == NULL || thread_ranks[rank].sm_wr == NULL) {
// //     fprintf(stderr, "Error: Sampling measures are NULL for thread %lu\n", thread_ranks[rank].tid);
// //     return;
// // }

// //   int res = numap_sampling_read_stop(&sm);
// //   if(res < 0) {
// //     printf("numap_sampling_stop error : %s\n", numap_error_message(res));
// //     abort();
// //   }

// //   if (numap_sampling_write_supported()) {
// //     res = numap_sampling_write_stop(&sm_wr);
// //     if(res < 0) {
// //       printf("numap_sampling_stop error : %s\n", numap_error_message(res));
// //       abort();
// //     }
// //   }
  
// //   int rank=nthreads;
// //   printf("\nnthread is %d\n",nthreads);

// //   _process_samples(thread_ranks[rank].thread_fp, &sm, ACCESS_READ);
// //   if (numap_sampling_write_supported()) {
// //     _process_samples(thread_ranks[rank].thread_fp, &sm_wr, ACCESS_WRITE);
// //   }

// //   numap_sampling_end(&sm);
// //   numap_sampling_end(&sm_wr);

// }

// static void __memory_init(void) __attribute__ ((constructor));
// static void __memory_init(void) {
//     real_create= dlsym(RTLD_NEXT, "pthread_create");
//     sampling_init();
//     profiling_thread_init();
//     printf("\n\nconstructing finished!!");
// }

// static void __memory_conclude(void) __attribute__ ((destructor));
// static void __memory_conclude(void) {
 
//   finalize();

// }




//1220
#include "handle.h"
#include <stdio.h>

__thread long long Count[3] = {0};
long long ReadSize = sizeof(long long);
__thread long long count = 0;

extern int __alarm_interval;
extern int alarm_enabled;
extern int enter;
extern int count11;
extern int enter_numap_generic_handler;

__thread pthread_t thread_ids[MAX_THREADS];
__thread int thread_count = 0;
struct first_thread_info thread_array[MAX_THREADS];

extern pthread_mutex_t sample_list_lock;
int rank;
_Atomic int nthreads;
__thread unsigned thread_rank;
int thread_bindings[MAX_THREADS];
struct numap_sampling_measure gSm;
struct numap_counting_measure gCm;
__thread int fd;

__thread struct numap_sampling_measure sm;
__thread struct numap_sampling_measure sm_wr;

static int  (*real_create) (pthread_t *, pthread_attr_t *, void * (*start)(void *), void * arg) = NULL;

pthread_mutex_t thread_lock;

void store_id(pthread_t *id) {
    // 使用%lu格式说明符将pthread_t打印为十进制数
    printf("new thread created with id  %lu\n", (unsigned long)(*id));
}

int nb_threads = 0;
int cpu_core = 0;
/* set to 1 if thread binding is activated */
int bind_threads = 1;
int _verbose = 1;

/* number of valid entries in the array */
int nb_thread_max = 1024;

static void __thread_cleanup_function(void* arg) {
  struct first_thread_info* me = arg;
  me->status = thread_status_finalized;
}

static void *__pthread_new_thread(void *arg) {
  void* res = NULL;
  struct __pthread_create_info_t *p_arg = (struct __pthread_create_info_t*) arg;
  void *(*f)(void *) = p_arg->func;
  void *__arg = p_arg->arg;
  int thread_rank = p_arg->thread_rank;
  free(p_arg);

  profiling_thread_init();
  //此处加入对线程开始初始化
  thread_array[thread_rank].status = thread_status_created;

  int oldtype;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
  pthread_cleanup_push(__thread_cleanup_function,&thread_array[thread_rank]);
  res = (*f)(__arg);
  pthread_cleanup_pop(0);
  __thread_cleanup_function(&thread_array[thread_rank]);

  return res;
}

int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start)(void *), void * arg){
    void *(*start_routine)(void *) = start;

    int thread_rank = __sync_fetch_and_add( &nb_threads, 1 );
    struct __pthread_create_info_t * __args = (struct __pthread_create_info_t*) malloc(sizeof(struct __pthread_create_info_t));
    __args->func = start_routine;
    __args->arg = arg;
    __args->thread_rank = thread_rank;

    if (!real_create){
        real_create = dlsym(RTLD_NEXT, "pthread_create");
    }

    pthread_attr_t local_attr;
    if (attr) {
        memcpy(&local_attr, attr, sizeof(local_attr));
    } else {
        pthread_attr_init(&local_attr);
    }
  
    int retval = real_create(&thread_array[thread_rank].tid, &local_attr, __pthread_new_thread, __args);
    memcpy(thread, &thread_array[thread_rank].tid, sizeof(pthread_t));

    return retval;
}

unsigned next_thread_rank = 0;
void profiling_thread_init(){
    thread_rank = __sync_fetch_and_add( &next_thread_rank, 1 );
    sampling_thread_init();
}

int allocated_threads = 0;
struct thread_info *thread_ranks = NULL;
_Atomic int nthreads = 0;

static struct thread_info * get_thread_info(pid_t pid) {
  for(int i=0; i<nthreads; i++)
    if(thread_ranks[i].tid == pid)
      return &thread_ranks[i];
  return NULL;
}

static void register_thread_pid(pid_t pid, struct numap_sampling_measure *sm, struct numap_sampling_measure *sm_wr) {
    pthread_mutex_lock(&thread_lock);  // 加锁

    // 分配内存
    if (allocated_threads == 0) {
        allocated_threads = 128;
        thread_ranks = (struct thread_info *)malloc(sizeof(struct thread_info) * allocated_threads);
        if (thread_ranks == NULL) {
            fprintf(stderr, "Failed to allocate memory for thread_ranks\n");
            pthread_mutex_unlock(&thread_lock); // 锁定失败时也需要解锁
            return;
        }
    }

    // 动态扩展数组
    while (nthreads >= allocated_threads) {
        allocated_threads *= 2;
        struct thread_info *temp = (struct thread_info *)realloc(thread_ranks, sizeof(struct thread_info) * allocated_threads);
        if (temp == NULL) {
            fprintf(stderr, "Failed to reallocate memory for thread_ranks\n");
            pthread_mutex_unlock(&thread_lock); // 重新分配失败时也需要解锁
            return;
        }
        thread_ranks = temp;
    }

    // 初始化新线程信息
    rank = nthreads++;
    thread_ranks[rank].tid = pid;
    thread_ranks[rank].rank = rank;
    thread_ranks[rank].sm = sm;
    thread_ranks[rank].sm_wr = sm_wr;

    // 创建文件描述符
    char filename[50];
    snprintf(filename, sizeof(filename), "virtualAddr_%lu.txt", thread_ranks[rank].tid);
    thread_ranks[rank].thread_fp = fopen(filename, "w");
    if (thread_ranks[rank].thread_fp == NULL) {
        perror("fopen");
        fprintf(stderr, "Error opening file for thread %lu: %s\n", thread_ranks[rank].tid, strerror(errno));
        pthread_mutex_unlock(&thread_lock); // 错误发生时也需要解锁
        return;
    }

    printf("Thread %lu successfully registered!\n", thread_ranks[rank].tid);

    pthread_mutex_unlock(&thread_lock);  // 解锁
}


// static void register_thread_pid(pid_t pid,
// 				struct numap_sampling_measure *sm,
// 				struct numap_sampling_measure *sm_wr) {
//   pthread_mutex_lock(&thread_lock);  // 加锁
//   if(allocated_threads == 0) {
//     thread_ranks = malloc(sizeof(struct thread_info)* 128);
//     allocated_threads = 128;
//   }
//   while(nthreads >= allocated_threads) {
//     allocated_threads *= 2;
//     thread_ranks = realloc(thread_ranks, sizeof(struct thread_info)*allocated_threads);
//   }


//   rank = nthreads++;
//   thread_ranks[rank].tid = pid;
//   thread_ranks[rank].rank = rank;
//   thread_ranks[rank].sm = sm;
//   thread_ranks[rank].sm_wr = sm_wr;
//   //在此直接创建一个文件描述符
//   char filename[50];
//   snprintf(filename, sizeof(filename), "virtualAddr_%lu.txt", thread_ranks[rank].tid);
//   thread_ranks[rank].thread_fp = fopen(filename, "w");
//   if (thread_ranks[rank].thread_fp == NULL) {
//     perror("fopen");
//     fprintf(stderr, "Error opening file for thread %lu: %s\n", thread_ranks[rank].tid, strerror(errno));
//     return;
//   }
//   printf("Thread %lu successfully registered!\n", thread_ranks[rank].tid);

//   pthread_mutex_unlock(&thread_lock);  // 解锁
// }

// 结构化数组用于存储pfrofiling线程的线程号以及其采样的指针
void sampling_start(struct thread_info *measure_sampling){
    printf("\nenter sampling_start\n");
    pthread_mutex_lock(&thread_lock);  // 加锁
    // check  if sm sm_wr are not NULL
    if (measure_sampling->sm == NULL || measure_sampling->sm_wr == NULL) {
    fprintf(stderr, "Error: Sampling measures are NULL for thread %lu\n", thread_ranks[rank].tid);
    return;
    }

    printf("Initialized sm_wr at address: %p\n", measure_sampling->sm_wr);
    
    //判断是否该measure_sampling已经在采样？
    if(measure_sampling->sm_wr->started == 0)
    {
      int res1 = numap_sampling_write_start(measure_sampling->sm_wr);
      printf("\nres1=%d\n",res1);
      if (res1 < 0)
     {
        fprintf(stderr, " -> numap_sampling_start error : %s\n", numap_error_message(res1));
        if(res1 ==  ERROR_PERF_EVENT_OPEN && errno == EACCES)
         {
        fprintf(stderr, "try running 'echo 1 > /proc/sys/kernel/perf_event_paranoid' to fix the problem\n");
       }
       pthread_mutex_unlock(&thread_lock);
       abort();
     }
    }
    pthread_mutex_unlock(&thread_lock);  // 解锁
}

// void sampling_start(struct thread_info *measure_sampling) {
//     printf("\nenter sampling_start\n");

//     // Check if sm and sm_wr are not NULL
//     if (measure_sampling->sm == NULL || measure_sampling->sm_wr == NULL) {
//         fprintf(stderr, "Error: Sampling measures are NULL for thread %lu\n", thread_ranks[rank].tid);
//         return;
//     }

//     printf("Initialized sm_wr at address: %p\n", measure_sampling->sm_wr);

//     int res1 = numap_sampling_write_start(measure_sampling->sm_wr);
//     if (res1 < 0) {
//         fprintf(stderr, " -> numap_sampling_start error : %s\n", numap_error_message(res1));

//         if (res1 == ERROR_PERF_EVENT_OPEN && errno == EACCES) {
//             fprintf(stderr, "Try running 'echo 1 > /proc/sys/kernel/perf_event_paranoid' to fix the problem\n");
//         }

//         // Instead of calling abort(), return from the function to handle the error gracefully
//         return;
//     }

//     printf("\nsampling_start111!!!\n");
// }


int sampling_rate =3000;
size_t numap_page_count = 32;

void sampling_thread_init(){
  pid_t tid = syscall(SYS_gettid);
  printf("\ntid is :%d;rank is :%d\n",tid,rank);
  register_thread_pid(tid, &sm, &sm_wr);

  if(thread_ranks[rank].sm_wr ==NULL)
  {
    fprintf(stderr, "Failed to allocate memory for numap_sampling_measure\n");
    exit(EXIT_FAILURE); 
  }

  int res = numap_sampling_init_measure(thread_ranks[rank].sm_wr, 1, sampling_rate, numap_page_count);
  if(res < 0) {
    fprintf(stderr, "numap_sampling_init error : %s\n", numap_error_message(res));
    abort();
  }

  thread_ranks[rank].sm->tids[0] = tid;
  thread_ranks[rank].sm_wr->tids[0] = tid;

  // 设置中断函数和中断信号
  struct sigaction s;  
  s.sa_handler = sig_handler;  
  int signo=SIGALRM;
  int ret = sigaction(signo, &s, NULL);
  if(ret<0) {  
      perror("sigaction failed"); 
      abort();  
  }

  int page_size=4096;
  int nsamples = numap_page_count * page_size / (sizeof(struct mem_sample)+sizeof(struct perf_event_header));
  if(numap_sampling_set_measure_handler(thread_ranks[rank].sm_wr, numap_write_handler, nsamples) != 0)
  {
    printf("numap_sampling_set_measure_handler failed\n");
  }
  
  __set_alarm();
  sampling_start(&thread_ranks[rank]);
  printf("\n sampling start!!\n");
}

extern struct mem_counters global_counters[2];

void sampling_init(){
  pthread_mutex_init(&thread_lock, NULL);  // 初始化互斥锁
  int err = numap_init();
   if(err != 0) {
    fprintf(stderr, "Error while initializing numap: %s\n", numap_error_message(err));
    abort();
  }
  pthread_mutex_init(&sample_list_lock, NULL);
  init_mem_counter(&global_counters[0]);
  init_mem_counter(&global_counters[1]);
}

void finalize()
{
  printf("\nwihch threas:%d\n",thread_ranks[rank].tid);
  for(int a=0;a<rank;a++)
  {
    // if(thread_ranks[rank].sm == &sm)
    // {
    // printf("\n true!\n");
    // }else
    // {
    //   if (thread_ranks[rank].sm == NULL || thread_ranks[rank].sm_wr == NULL) 
    //   {
    //    fprintf(stderr, "Error: Sampling measures are NULL for thread %lu\n", thread_ranks[rank].tid);
    //    return;
    //   }
    
    if(thread_ranks[a].sm_wr->started != 0)
    {

      printf("\nthread id is :%d\n",a);
      if (numap_sampling_write_supported())
      {
       int res = numap_sampling_write_stop(thread_ranks[a].sm_wr);
       if(res < 0) 
         {
         printf("numap_sampling_stop error : %s\n", numap_error_message(res));
         abort();
        }
      }

     printf("Initialized sm_wr at address: %p\n", (void*)thread_ranks[a].sm_wr);

     if (numap_sampling_write_supported()) 
     {
        _process_samples(thread_ranks[a].thread_fp,thread_ranks[a].sm_wr, ACCESS_WRITE);
     }
     numap_sampling_write_print(thread_ranks[a].thread_fp,thread_ranks[a].sm_wr,1);

     numap_sampling_end(thread_ranks[a].sm_wr);
     printf("\nFinalize!!!\n");
    }
  }
}

static void __memory_init(void) __attribute__ ((constructor));
static void __memory_init(void) {
    real_create= dlsym(RTLD_NEXT, "pthread_create");
    sampling_init();
    profiling_thread_init();
    printf("\n\nconstructing finished!!");
}

static void __memory_conclude(void) __attribute__ ((destructor));
static void __memory_conclude(void) {
  finalize();
  
  printf("\nnbthreads is :%d\n",nb_threads);
  for (int i = 0; i < nthreads; ++i) {
    if (thread_ranks[i].thread_fp != NULL) {
      fclose(thread_ranks[i].thread_fp);
    }
  }

  if (thread_ranks != NULL) {
    free(thread_ranks); // 释放分配的内存
  }
}


