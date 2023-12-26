
// #define _GNU_SOURCE
// #include <dlfcn.h>
// #include <stdio.h>
// #include <stdlib.h>

#define _GNU_SOURCE
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <handle.h>

#define MAX_THREADS 50

// __thread pthread_t thread_ids[MAX_THREADS];
// __thread int thread_count = 0;

// struct numap_sampling_measure gSm;

// // 函数指针，用于保存原始的 pthread_create 函数
// int (*libpthread_create)(pthread_t *thread, const pthread_attr_t *attr,
//                          void *(*start_routine)(void *), void *arg) = NULL;

// int pthread_create(pthread_t *__restrict thread,
//                    const pthread_attr_t *__restrict attr,
//                    void *(*start_routine)(void *),
//                    void *__restrict arg)
// {
//     fprintf(stderr, "\naaaaaaa\n");
//     fflush(stderr);

//     // 获取原始的 pthread_create 函数指针
//     if (!libpthread_create)
//     {
//         libpthread_create = dlsym(RTLD_NEXT, "pthread_create");
//     }

//     // 调用原始的 pthread_create 函数
//     int result = libpthread_create(thread, attr, start_routine, arg);

//     // 将新创建线程的 ID 存储到数组中
//     if (result == 0 && thread_count < MAX_THREADS)
//     {
//         thread_ids[thread_count++] = *thread;
//     }

//     return result;
// }

int main(int argc, char **argv)
{
    char ld_preload[4096] = "";
    snprintf(ld_preload, sizeof(ld_preload), "/data/cqy/1212mutithreads/lib/libnumap.so");

    setenv("LD_PRELOAD", ld_preload, 1);

    // 请注意：此处不再需要再次获取 libpthread_create
    // libpthread_create = dlsym(RTLD_NEXT, "pthread_create");

    char **new_argv = (char **)malloc((argc - 1) * sizeof(char *));
    for (int i = 1; i < argc; i++)
    {
        new_argv[i - 1] = argv[i];
    }
    new_argv[argc - 1] = NULL;
    printf("LD_PRELOAD: %s\n", getenv("LD_PRELOAD"));

    // 打印所有传入的参数
    printf("传入的参数：\n");
    for (int i = 0; i < argc - 1; i++)
    {
        printf("参数 %d: %s\n", i + 1, new_argv[i]);
        fflush(stdout);
    }

    extern char **environ;

    int ret = execve(argv[1], new_argv, environ);
    if (ret == -1)
    {
        perror("execve");
    }

    free(new_argv);

    return 0;
}
