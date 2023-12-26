#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include <err.h>
#include <time.h>
#include <numa.h>
#include <linux/version.h>
#include <pthread.h>
#include <signal.h>
#include "handle.h"


// #include "numap.h"
#include "numap_analyse.h"

#define PERF_EVENT_MLOCK_KB_FILE "/proc/sys/kernel/perf_event_mlock_kb"

#define NOT_SUPPORTED "NOT_SUPPORTED"

// extern struct numap_sampling_measure gSm;
FILE *fpVirtualAddr = NULL;

// extern __thread struct numap_sampling_measure sm;
// extern __thread struct numap_sampling_measure sm_wr;

// extern struct thread_info *thread_ranks;
// extern _Atomic int nthreads;
// extern int rank;


struct archi
{
    unsigned int id;
    char name[256];
    char sampling_read_event[256];
    char sampling_write_event[256];
    char counting_read_event[256];
    char counting_write_event[256];
};

struct link_fd_measure
{
    struct link_fd_measure *next;
    int fd;
    struct numap_sampling_measure *measure;
};


/* If your Intel CPU is not supported by Numap, you need to add a new architecture:
 * .id can be found by running lscpu (CPU Familly | Model <<8)
 * .sampling_read_event and .sampling_write_event can be found in the Intel Software Developpers Manuel (Chapter 19)
 */

#define CPU_MODEL(family, model) ((family) | (model) << 8)

/*  in ns */
long __alarm_interval = 3 * 1000000;
int alarm_enabled = 1;
__thread int alarm_set = 0;
//线程局部变量

// void __set_alarm() {
//   if(__alarm_interval>=0 && alarm_enabled && (! alarm_set)) {
//     fprintf(stderr,"\nset!!!alarm!!!\n");
//     fflush(stderr);
//     alarm_set = 1;
//     struct sigevent sevp;
//     sevp.sigev_notify=SIGEV_THREAD_ID | SIGEV_SIGNAL;
//     //表示定时器到期时会向指定线程发送信号；
//     sevp.sigev_signo=SIGALRM;
//     sevp.sigev_value.sival_int=0;
//     sevp.sigev_notify_function = NULL;
//     sevp.sigev_notify_attributes=NULL;
//     //定时器到期时不调用任何函数和不设置任何属性
//     sevp._sigev_un._tid = syscall(SYS_gettid);
//     //设置为当前线程的线程 ID
    
//     timer_t *t = malloc(sizeof(timer_t));
//     int ret = timer_create(CLOCK_REALTIME, &sevp, t);    
//     //创建一个新的定时器，并将定时器的 ID 存储在 t 变量中。  
//     if(ret != 0){
//       perror("timer create failed");
//       abort();
//     }
//     struct itimerspec new_value, old_value;
//     new_value.it_interval.tv_sec=0;
//     new_value.it_interval.tv_nsec=__alarm_interval;
//     //定时器时间间隔设置为0，且首次触发时间为__alarm_interval
    
//     new_value.it_value.tv_sec=0;
//     new_value.it_value.tv_nsec=__alarm_interval;
    
//     ret = timer_settime(*t,0, &new_value, &old_value);
//     //设置定时器的时间间隔和首次触发事件，并将旧定时器的参数保存在old_value
      
//     if(ret != 0){
//       perror("timer settime failed");
//       abort();
//       //定时器设置失败则中止程序
//     }
//   }
// }


// void refresh_wrapper_handler(int signum, siginfo_t *info, void *ucontext)
// {
//     // static int enter=0;
//     enter++;
//     // printf("\nenter numbers:%d\n",enter);
//     // static int count11 = 0;
//     if (info->si_code == POLL_IN){
//         count11++;
//         struct timespec current_time;
//         clock_gettime(CLOCK_MONOTONIC, &current_time);
//     // fprintf(stderr,"\nhere,refresh!!!!\n");
//     // printf("\ncount numbers:%d\n",count);
//     // fflush(stdout);
//     // 获取当前时间
//     // 计算时间间隔并打印
//     if (count11 > 1) {
//             long long int nanoseconds = (current_time.tv_sec - last_time.tv_sec) * 1000000000LL +
//                                        (current_time.tv_nsec - last_time.tv_nsec);

//             // printf("POLL_IN 时间间隔：%lld 纳秒\n", nanoseconds);
//             // fflush(stdout);
//         }

//         // 更新上次的时间
//         last_time= current_time;

//     }
//     // fprintf(stderr,"\nhere,refresh!!!!\n");
//     // fflush(stdout);
//     // static int count=0;
//     // if (count ==1000){
//     //     _process_samples(&gSm, ACCESS_WRITE);
//     // }
//     // count++;
//     else if (info->si_code == POLL_HUP)
//     {
//         /* TODO: copy the samples */    
//  printf("\nenterenterenter\n");
//  fflush(stdout);

//         int fd = info->si_fd;

//         // printf("\n11111111enterenterenter\n");
//         // fflush(stdout);
//         // search for corresponding measure
//         struct link_fd_measure *current_lfm = link_fd_measure;
//         // printf("\n22222222enterenterenter\n");
//         // fflush(stdout);
//         while (current_lfm != NULL && current_lfm->fd != fd)
//         {
//             current_lfm = current_lfm->next;
//         // printf("\n333333333333enterenterenter\n");
//         // fflush(stdout);
//         }
//         if (current_lfm == NULL)
//         {
//             fprintf(stderr, "No measure associated with fd %d\n", fd);
//             abort();
//             exit(EXIT_FAILURE);
//         }
//         // printf("\n44444444444444enterenterenter\n");
//         // fflush(stdout);
//         struct numap_sampling_measure *measure = current_lfm->measure;

//         if (gSm.handler)
//         {
//         //   printf("\n555555555555555enterenterenter\n");
//         //   fflush(stdout);
//             gSm.handler(&gSm, fd);
//         }
//         else{
//             // printf("\n\n");
//         }
//         // printf("\n77777777777777enterenterenter\n");
//         // fflush(stdout);
//         gSm.total_samples += gSm.nb_refresh;
//         printf("\ntotal_samples=%d\n",gSm.total_samples);
//         fflush(stdout);

//         ioctl(info->si_fd, PERF_EVENT_IOC_REFRESH, gSm.nb_refresh);
//     }
// }

struct link_fd_measure *link_fd_measure = NULL;

void refresh_wrapper_handler(int signum, siginfo_t *info, void *ucontext)
{
   if (info->si_code == POLL_HUP)
    {
        /* TODO: copy the samples */    
        // printf("\nenterenterenter\n");
        // fflush(stdout);

        int fd = info->si_fd;
        struct link_fd_measure *current_lfm = link_fd_measure;
        while (current_lfm != NULL && current_lfm->fd != fd)
        {
            current_lfm = current_lfm->next;
        }
        if (current_lfm == NULL)
        {
            fprintf(stderr, "No measure associated with fd %d\n", fd);
            abort();
            exit(EXIT_FAILURE);
        }

        struct numap_sampling_measure *measure = current_lfm->measure;

        if (sm_wr.handler)
        {
           sm_wr.handler(&sm_wr, fd);
        }
        else{
            // printf("\n\n");
        }
        sm_wr.total_samples += sm_wr.nb_refresh;
        printf("\nnb_refresh is %d\n",sm_wr.nb_refresh);
        printf("\ntotal_samples=%d\n",sm_wr.total_samples);
        fflush(stdout);

        ioctl(info->si_fd, PERF_EVENT_IOC_REFRESH, sm_wr.nb_refresh);
    }
}

void __set_alarm() {
    if (__alarm_interval >= 0 && alarm_enabled && (!alarm_set)) {
        fprintf(stderr, "\nset!!!alarm!!!\n");
        fflush(stderr);
        alarm_set = 1;
        struct sigaction sa;
        sa.sa_handler = refresh_wrapper_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if (sigaction(SIGALRM, &sa, NULL) == -1) {
            perror("sigaction");
            abort();
        }

        struct itimerspec new_value, old_value;
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_nsec = 1800000;
        new_value.it_value = new_value.it_interval;

        timer_t *t = malloc(sizeof(timer_t));
        int ret = timer_create(CLOCK_REALTIME, NULL, t);
        if (ret != 0) {
            perror("timer create failed");
            abort();
        }

        ret = timer_settime(*t, 0, &new_value, &old_value);
        if (ret != 0) {
            perror("timer settime failed");
            abort();
        }
    }
}


extern void _process_samples(FILE *thread_fp,
                             struct numap_sampling_measure *sm,
                             enum access_type access_type);

static void get_archi(unsigned int archi_id, struct archi *arch)
{
    arch->id = archi_id;

    snprintf(arch->name, 256, "Unknown architecture");
    snprintf(arch->sampling_read_event, 256, NOT_SUPPORTED);
    snprintf(arch->sampling_write_event, 256, NOT_SUPPORTED);
    snprintf(arch->counting_read_event, 256, NOT_SUPPORTED);
    snprintf(arch->counting_write_event, 256, NOT_SUPPORTED);

    switch (archi_id)
    {
    case CPU_MODEL(6, 151):
    case CPU_MODEL(6, 154):
        snprintf(arch->name, 256, "Alder Lake micro arch");
        /* Not tested. Let's assume these events are the same as the previous cpu generation */
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_INST_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 167):
        snprintf(arch->name, 256, "Rocket Lake micro arch");
        /* Not tested. Let's assume these events are the same as the previous cpu generation */
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_INST_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 141):
    case CPU_MODEL(6, 140):
        snprintf(arch->name, 256, "Tiger Lake micro arch");
        /* Not tested. Let's assume these events are the same as the previous cpu generation */
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_INST_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 125):
    case CPU_MODEL(6, 126):
        snprintf(arch->name, 256, "Ice Lake micro arch");
        /* Not tested. Let's assume these events are the same as the previous cpu generation */
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_INST_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 102):
        snprintf(arch->name, 256, "Cannon Lake micro arch");
        /* Not tested. Let's assume these events are the same as the previous cpu generation */
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_INST_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 158):
    case CPU_MODEL(6, 142):
        snprintf(arch->name, 256, "Kaby Lake micro arch");
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_INST_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 94):
    case CPU_MODEL(6, 78):
    case CPU_MODEL(6, 85):
        snprintf(arch->name, 256, "Skylake micro arch");
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_UOPS_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 79):
    case CPU_MODEL(6, 86):
    case CPU_MODEL(6, 71):
    case CPU_MODEL(6, 61):
        snprintf(arch->name, 256, "Broadwell micro arch");
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_UOPS_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 60):
    case CPU_MODEL(6, 63):
    case CPU_MODEL(6, 69):
    case CPU_MODEL(6, 70):
        snprintf(arch->name, 256, "Haswell micro arch");
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_UOPS_RETIRED:ALL_STORES");
        break;

    case CPU_MODEL(6, 58):
    case CPU_MODEL(6, 62):
        snprintf(arch->name, 256, "Ivy Bridge micro arch");
        // NOTE: in the Intel SDM, read sampling event is MEM_TRANS_RETIRED:LOAD_LATENCY.
        // In practice this event does not work. As a consequence we use the event below
        // which is the one used by perf mem record and reported by the pfm library
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LATENCY_ABOVE_THRESHOLD:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_TRANS_RETIRED:PRECISE_STORE");
        // snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LOAD_LATENCY:ldlat=3");
        // snprintf(arch->sampling_write_event, 256, "MEM_TRANS_RETIRED:PRECISE_STORE");
        break;

    case CPU_MODEL(6, 42):
    case CPU_MODEL(6, 45):
        snprintf(arch->name, 256, "Sandy Bridge micro arch");
        // NOTE: in the Intel SDM, read sampling event is MEM_TRANS_RETIRED:LOAD_LATENCY.
        // In practice this event does not work. As a consequence we use the event below
        // which is the one used by perf mem record and reported by the pfm library
        snprintf(arch->sampling_read_event, 256, "MEM_TRANS_RETIRED:LATENCY_ABOVE_THRESHOLD:ldlat=3");
        snprintf(arch->sampling_write_event, 256, "MEM_TRANS_RETIRED:PRECISE_STORE");
        break;

    case CPU_MODEL(6, 46):
    case CPU_MODEL(6, 30):
    case CPU_MODEL(6, 26):
    case CPU_MODEL(6, 31):
        snprintf(arch->name, 256, "Nehalem micro arch");
        snprintf(arch->sampling_read_event, 256, "MEM_INST_RETIRED:LATENCY_ABOVE_THRESHOLD:ldlat=3");
        break;

    case CPU_MODEL(6, 44):
    case CPU_MODEL(6, 47):
    case CPU_MODEL(6, 37):
        snprintf(arch->name, 256, "Westmere micro arch");
        snprintf(arch->sampling_read_event, 256, "MEM_INST_RETIRED:LATENCY_ABOVE_THRESHOLD:ldlat=3");
        break;

        // Intel Xeon Phi CPUs (currently not supported)
        // I'm not sure if PEBS is available on these cpus
    case CPU_MODEL(6, 133):
        snprintf(arch->name, 256, "Knights Mill micro arch");
        break;

    case CPU_MODEL(6, 87):
        snprintf(arch->name, 256, "Knights Landing micro arch");
        break;

        // Old Intel Xeon Phi CPUs
    case CPU_MODEL(11, 0):
        snprintf(arch->name, 256, "Knights Ferry micro arch");
        break;
    case CPU_MODEL(11, 1):
        snprintf(arch->name, 256, "Knights Corner micro arch");
        break;

        // Some old Intel arch (will probably never be supported)
    case CPU_MODEL(15, 6):
        snprintf(arch->name, 256, "Netburst micro arch");
        break;

    case CPU_MODEL(15, 4):
    case CPU_MODEL(15, 3):
        snprintf(arch->name, 256, "Prescott micro arch");
        break;

    case CPU_MODEL(15, 2):
        snprintf(arch->name, 256, "Northwood micro arch");
        break;
    case CPU_MODEL(15, 1):
        snprintf(arch->name, 256, "Willamette micro arch");
        break;

    case CPU_MODEL(6, 29):
    case CPU_MODEL(6, 23):
        snprintf(arch->name, 256, "Penryn micro arch");
        break;

    case CPU_MODEL(6, 15):
    case CPU_MODEL(6, 22):
        snprintf(arch->name, 256, "Core micro arch");
        break;

    case CPU_MODEL(6, 14):
        snprintf(arch->name, 256, "Modified Pentium M micro arch");
        break;

    case CPU_MODEL(6, 21):
    case CPU_MODEL(6, 13):
    case CPU_MODEL(6, 9):
        snprintf(arch->name, 256, "Pentium M micro arch");
        break;

        // Intel Atom/ Celeron CPUs (will probably never be supported)
    case CPU_MODEL(6, 134):
        snprintf(arch->name, 256, "Tremont micro arch");
        break;

    case CPU_MODEL(6, 122):
        snprintf(arch->name, 256, "Goldmont Plus micro arch");
        break;

    case CPU_MODEL(6, 95):
    case CPU_MODEL(6, 92):
        snprintf(arch->name, 256, "Goldmont micro arch");
        break;

    case CPU_MODEL(6, 76):
        snprintf(arch->name, 256, "Airmont micro arch");
        break;

    case CPU_MODEL(6, 55):
    case CPU_MODEL(6, 74):
    case CPU_MODEL(6, 77):
    case CPU_MODEL(6, 93):
        snprintf(arch->name, 256, "Silvermont micro arch");
        break;

    case CPU_MODEL(6, 39):
    case CPU_MODEL(6, 53):
    case CPU_MODEL(6, 54):
        snprintf(arch->name, 256, "Saltwell micro arch");
        break;

    case CPU_MODEL(6, 28):
    case CPU_MODEL(6, 38):
        snprintf(arch->name, 256, "Bonnell micro arch");
        break;
    }
}

unsigned char get_family(unsigned int archi_id)
{
    return (archi_id >> (8 * 0)) & 0xff;
}

unsigned char get_model(unsigned int archi_id)
{
    return (archi_id >> (8 * 1)) & 0xff;
}

/**
 * Globals in a shared lib are handled in such a way that each process
 * mapping the lib has its own copy of these globals.
 */
unsigned int nb_numa_nodes;
int numa_node_to_cpu[MAX_NB_NUMA_NODES];
unsigned int perf_event_mlock_kb;
struct archi *current_archi;
char *model_name = NULL;
int curr_err;
int enter=0;
int count11 = 0;

// struct link_fd_measure
// {
//     struct link_fd_measure *next;
//     int fd;
//     struct numap_sampling_measure *measure;
// };

// struct link_fd_measure *link_fd_measure = NULL;
pthread_mutex_t link_fd_lock;

/**
 * Special function called each time a process using the lib is
 * started.
 */
__attribute__((constructor(101))) void init(void)
{

    int node;
    int cpu;

    // Supported_archs

    // Check architecture
    FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
    char *arg = NULL;
    size_t size = 0;
    unsigned char family = 0;
    unsigned char model = 0;
    const char *family_string = "cpu family\t";
    const char *model_string = "model\t";
    const char *model_name_string = "model name\t";
    while (getline(&arg, &size, cpuinfo) != -1 && (!family || !model || !model_name))
    {
        if (strncmp(family_string, arg, strlen(family_string)) == 0)
        {
            strtok(arg, ":");
            family = atoi(strtok(NULL, "\n"));
        }
        else if (strncmp(model_string, arg, strlen(model_string)) == 0)
        {
            strtok(arg, ":");
            model = atoi(strtok(NULL, "\n"));
        }
        else if (strncmp(model_name_string, arg, strlen(model_name_string)) == 0)
        {
            strtok(arg, ":");
            char *model_name_strtok = strtok(NULL, "\n");
            int len = strlen(model_name_strtok);
            model_name = malloc((len + 1) * sizeof(char));
            strcpy(model_name, model_name_strtok);
        }
    }
    free(arg);
    fclose(cpuinfo);
    current_archi = malloc(sizeof(struct archi));
    get_archi(CPU_MODEL(family, model), current_archi);

    // Get numa configuration
    int available = numa_available();
    if (available == -1)
    {
        nb_numa_nodes = -1;
    }
    else
    {
        nb_numa_nodes = numa_num_configured_nodes();
        int nb_cpus = numa_num_configured_cpus();
        for (node = 0; node < nb_numa_nodes; node++)
        {
            struct bitmask *mask = numa_allocate_cpumask();
            numa_node_to_cpus(node, mask);
            numa_node_to_cpu[node] = -1;
            for (cpu = 0; cpu < nb_cpus; cpu++)
            {
                if (*(mask->maskp) & (1 << cpu))
                {
                    numa_node_to_cpu[node] = cpu;
                    break;
                }
            }
            numa_bitmask_free(mask);
            if (numa_node_to_cpu[node] == -1)
            {
                nb_numa_nodes = -1; // to be handled properly
            }
        }
    }

    // Get perf config
    FILE *f = fopen(PERF_EVENT_MLOCK_KB_FILE, "r");
    if (!f)
    {
        fprintf(stderr, "Failed to open file: `%s'\n", PERF_EVENT_MLOCK_KB_FILE);
        exit(-1);
    }
    if (fscanf(f, "%u", &perf_event_mlock_kb) != 1)
    {
        fprintf(stderr, "Failed to read perf_event_mlock_kb from file: `%s'\n", PERF_EVENT_MLOCK_KB_FILE);
        fclose(f);
        exit(-1);
    }
    fclose(f);
}

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    if (result == NULL)
    {
        return "malloc failed in concat\n";
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *build_string(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char *result = NULL;
    vasprintf(&result, fmt, args);
    return result;
}

const char *numap_error_message(int error)
{
    switch (error)
    {
    case ERROR_NUMAP_NOT_NUMA:
        return "libnumap: numa lib not available";
    case ERROR_NUMAP_STOP_BEFORE_START:
        return "libnumap: stop called before start";
    case ERROR_NUMAP_ALREADY_STARTED:
        return "libnumap: start called again before stop";
    case ERROR_NUMAP_ARCH_NOT_SUPPORTED:
        return build_string("libnumap: architecture not supported: %s (family %d, model %d)",
                            model_name, get_family(current_archi->id), get_model(current_archi->id));
    case ERROR_NUMAP_READ_SAMPLING_ARCH_NOT_SUPPORTED:
        return build_string("libnumap: read sampling not supported on architecture: %s (family %d, model %d)",
                            model_name, get_family(current_archi->id), get_model(current_archi->id));
    case ERROR_NUMAP_WRITE_SAMPLING_ARCH_NOT_SUPPORTED:
        return build_string("libnumap: write sampling not supported on architecture: %s (family %d, model %d)",
                            model_name, get_family(current_archi->id), get_model(current_archi->id));
    case ERROR_PERF_EVENT_OPEN:
        return build_string("libnumap: error when calling perf_event_open: %s", strerror(errno));
    case ERROR_PFM:
        return build_string("libnumap: error when initializing pfm: %s", pfm_strerror(curr_err));
    case ERROR_READ:
        return "libnumap: error while trying to read counter";
    default:
        return "libnumap: unknown error";
    }
}

int set_signal_handler(void (*handler)(int, siginfo_t *, void *))
{
    struct sigaction sigoverflow;
    memset(&sigoverflow, 0, sizeof(struct sigaction));
    sigoverflow.sa_sigaction = handler;
    //设置信号处理器的处理函数为handler
    sigoverflow.sa_flags = SA_SIGINFO;
    //指定信号处理器使用SA_SIGINFO标志,在信号处理函数中获取关于信号的附加信息

    if (sigaction(SIGIO, &sigoverflow, NULL) < 0)
    {
        fprintf(stderr, "could not set up signal handler\n");
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    // printf("\nset_signal!!!\n");
    //将handler与SIGIO信号关联，从而在PMU溢出时调用该函数
    return 0;
}

// int set_signal_handler(void (*handler)(int, siginfo_t *, void *))
// {
//     struct sigaction sigusr2;
//     memset(&sigusr2, 0, sizeof(struct sigaction));
//     sigusr2.sa_sigaction = handler;
//     sigusr2.sa_flags = SA_SIGINFO;

//     if (sigaction(SIGUSR2, &sigusr2, NULL) < 0)
//     {
//         fprintf(stderr, "could not set up signal handler\n");
//         perror("sigaction");
//         exit(EXIT_FAILURE);
//     }
//     printf("\nset_signal!!!\n");

//     return 0;
// }


int numap_init(void)
{

    if (nb_numa_nodes == -1)
    {
        return ERROR_NUMAP_NOT_NUMA;
    }

    if (current_archi == NULL)
    {
        return ERROR_NUMAP_ARCH_NOT_SUPPORTED;
    }

    curr_err = pfm_initialize();
    if (curr_err != PFM_SUCCESS)
    {
        return ERROR_PFM;
    }

    link_fd_measure = NULL;
    pthread_mutex_init(&link_fd_lock, NULL);

    // Open thread-specific file
    // char filename[50];
    // snprintf(filename, sizeof(filename), "virtualAddr_%lu.txt", (unsigned long)pthread_self());

    // printf("\nwhat is the thread now:%lu\n",(unsigned long)pthread_self());

    // Store file descriptor in thread-specific structure
    // thread_array[tid].fp = fopen(filename, "w");
    // if (thread_array[tid].fp == NULL)
    // {
    //     printf("Error opening file for thread %lu!\n", (unsigned long)tid);
    //     return 1;
    // }
    // fpVirtualAddr = fopen("virtualAddr.txt", "w");
    // if (fpVirtualAddr == NULL)
    // {
    //     printf("Error opening file!\n");
    //     return 1;
    // }

    return 0;
}

int numap_counting_init_measure(struct numap_counting_measure *measure)
{

    measure->nb_nodes = nb_numa_nodes;
    measure->started = 0;
    return 0;
}

// void refresh_wrapper_handler(int signum, siginfo_t *info, void *ucontext){
//     fprintf(stderr,"\nhere,refresh!!!!\n");
//     fflush(stdout);
// }
struct timespec last_time;



int numap_sampling_set_measure_handler(struct numap_sampling_measure *measure, void (*handler)(struct numap_sampling_measure *, int), int nb_refresh)
{
    // Set signal handler
    // may be given as function argument later
    measure->handler = handler;
    if (nb_refresh <= 0)
    {
        fprintf(stderr, "Undefined behaviour : nb_refresh %d <= 0\n", nb_refresh);
        exit(EXIT_FAILURE);
    }
    measure->nb_refresh = nb_refresh;

    printf("\nset measure_handler success!!\n");

    return 0;
}

int __numap_counting_start(struct numap_counting_measure *measure, struct perf_event_attr *pe_attr_read, struct perf_event_attr *pe_attr_write)
{

    /**
     * Check everything is ok
     */
    if (measure->started != 0)
    {
        return ERROR_NUMAP_ALREADY_STARTED;
    }
    else
    {
        measure->started++;
    }

    // Open the events on each NUMA node with Linux system call
    int HwConfig[2] = {PERF_COUNT_HW_INSTRUCTIONS, PERF_COUNT_HW_CPU_CYCLES};
    int HwCacheId[2] = {PERF_COUNT_HW_CACHE_L1D, PERF_COUNT_HW_CACHE_DTLB};
    int HwCacheOpId[3] = {PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_OP_WRITE, PERF_COUNT_HW_CACHE_OP_PREFETCH};
    int HwCacheOpResultId[2] = {PERF_COUNT_HW_CACHE_RESULT_ACCESS, PERF_COUNT_HW_CACHE_RESULT_MISS};
    pe_attr_read->type = PERF_TYPE_HW_CACHE;
    pe_attr_write->type = PERF_TYPE_HW_CACHE;

    for (int node = 0; node < measure->nb_nodes; node++)
    {
        // measure access
        pe_attr_read->config = (HwCacheId[node]) | (HwCacheOpId[0] << 8) | (HwCacheOpResultId[0] << 16);
        measure->fd_reads[node][0] = perf_event_open(pe_attr_read, 0, -1, -1, 0);
        // pid_t tid = syscall(SYS_gettid);
        // printf("\nnow perf syscall thread is %lu\n",(unsigned long)tid);
        if (measure->fd_reads[node][0] == -1)
        {
            return ERROR_PERF_EVENT_OPEN;
        }

        // measure miss
        pe_attr_read->config = (HwCacheId[node]) | (HwCacheOpId[0] << 8) | (HwCacheOpResultId[1] << 16);
        measure->fd_reads[node][1] = perf_event_open(pe_attr_read, 0, -1, -1, 0);
        if (measure->fd_reads[node][1] == -1)
        {
            return ERROR_PERF_EVENT_OPEN;
        }
    }

    // Starts measure
    for (int node = 0; node < measure->nb_nodes; node++)
    {
        for (int i = 0; i < 2; i++)
        {
            ioctl(measure->fd_reads[node][i], PERF_EVENT_IOC_RESET, 0);
            ioctl(measure->fd_reads[node][i], PERF_EVENT_IOC_ENABLE, 0);
        }
    }

    /*
        pe_attr_read->type = PERF_TYPE_HW_CACHE;
        pe_attr_read->config = (HwCacheId[0]) | (HwCacheOpId[0] << 8) | (HwCacheOpResultId[0] << 16);
        measure->fd_reads[0][0] = perf_event_open(pe_attr_read, 0, -1, -1, 0);
        if (measure->fd_reads[0][0] == -1)
        {
            return ERROR_PERF_EVENT_OPEN;
        }
        ioctl(measure->fd_reads[0][0], PERF_EVENT_IOC_RESET, 0);
        ioctl(measure->fd_reads[0][0], PERF_EVENT_IOC_ENABLE, 0);
        */

    return 0;
}

int numap_counting_start(struct numap_counting_measure *measure)
{

    // Set attribute parameter for counting reads using pfmlib
    struct perf_event_attr pe_attr_read;
    memset(&pe_attr_read, 0, sizeof(pe_attr_read));
    pe_attr_read.size = sizeof(pe_attr_read);
    pfm_perf_encode_arg_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.size = sizeof(pfm_perf_encode_arg_t);
    arg.attr = &pe_attr_read;
    char *fstr;
    arg.fstr = &fstr;
    curr_err = pfm_get_os_event_encoding(current_archi->sampling_read_event, PFM_PLM0 | PFM_PLM3, PFM_OS_PERF_EVENT, &arg);
    if (curr_err != PFM_SUCCESS)
    {
        return ERROR_PFM;
    }
    // Other parameters
    pe_attr_read.disabled = 1;
    pe_attr_read.exclude_kernel = 1;
    pe_attr_read.exclude_hv = 1;

    // Set attribute parameter for counting writes using pfmlib
    struct perf_event_attr pe_attr_write;

    return __numap_counting_start(measure, &pe_attr_read, &pe_attr_write);
}

int numap_counting_stop(struct numap_counting_measure *measure)
{

    for (int node = 0; node < measure->nb_nodes; node++)
    {
        for (int i = 0; i < 2; i++)
        {
            ioctl(measure->fd_reads[node][i], PERF_EVENT_IOC_DISABLE, 0);
            if (read(measure->fd_reads[node][i], &measure->reads_count[node][i],
                     sizeof(long long)) == -1)
            {
                printf("\nfunc=%s, node=%d, i=%d\n", __func__, node, i);
                return ERROR_READ;
            }
            close(measure->fd_reads[node][i]);
        }
    }

    /*
 long long cnt = 0;
 ioctl(measure->fd_reads[0][0], PERF_EVENT_IOC_DISABLE, 0);
 if (read(measure->fd_reads[0][0], &cnt,
          sizeof(long long)) == -1)
 {
     printf("\nThis test ...\n");
     return ERROR_READ;
 }
 close(measure->fd_reads[0][0]);
 printf("\ntest cnt=%lld\n", cnt);
 */

    return 0;
}

int numap_counting_print(struct numap_counting_measure *measure)
{
    printf("\n L1data cache access=%lld, L1Data cache miss=%lld, Data TLB cache access=%lld, Data TLB cache miss=%lld\n",
           measure->reads_count[0][0], measure->reads_count[0][1], measure->reads_count[1][0], measure->reads_count[1][1]);
    return 0;
}

void sigusr2_handler(int signum) {
    // 什么都不做，只是忽略 SIGUSR2 信号
    fprintf(stderr,"\nfunc=%s,123236666\n");
    fflush(stdout);
}

int numap_sampling_init_measure(struct numap_sampling_measure *measure, int nb_threads, int sampling_rate,
 int mmap_pages_count)
{

    int thread;
    measure->started = 0;
    measure->page_size = (size_t)sysconf(_SC_PAGESIZE);
    measure->mmap_pages_count = mmap_pages_count;
    measure->mmap_len = measure->page_size + measure->page_size * measure->mmap_pages_count;
    measure->nb_threads = nb_threads;
    measure->sampling_rate = sampling_rate;
    
    for (thread = 0; thread < measure->nb_threads; thread++)
    {
        measure->fd_per_tid[thread] = 0;
        measure->metadata_pages_per_tid[thread] = 0;
    }
    measure->handler = NULL;
    measure->total_samples = 0;
    set_signal_handler(refresh_wrapper_handler);
    // signal(SIGUSR2, sigusr2_handler);
    measure->nb_refresh = 1000; // default refresh
    printf("numap_sampling_init_measures!!!!");

    return 0;
}

static int __numap_sampling_resume(struct numap_sampling_measure *measure)
{
    int thread;
    for (thread = 0; thread < measure->nb_threads; thread++)
    {
        ioctl(measure->fd_per_tid[thread], PERF_EVENT_IOC_RESET, 0);
        fcntl(measure->fd_per_tid[thread], F_SETFL, O_ASYNC | O_NONBLOCK);
        fcntl(measure->fd_per_tid[thread], F_SETSIG, SIGIO);
        fcntl(measure->fd_per_tid[thread], F_SETOWN, measure->tids[thread]);
        ioctl(measure->fd_per_tid[thread], PERF_EVENT_IOC_REFRESH, measure->nb_refresh);
        ioctl(measure->fd_per_tid[thread], PERF_EVENT_IOC_ENABLE, 0);
    }
    return 0;
}

// static int __numap_sampling_resume(struct numap_sampling_measure *measure) {
  
//   static int hhh=0;
//   printf("\nhow many times?%d\n",hhh);
//   hhh++;
//   int thread;
//   for (thread = 0; thread < measure->nb_threads; thread++) {
//     ioctl(measure->fd_per_tid[thread], PERF_EVENT_IOC_RESET, 0);
//     //PERF_EVENT_IOC_RESET命令，将性能计数器的计数器值重置为0，准备进行新一轮的采样。
//     fcntl(measure->fd_per_tid[thread], F_SETFL, O_ASYNC|O_NONBLOCK);
//     //异步、非阻塞
//     fcntl(measure->fd_per_tid[thread], F_SETSIG, SIGUSR2);
//     //设置文件描述符的异步和非阻塞属性，允许异步IO操作，当文件描述符上有异步IO事件（例如 POLL_IN 和 POLL_HUP 事件）发生时，将触发与 SIGUSR2 信号关联的信号处理函数。
//     fcntl(measure->fd_per_tid[thread], F_SETOWN, measure->tids[thread]);
//     //将文件描述符的所有权设置为目标线程的ID measure->tids[thread]。这样，当性能计数器产生采样时，操作系统会发送SIGUSR2信号给目标线程，通知其有新的数据到达。
//     ioctl(measure->fd_per_tid[thread], PERF_EVENT_IOC_REFRESH, measure->nb_refresh);
//     //设置性能计数器的刷新周期为measure->nb_refresh，即每隔一定数量的事件刷新一次计数器
//     ioctl(measure->fd_per_tid[thread], PERF_EVENT_IOC_ENABLE, 0);
//   }
//  return 0;
// }

int numap_sampling_resume(struct numap_sampling_measure *measure)
{
    /**
     * Check everything is ok
     */
    if (measure->started != 0)
    {
        return ERROR_NUMAP_ALREADY_STARTED;
    }
    else
    {
        measure->started++;
    }

    return __numap_sampling_resume(measure);
}

int __numap_sampling_start(struct numap_sampling_measure *measure, struct perf_event_attr *pe_attr)
{

    /**
     * Check everything is ok
     */
    if (measure->started != 0)
    {
        return ERROR_NUMAP_ALREADY_STARTED;
    }
    else
    {
        measure->started++;
    }

    /**
     * Open the events for each thread in measure with Linux system call: we do per
     * thread monitoring by giving the system call the thread id and a
     * cpu = -1, this way the kernel handles the migration of counters
     * when threads are migrated. Then we mmap the result.
     */
    int cpu = -1;
    int thread;
    for (thread = 0; thread < measure->nb_threads; thread++)
    {
        if (measure->metadata_pages_per_tid[thread])
        {
            /* Already open, we can skip this one */
            continue;
        }

        measure->fd_per_tid[thread] = perf_event_open(pe_attr, 0, cpu,
                                                      -1, 0);
        if (measure->fd_per_tid[thread] == -1)
        {
            return ERROR_PERF_EVENT_OPEN;
        }
        measure->metadata_pages_per_tid[thread] = mmap(NULL, measure->mmap_len, PROT_WRITE, MAP_SHARED, measure->fd_per_tid[thread], 0);
        // if (!measure->fd_per_tid[thread]) {
        //       printf("\nperf_event_open is null\n");
        // }else {
        //     printf("\nperf_event_open exist\n");
        // }
        
        if (measure->metadata_pages_per_tid[thread] == MAP_FAILED)
        {
            if (errno == EPERM)
            {
                fprintf(stderr, "Permission error mapping pages.\n"
                                "Consider increasing /proc/sys/kernel/perf_event_mlock_kb,\n"
                                "(mmap length parameter = %zd > perf_event_mlock_kb = %u)\n",
                        measure->mmap_len, (perf_event_mlock_kb * 1024));
            }
            else
            {
                fprintf(stderr, "Couldn't mmap file descriptor: %s - errno = %d\n",
                        strerror(errno), errno);
            }
            exit(EXIT_FAILURE);
        }
        struct link_fd_measure *new_lfm = malloc(sizeof(struct link_fd_measure));
        new_lfm->fd = measure->fd_per_tid[thread];
        new_lfm->measure = measure;

        pthread_mutex_lock(&link_fd_lock);
        new_lfm->next = link_fd_measure;
        link_fd_measure = new_lfm;
        pthread_mutex_unlock(&link_fd_lock);
    }

    __numap_sampling_resume(measure);

    return 0;
}

int numap_sampling_read_supported()
{

    if (strcmp(current_archi->sampling_read_event, NOT_SUPPORTED) == 0)
    {
        return 0;
    }
    return 1;
}

int numap_sampling_read_start_generic(struct numap_sampling_measure *measure, uint64_t sample_type)
{

    // Checks that read sampling is supported before calling pfm
    if (!numap_sampling_read_supported())
    {
        return ERROR_NUMAP_READ_SAMPLING_ARCH_NOT_SUPPORTED;
    }

    // Set attribute parameter for perf_event_open using pfmlib
    struct perf_event_attr pe_attr;
    memset(&pe_attr, 0, sizeof(pe_attr));
    pe_attr.size = sizeof(pe_attr);
    pfm_perf_encode_arg_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.size = sizeof(pfm_perf_encode_arg_t);
    arg.attr = &pe_attr;
    char *fstr;
    arg.fstr = &fstr;

    curr_err = pfm_get_os_event_encoding(current_archi->sampling_write_event, PFM_PLM0 | PFM_PLM3, PFM_OS_PERF_EVENT, &arg);
    if (curr_err != PFM_SUCCESS)
    {
        return ERROR_PFM;
    }

    // Sampling parameters
    pe_attr.sample_period = measure->sampling_rate;
    pe_attr.sample_type = sample_type;
    pe_attr.mmap = 1;
    pe_attr.task = 1;
    pe_attr.precise_ip = 2;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
    pe_attr.use_clockid = 1;
    pe_attr.clockid = CLOCK_MONOTONIC_RAW;
#else
#warning NUMAP: using clockid is not possible on kernel version < 4.1. This feature will be disabled.
#endif
    // Other parameters
    pe_attr.disabled = 1;
    pe_attr.exclude_kernel = 1;
    pe_attr.exclude_hv = 1;
    // pe_attr.inherit =1;

    return __numap_sampling_start(measure, &pe_attr);
}

int numap_sampling_read_start(struct numap_sampling_measure *measure)
{
    return numap_sampling_read_start_generic(measure, PERF_SAMPLE_TIME | PERF_SAMPLE_ADDR | PERF_SAMPLE_WEIGHT | PERF_SAMPLE_DATA_SRC);
}

int numap_sampling_read_stop(struct numap_sampling_measure *measure)
{

    // Check everything is ok
    if (measure->started == 0)
    {
        return ERROR_NUMAP_STOP_BEFORE_START;
    }
    else
    {
        measure->started = 0;
    }
    int thread;
    for (thread = 0; thread < measure->nb_threads; thread++)
    {
        ioctl(measure->fd_per_tid[thread], PERF_EVENT_IOC_DISABLE, 0);
    }

    return 0;
}

int numap_sampling_write_supported()
{

    if (strcmp(current_archi->sampling_write_event, NOT_SUPPORTED) == 0)
    {
        return 0;
    }
    return 1;
}

int howmany_times = 0;

int numap_sampling_write_start_generic(struct numap_sampling_measure *measure, uint64_t sample_type)
{
    // Checks that write sampling is supported before calling pfm
    howmany_times ++;
    // printf("\n how many perf syscall : %d\n",howmany_times);
    if (!numap_sampling_write_supported())
    {
        return ERROR_NUMAP_WRITE_SAMPLING_ARCH_NOT_SUPPORTED;
    }

    // Set attribute parameter for perf_event_open using pfmlib
    struct perf_event_attr pe_attr;
    memset(&pe_attr, 0, sizeof(pe_attr));
    pe_attr.size = sizeof(pe_attr);
    pfm_perf_encode_arg_t arg;
    memset(&arg, 0, sizeof(arg));
    arg.size = sizeof(pfm_perf_encode_arg_t);
    arg.attr = &pe_attr;
    char *fstr;
    arg.fstr = &fstr;
    curr_err = pfm_get_os_event_encoding(current_archi->sampling_write_event, PFM_PLM0 | PFM_PLM3, PFM_OS_PERF_EVENT, &arg);
    if (curr_err != PFM_SUCCESS)
    {
        return ERROR_PFM;
    }

    // Sampling parameters
    pe_attr.sample_period = measure->sampling_rate;
    pe_attr.sample_type = sample_type;
    pe_attr.mmap = 1;
    pe_attr.task = 1;
    pe_attr.precise_ip = 2;

    // Other parameters
    pe_attr.disabled = 1;
    pe_attr.exclude_kernel = 1;
    pe_attr.exclude_hv = 1;

    printf("\n%d numap_sampling_write_start_generic!!!\n",howmany_times);

    return __numap_sampling_start(measure, &pe_attr);
}

int numap_sampling_write_start(struct numap_sampling_measure *measure)
{
    return numap_sampling_write_start_generic(measure, PERF_SAMPLE_TIME | PERF_SAMPLE_ADDR | PERF_SAMPLE_WEIGHT | PERF_SAMPLE_DATA_SRC);
}

int numap_sampling_write_stop(struct numap_sampling_measure *measure)
{
    return numap_sampling_write_stop_generic(measure);
}

int numap_sampling_write_stop_generic(struct numap_sampling_measure *measure)
{

    // Check everything is ok
    if (measure->started == 0)
    {
        return ERROR_NUMAP_STOP_BEFORE_START;
    }
    else
    {
        measure->started = 0;
    }
    int thread;
    for (thread = 0; thread < measure->nb_threads; thread++)
    {
        ioctl(measure->fd_per_tid[thread], PERF_EVENT_IOC_DISABLE, 0);
    }

    return 0;
}

int numap_sampling_end(struct numap_sampling_measure *measure)
{
    int thread;

    struct link_fd_measure *current_lfm;
    while (link_fd_measure != NULL)
    {
        current_lfm = link_fd_measure;
        link_fd_measure = link_fd_measure->next;
        free(current_lfm);
    }

    for (thread = 0; thread < measure->nb_threads; thread++)
    {
        munmap(measure->metadata_pages_per_tid[thread], measure->mmap_len);
        close(measure->fd_per_tid[thread]);
    }
    return 0;
}

int enter_numap_generic_handler;
extern int nb_threads;

void numap_generic_handler(FILE *thread_fp,
                           struct numap_sampling_measure *sm,
                           int fd,
                           enum access_type access_type)
{
    // for (int i = 0; i < sm->nb_threads; i++)
    // {
    //     numap_sampling_read_stop(sm);
    //     __process_samples(sm, ACCESS_READ);
    //     numap_sampling_resume(sm);
    // }
    // fprintf(stderr,"\nfunc=%s,66666666\n");
    // fflush(stdout);
    // exit(-1);
        for (int i = 0; i < nb_threads; i++)
    {
        // enter_numap_generic_handler++;
        // printf("enter numap_generic_handler for %d times",enter_numap_generic_handler);
        numap_sampling_write_stop(thread_ranks[rank].sm_wr);
        _process_samples(thread_fp, thread_ranks[rank].sm_wr, ACCESS_WRITE);
        numap_sampling_resume(thread_ranks[rank].sm_wr);
    }
}

void numap_read_handler(FILE *thread_fp, struct numap_sampling_measure *sm, int fd)
{
    // printf("\nfunc=%s, test999...\n");
    // fflush(stdout);
    numap_generic_handler(thread_fp, sm, fd, ACCESS_READ);
}

void numap_write_handler(FILE *thread_fp, struct numap_sampling_measure *sm, int fd)
{
    // printf("\nfunc=%s, test999...\n");
    // fprintf(stderr,"\ntest999\n");
    numap_generic_handler(thread_fp, sm, fd, ACCESS_WRITE);
}

void mem_sampling_colls() {

//   int res = numap_sampling_read_stop(&sm);
//   if(res < 0) {
//     printf("numap_sampling_stop error : %s\n", numap_error_message(res));
//     abort();
//   }
   int static collect_num=0;
   collect_num++;
  // Stop memory write access sampling if needed
  printf("\nwhich thread %d enter mem_sampling_colls\n",(int)rank);
  
  if(thread_ranks[rank].sm_wr->started == 0){
        int res = numap_sampling_write_stop(&sm_wr);
    if(res < 0) {
      printf("numap_sampling_stop error : %s\n", numap_error_message(res));
      abort();
    }
//   printf("\ncollect_numbers:%d\n",collect_num);
//   fflush(stdout);
  _process_samples(thread_ranks[rank].thread_fp,&sm_wr, ACCESS_WRITE);
  //分析读取访问采样数据，并将结果保存在sm结构体中。
  }
}

void mem_sampling_resume() {
    //暂时不采样读的操作
//   // Resume read sampling
//   int res = numap_sampling_resume(&sm);
//   //恢复内存读取采样,&sm是指向内存采样结构体的指针
//   if(res < 0) {
//     fprintf(stderr, "numap_sampling_resume error : %s\n", numap_error_message(res));
//     if(res ==  ERROR_PERF_EVENT_OPEN && errno == EACCES) {
//       fprintf(stderr, "try running 'echo 1 > /proc/sys/kernel/perf_event_paranoid' to fix the problem\n");
//     }
//     abort();
//   }

// Resume write sampling if needed
  if (numap_sampling_write_supported()) {
    int res = numap_sampling_resume(&sm_wr);
    if(res < 0) {
      fprintf(stderr, "numap_sampling_resume error : %s\n", numap_error_message(res));
      abort();
    }
  }

}

void sig_handler(int signal) {
      printf("\nenter sig_handler!!\n");
      mem_sampling_colls();
      mem_sampling_resume();
    }