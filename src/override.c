#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <bits/pthreadtypes.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>

#define MAX_THREADS 1024
#define __NR_gettid 186

static int  (*real_create) (pthread_t *, pthread_attr_t *, void * (*start)(void *), void * arg) = NULL;
void store_id(pthread_t *id) {
    // 使用%lu格式说明符将pthread_t打印为十进制数
    printf("new thread created with id  %lu\n", (unsigned long)(*id));
}

#ifndef __pid_t_defined
typedef __pid_t pid_t;
# define __pid_t_defined
#endif

#undef pthread_create
//取消对 pthread_create 宏的定义，准备重新定义它。

/* Internal structure used for transmitting the function and argument
 * during pthread_create.
 */
struct __pthread_create_info_t {
  void *(*func)(void *);
  void *arg;
  int thread_rank;
};

enum thread_status_t {
  thread_status_none,
  thread_status_created,
  thread_status_finalized
};

struct thread_info {
  pthread_t tid;
  enum thread_status_t status;
};

struct thread_info thread_array[MAX_THREADS];
int nb_threads = 0;
int cpu_core = 0;

/* set to 1 if thread binding is activated */
int bind_threads = 1;
int _verbose = 1;

/* array describing the binding of each thread */
int thread_bindings[MAX_THREADS];
/* number of valid entries in the array */
int nb_thread_max = 1024;

static void __thread_cleanup_function(void* arg) {
  struct thread_info* me = arg;
  me->status = thread_status_finalized;
}

static void *__pthread_new_thread(void *arg) {
  void* res = NULL;
  struct __pthread_create_info_t *p_arg = (struct __pthread_create_info_t*) arg;
  void *(*f)(void *) = p_arg->func;
  void *__arg = p_arg->arg;
  int thread_rank = p_arg->thread_rank;
  free(p_arg);

  int oldtype;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

  pthread_cleanup_push(__thread_cleanup_function,
                       &thread_array[thread_rank]);

  res = (*f)(__arg);

  pthread_cleanup_pop(0);

  return res;
}

//当前策略用于均匀绑定线程到每个核心上
void set_affinity(pthread_t *thread, int thread_rank) {
    int total_cores = 16;  // 总的CPU核心数，包括0-15和32-47
    if ((thread_rank / total_cores)%2 == 1)
    {
      cpu_core =  (thread_rank % total_cores) + ((thread_rank / total_cores) * 32);
    }else{
      cpu_core = thread_rank % total_cores;
    }    

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core, &cpuset);

    int ret = pthread_setaffinity_np(*thread, sizeof(cpuset), &cpuset);
    if (ret != 0) {
        perror("pthread_setaffinity_np failed");
        abort();
    }

    if (_verbose) {
        pid_t tid = syscall(__NR_gettid);
        printf("I'm thread %d (tid=%lu) bound on cpu %d\n", thread_rank, (unsigned long)thread_array[thread_rank].tid, cpu_core);
    }
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

    if (bind_threads && thread_rank < nb_thread_max) {
        set_affinity(thread, thread_rank);
    }

    if (!retval) {
        store_id(thread);
    }

    return retval;
}


// #define _GNU_SOURCE
// #include <stdio.h>
// #include <stdint.h>
// #include <bits/pthreadtypes.h>
// #include <dlfcn.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sched.h>
// #include <pthread.h>

// #define MAX_THREADS 1024
// #define __NR_gettid 186

// static int  (*real_create) (pthread_t *, pthread_attr_t *, void * (*start)(void *), void * arg) = NULL;
// void store_id(pthread_t *id) {
//     // 使用%lu格式说明符将pthread_t打印为十进制数
//     printf("new thread created with id  %lu\n", (unsigned long)(*id));
// }

// #ifndef __pid_t_defined
// typedef __pid_t pid_t;
// # define __pid_t_defined
// #endif

// #undef pthread_create
// //取消对 pthread_create 宏的定义，准备重新定义它。

// /* Internal structure used for transmitting the function and argument
//  * during pthread_create.
//  */
// struct __pthread_create_info_t {
//   void *(*func)(void *);
//   void *arg;
//   int thread_rank;
// };

// enum thread_status_t {
//   thread_status_none,
//   thread_status_created,
//   thread_status_finalized
// };

// struct thread_info {
//   pthread_t tid;
//   enum thread_status_t status;
// };

// struct thread_info thread_array[MAX_THREADS];
// int nb_threads = 0;
// int cpu_core = 0;

// /* set to 1 if thread binding is activated */
// int bind_threads=1;
// int _verbose = 1;

// /* array describing the binding of each thread */
// int thread_bindings[MAX_THREADS];
// /* number of valid entries in the array */
// int nb_thread_max=1024;

// static void __thread_cleanup_function(void* arg) {
//   struct thread_info* me = arg;
//   me->status = thread_status_finalized;
// }

// static void *__pthread_new_thread(void *arg){
  
//   void* res = NULL;
//   struct __pthread_create_info_t *p_arg = (struct __pthread_create_info_t*) arg;
//   void *(*f)(void *) = p_arg->func;
//   void *__arg = p_arg->arg;
//   int thread_rank = p_arg->thread_rank;
//   free(p_arg);

//   int oldtype;
//   pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

//   pthread_cleanup_push(__thread_cleanup_function,
// 		       &thread_array[thread_rank]);

//   res = (*f)(__arg);

//   pthread_cleanup_pop(0);

//   return res;
//   #include <stdio.h>
// } 

// int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start)(void *), void * arg){
//     void *(*start_routine)(void *) = start;

//     int thread_rank = __sync_fetch_and_add( &nb_threads, 1 );
//     struct __pthread_create_info_t * __args =(struct __pthread_create_info_t*) malloc(sizeof(struct __pthread_create_info_t));
//       __args->func = start_routine;
//     __args->arg = arg;
//     __args->thread_rank= thread_rank;

//     //定义一个静态指针 real_create，指向原始的 pthread_create 函数。初始值为 NULL。
//     if (!real_create){
//         real_create = dlsym(RTLD_NEXT, "pthread_create");
//         //通过 dlsym 函数获取原始 pthread_create 函数的地址。
//     }

//     pthread_attr_t local_attr;
//     if(attr) {
//     memcpy(&local_attr, attr, sizeof(local_attr));
//     } 
//     else {
//     pthread_attr_init(&local_attr);
//     }
  
//     int retval = real_create(&thread_array[thread_rank].tid, &local_attr, __pthread_new_thread, __args);
//     memcpy(thread, &thread_array[thread_rank].tid, sizeof(pthread_t));

//     printf("\n%lu\n",(unsigned long) thread_array[thread_rank].tid);

//     // 将线程绑定到相应的 CPU 核心
//     if (bind_threads && thread_rank < nb_thread_max) {
//     int total_cores = 16;  // 总的CPU核心数，包括0-15和32-47

//     if (thread_rank >= total_cores){
//       cpu_core = (thread_rank % total_cores) + 32;
//     } else {
//       cpu_core = thread_rank;
//    }

//     cpu_set_t cpuset;
//     CPU_ZERO(&cpuset);
//     CPU_SET(cpu_core, &cpuset);

//     int ret = pthread_attr_setaffinity_np(&local_attr, sizeof(cpuset), &cpuset);
//     if (ret != 0) {
//         perror("pthread_attr_setaffinity_np failed");
//         abort();
//     }

//     thread_bindings[thread_rank] = cpu_core;
    
//     if(_verbose) {
//     pid_t tid = syscall(__NR_gettid);
//     printf("I'm thread %d (tid=%lu) bound on cpu %d\n", thread_rank, (unsigned long)thread_array[thread_rank].tid, thread_bindings[thread_rank]);
//   }

//     if(!retval) {
//         store_id(thread);
//     }

//     return retval;
//  }
// }


/*1209*/

// #define _GNU_SOURCE
// #include <stdio.h>
// #include <stdint.h>
// #include <bits/pthreadtypes.h>
// #include <dlfcn.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sched.h>
// #include <pthread.h>

// #define MAX_THREADS 1024
// #define __NR_gettid 186

// static int  (*real_create) (pthread_t *, pthread_attr_t *, void * (*start)(void *), void * arg) = NULL;
// void store_id(pthread_t *id) {
//     // 使用%lu格式说明符将pthread_t打印为十进制数
//     printf("new thread created with id  %lu\n", (unsigned long)(*id));
// }

// #ifndef __pid_t_defined
// typedef __pid_t pid_t;
// # define __pid_t_defined
// #endif

// #undef pthread_create
// //取消对 pthread_create 宏的定义，准备重新定义它。

// /* Internal structure used for transmitting the function and argument
//  * during pthread_create.
//  */
// struct __pthread_create_info_t {
//   void *(*func)(void *);
//   void *arg;
//   int thread_rank;
// };

// enum thread_status_t {
//   thread_status_none,
//   thread_status_created,
//   thread_status_finalized
// };

// struct thread_info {
//   pthread_t tid;
//   enum thread_status_t status;
// };

// struct thread_info thread_array[MAX_THREADS];
// int nb_threads = 0;
// int cpu_core = 0;

// /* set to 1 if thread binding is activated */
// int bind_threads=1;
// int _verbose = 1;

// /* array describing the binding of each thread */
// int thread_bindings[MAX_THREADS];
// /* number of valid entries in the array */
// int nb_thread_max=1024;

// static void __thread_cleanup_function(void* arg) {
//   struct thread_info* me = arg;
//   me->status = thread_status_finalized;
// }

// static void *__pthread_new_thread(void *arg){
  
//   void* res = NULL;
//   struct __pthread_create_info_t *p_arg = (struct __pthread_create_info_t*) arg;
//   void *(*f)(void *) = p_arg->func;
//   void *__arg = p_arg->arg;
//   int thread_rank = p_arg->thread_rank;
//   free(p_arg);

//   int oldtype;
//   pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

//   pthread_cleanup_push(__thread_cleanup_function,
// 		       &thread_array[thread_rank]);

//   // if(_verbose) {
//   //   pid_t tid = syscall(__NR_gettid);
//   //   printf("I'm thread %d (tid=%d) bound on cpu %d\n", thread_rank, tid, thread_bindings[thread_rank]);
//   // }

//   res = (*f)(__arg);

//   pthread_cleanup_pop(0);
//   // if(_verbose)
//   //   fprintf(stderr, "End of thread %lu\n", thread_array[thread_rank].tid);
//   // __thread_cleanup_function(&thread_array[thread_rank]);
  
//   return res;
//   #include <stdio.h>
// } 

// int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start)(void *), void * arg){
//     void *(*start_routine)(void *) = start;

//     int thread_rank = __sync_fetch_and_add( &nb_threads, 1 );
//     struct __pthread_create_info_t * __args =(struct __pthread_create_info_t*) malloc(sizeof(struct __pthread_create_info_t));
//       __args->func = start_routine;
//     __args->arg = arg;
//     __args->thread_rank= thread_rank;

//     //定义一个静态指针 real_create，指向原始的 pthread_create 函数。初始值为 NULL。
//     if (!real_create){
//         real_create = dlsym(RTLD_NEXT, "pthread_create");
//         //通过 dlsym 函数获取原始 pthread_create 函数的地址。
//     }

//     pthread_attr_t local_attr;
//     if(attr) {
//     memcpy(&local_attr, attr, sizeof(local_attr));
//     } 
//     else {
//     pthread_attr_init(&local_attr);
//     }
  
//     int retval = real_create(&thread_array[thread_rank].tid, &local_attr, __pthread_new_thread, __args);
//     memcpy(thread, &thread_array[thread_rank].tid, sizeof(pthread_t));

//     //输出新创建的线程号
//     // printf("\n%lu\n",(unsigned long) (*thread));
//     printf("\n%lu\n",(unsigned long) thread_array[thread_rank].tid);

//     // 将线程绑定到相应的 CPU 核心
//     if (bind_threads && thread_rank < nb_thread_max) {
//     int total_cores = 16;  // 总的CPU核心数，包括0-15和32-47
//     int compare = thread_rank / total_cores;
//     if (compare >= 1){
//       cpu_core = (thread_rank & total_cores) +32;
//     }else{
//       cpu_core = (thread_rank % total_cores);
//     }

//     cpu_set_t cpuset;
//     CPU_ZERO(&cpuset);
//     CPU_SET(cpu_core, &cpuset);

//     int ret = pthread_attr_setaffinity_np(&local_attr, sizeof(cpuset), &cpuset);
//     if (ret != 0) {
//         perror("pthread_attr_setaffinity_np failed");
//         abort();
//     }
    
//     if(_verbose) {
//     pid_t tid = syscall(__NR_gettid);
//     // printf("I'm thread %d (tid=%d) bound on cpu %d\n", thread_rank, tid, thread_bindings[thread_rank]);
//     printf("I'm thread %d (tid=%lu) bound on cpu %d\n", thread_rank, (unsigned long)thread_array[thread_rank].tid, thread_bindings[thread_rank]);
//   }

//     if(!retval) {
//         store_id(thread);
//     }

//     return retval;
//  }
// }


/*1209*/

// #define _GNU_SOURCE
// #include <stdio.h>
// #include <stdint.h>
// #include <bits/pthreadtypes.h>
// #include <dlfcn.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sched.h>
// #include <pthread.h>

// #define MAX_THREADS 1024
// #define __NR_gettid 186

// static int  (*real_create) (pthread_t *, pthread_attr_t *, void * (*start)(void *), void * arg) = NULL;
// void store_id(pthread_t  * id) {
//     fprintf(stderr, "new thread created with id  0x%lx\n", (*id));
// }

// #ifndef __pid_t_defined
// typedef __pid_t pid_t;
// # define __pid_t_defined
// #endif

// #undef pthread_create
// //取消对 pthread_create 宏的定义，准备重新定义它。

// /* Internal structure used for transmitting the function and argument
//  * during pthread_create.
//  */
// struct __pthread_create_info_t {
//   void *(*func)(void *);
//   void *arg;
//   int thread_rank;
// };

// // enum
// // {
// //   PTHREAD_CANCEL_DEFERRED,
// // #define PTHREAD_CANCEL_DEFERRED	PTHREAD_CANCEL_DEFERRED
// //   PTHREAD_CANCEL_ASYNCHRONOUS
// // #define PTHREAD_CANCEL_ASYNCHRONOUS	PTHREAD_CANCEL_ASYNCHRONOUS
// // };

// enum thread_status_t {
//   thread_status_none,
//   thread_status_created,
//   thread_status_finalized
// };

// struct thread_info {
//   pthread_t tid;
//   enum thread_status_t status;
// };

// struct thread_info thread_array[MAX_THREADS];
// int nb_threads = 0;
// int cpu_core = 0;

// /* set to 1 if thread binding is activated */
// int bind_threads=1;
// int _verbose = 0;

// /* array describing the binding of each thread */
// int thread_bindings[MAX_THREADS];
// /* number of valid entries in the array */
// int nb_thread_max=1024;

// static void __thread_cleanup_function(void* arg) {
//   struct thread_info* me = arg;
//   me->status = thread_status_finalized;
// }

// static void *__pthread_new_thread(void *arg){
  
//   void* res = NULL;
//   struct __pthread_create_info_t *p_arg = (struct __pthread_create_info_t*) arg;
//   void *(*f)(void *) = p_arg->func;
//   void *__arg = p_arg->arg;
//   int thread_rank = p_arg->thread_rank;
//   free(p_arg);

//   int oldtype;
//   pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

//   pthread_cleanup_push(__thread_cleanup_function,
// 		       &thread_array[thread_rank]);

//   if(_verbose) {
//     pid_t tid = syscall(__NR_gettid);
//     printf("I'm thread %d (tid=%d) bound on cpu %d\n", thread_rank, tid, thread_bindings[thread_rank]);
//   }

//   res = (*f)(__arg);

//   pthread_cleanup_pop(0);
//   if(_verbose)
//     fprintf(stderr, "End of thread %lu\n", thread_array[thread_rank].tid);
//   __thread_cleanup_function(&thread_array[thread_rank]);
  
//   return res;
//   #include <stdio.h>
// } 

// int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start)(void *), void * arg){
//     void *(*start_routine)(void *) = start;

//     int thread_rank = __sync_fetch_and_add( &nb_threads, 1 );
//     struct __pthread_create_info_t * __args =(struct __pthread_create_info_t*) malloc(sizeof(struct __pthread_create_info_t));
//       __args->func = start_routine;
//     __args->arg = arg;
//     __args->thread_rank= thread_rank;

//     // static int (*real_create)(pthread_t * , pthread_attr_t *, void * (*start)(void *), void *) = NULL;
//     //定义一个静态指针 real_create，指向原始的 pthread_create 函数。初始值为 NULL。
//     if (!real_create){
//         real_create = dlsym(RTLD_NEXT, "pthread_create");
//         //通过 dlsym 函数获取原始 pthread_create 函数的地址。
//     }

//     pthread_attr_t local_attr;
//     if(attr) {
//     memcpy(&local_attr, attr, sizeof(local_attr));
//     } 
//     else {
//     pthread_attr_init(&local_attr);
//     }
    
//     // if (bind_threads && thread_rank < nb_thread_max){
//     //     if(thread_bindings[thread_rank] >=0){
//     //         cpu_set_t cpuset;
//     //         CPU_ZERO(&cpuset);
//     //         CPU_SET(thread_bindings[thread_rank],&cpuset);
//     //         int ret = pthread_attr_setaffinity_np(&local_attr,sizeof(cpuset),&cpuset);
//     //         if(ret != 0){
// 	  //          perror("pthread_attr_setaffinity_np failed");
// 	  //          abort();
//     //         }
//     // }
     
//     int retval = real_create(&thread_array[thread_rank].tid, &local_attr, __pthread_new_thread, __args);
//     memcpy(thread, &thread_array[thread_rank].tid, sizeof(pthread_t));

//     // 将线程绑定到相应的 CPU 核心
//     if (bind_threads && thread_rank < nb_thread_max) {
//     int total_cores = 16;  // 总的CPU核心数，包括0-15和32-47
//     int compare = thread_rank / total_cores;
//     if (compare >= 1){
//       cpu_core = (thread_rank & total_cores) +32;
//     }else{
//       int cpu_core = (thread_rank % total_cores);
//     }

//     cpu_set_t cpuset;
//     CPU_ZERO(&cpuset);
//     CPU_SET(cpu_core, &cpuset);

//     int ret = pthread_attr_setaffinity_np(&local_attr, sizeof(cpuset), &cpuset);
//     if (ret != 0) {
//         perror("pthread_attr_setaffinity_np failed");
//         abort();
//     }
//     // rc = real_create(thread, attr, start, arg);
//     fprintf(stderr,"creates!!!");
//     if(!retval) {
//         store_id(thread);
//     }

//     return retval;
//  }
// }