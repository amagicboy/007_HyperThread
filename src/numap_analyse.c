#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
// #include "numap.h"
#include "handle.h"

struct sample_list *samples = NULL;
pthread_mutex_t sample_list_lock;
static long long nb_sample_buffers = 0;
struct mem_counters global_counters[2];
static uint64_t nb_samples_total = 0;
static size_t total_buffer_size = 0;
extern FILE *fpVirtualAddr;

#define INIT_COUNTER(c)            \
    do                             \
    {                              \
        c.count = 0;               \
        c.min_weight = UINT64_MAX; \
        c.max_weight = 0;          \
        c.sum_weight = 0;          \
    } while (0)

/* initialize a mem_counters structure */
void init_mem_counter(struct mem_counters *counters)
{
    counters->total_count = 0;
    counters->total_weight = 0;
    counters->na_miss_count = 0;

    INIT_COUNTER(counters->cache1_hit);
    INIT_COUNTER(counters->cache2_hit);
    INIT_COUNTER(counters->cache3_hit);
    INIT_COUNTER(counters->lfb_hit);
    INIT_COUNTER(counters->local_ram_hit);
    INIT_COUNTER(counters->remote_ram_hit);
    INIT_COUNTER(counters->remote_cache_hit);
    INIT_COUNTER(counters->io_memory_hit);
    INIT_COUNTER(counters->uncached_memory_hit);
    INIT_COUNTER(counters->cache1_miss);
    INIT_COUNTER(counters->cache2_miss);
    INIT_COUNTER(counters->cache3_miss);
    INIT_COUNTER(counters->lfb_miss);
    INIT_COUNTER(counters->local_ram_miss);
    INIT_COUNTER(counters->remote_ram_miss);
    INIT_COUNTER(counters->remote_cache_miss);
    INIT_COUNTER(counters->io_memory_miss);
    INIT_COUNTER(counters->uncached_memory_miss);
    counters->virtual_addr_head.next = NULL;
}

#define UPDATE_COUNTER(counter, sample)          \
    do                                           \
    {                                            \
        counter.count++;                         \
        if (sample->weight < counter.min_weight) \
            counter.min_weight = sample->weight; \
        if (sample->weight > counter.max_weight) \
            counter.max_weight = sample->weight; \
        counter.sum_weight += sample->weight;    \
    } while (0)

void update_counters(FILE *thread_fp,
                     struct mem_counters *counters,
                     struct mem_sample *sample,
                     enum access_type access_type)
// void update_counters(struct mem_counters *counters,
//                      struct mem_sample *sample,
//                      enum access_type access_type)
{

    counters[access_type].total_count++;
    counters[access_type].total_weight += sample->weight;

    if (sample->data_src.mem_lvl & PERF_MEM_LVL_NA)
    {
        counters[access_type].na_miss_count++;
    }

    if (sample->data_src.mem_lvl & PERF_MEM_LVL_L1)
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].cache1_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].cache1_miss, sample);
    }

    if (sample->data_src.mem_lvl & PERF_MEM_LVL_L2)
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].cache2_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].cache2_miss, sample);
    }

    if (sample->data_src.mem_lvl & PERF_MEM_LVL_L3)
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].cache3_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].cache3_miss, sample);
    }

    if (sample->data_src.mem_lvl & PERF_MEM_LVL_LFB)
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].lfb_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].lfb_miss, sample);
    }

    if (sample->data_src.mem_lvl & PERF_MEM_LVL_LOC_RAM)
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].local_ram_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].local_ram_miss, sample);
    }

    if ((sample->data_src.mem_lvl & PERF_MEM_LVL_REM_RAM1) ||
        (sample->data_src.mem_lvl & PERF_MEM_LVL_REM_RAM2))
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].remote_ram_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].remote_ram_miss, sample);
    }

    if ((sample->data_src.mem_lvl & PERF_MEM_LVL_REM_CCE1) ||
        (sample->data_src.mem_lvl & PERF_MEM_LVL_REM_CCE2))
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].remote_cache_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].remote_cache_miss, sample);
    }

    if (sample->data_src.mem_lvl & PERF_MEM_LVL_IO)
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].io_memory_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].io_memory_miss, sample);
    }

    if (sample->data_src.mem_lvl & PERF_MEM_LVL_UNC)
    {
        if (sample->data_src.mem_lvl & PERF_MEM_LVL_HIT)
            UPDATE_COUNTER(counters[access_type].uncached_memory_hit, sample);
        else if (sample->data_src.mem_lvl & PERF_MEM_LVL_MISS)
            UPDATE_COUNTER(counters[access_type].uncached_memory_miss, sample);
    }
    // fprintf(thread_fp, "virtual addr=%" PRIx64 " timestamps=%" PRIx64 "\n", sample->addr, sample->timestamp);
    fprintf(thread_fp, "virtual addr=%" PRIx64 " timestamps=%ld"  "\n", sample->addr, sample->timestamp);
    // Write data to the thread-specific file
    // fprintf(thread_fp, "virtual addr=%" PRIx64 " timestamps=%" PRIx64 "\n", sample->addr, sample->timestamp);
}

// static void __analyze_buffer(FILE *thread_fp, struct sample_list *samples,
//                              int *nb_samples,
//                              int *found_samples)
static void __analyze_buffer(FILE *thread_fp,
                             struct sample_list *samples,
                             int *nb_samples,
                             int *found_samples)
{
    if (samples->data_tail == samples->data_head)
        return;

    unsigned start_cpt = samples->data_tail;
    unsigned stop_cpt = samples->data_head;
    uintptr_t reset_cpt = samples->buffer_size;
    unsigned cur_cpt = start_cpt;

    if (stop_cpt < start_cpt)
    {
        /* the buffer is a ring buffer and we need to explore both parts of the "ring": */

        // ------------------------------------------------------
        // | second_block   |                  |first_block      |
        // -------------------------------------------------------
        //               stop_cpt            start_cpt         reset_cpt

        /* in order to make the while condition easier to understand, let's first analyze
         * the first block. When the end of the buffer is reached, we reset counters so that
         * the second block is analyzed
         */
        stop_cpt = reset_cpt;
    }
    // printf("\nfunc=%s, test100...\n", __func__);
    while (cur_cpt < stop_cpt)
    {
        struct perf_event_header *event = (struct perf_event_header *)((uintptr_t)samples->buffer + cur_cpt);
        if (event->size == 0)
        {
            fprintf(stderr, "Error: invalid header size = 0. %p\n", samples);
            abort();
        }
        if (event->type == PERF_RECORD_SAMPLE)
        {
            struct mem_sample *sample = (struct mem_sample *)((char *)(event) + sizeof(struct perf_event_header));
            uint8_t frontier_buffer[event->size];

            if (cur_cpt + event->size > reset_cpt)
            {
                // we reached the end of the buffer. The event is split in two parts:
                // ------------------------------------------------------
                // | second_part    |                  | first_part      |
                // -------------------------------------------------------
                //                                   cur_cpt         reset_cpt
                size_t first_part_size = reset_cpt - cur_cpt;
                size_t second_part_size = event->size - first_part_size;

                // copy the event in a contiguous buffer
                memcpy(frontier_buffer, sample, first_part_size); // copy the first part
                memcpy(&frontier_buffer[first_part_size], samples->buffer, second_part_size);
                sample = (struct mem_sample *)frontier_buffer;
            }
            (*nb_samples)++;
            // update_counters(thread_fp,global_counters, sample, samples->access_type);
            update_counters(thread_fp,  global_counters, sample, samples->access_type);
        }
        cur_cpt += event->size;
        if (cur_cpt >= reset_cpt && reset_cpt != samples->data_head)
        {
            cur_cpt -= reset_cpt;
            stop_cpt = samples->data_head;
        }
    }
}

// static void __copy_buffer(struct sample_list *sample_list,
//                           int *nb_samples,
//                           int *found_samples)
                          
static void __copy_buffer(FILE *thread_fp,
                          struct sample_list *sample_list,
                          int *nb_samples,
                          int *found_samples)
{
    if (sample_list->data_head == sample_list->data_tail)
        /* nothing to do */
        return;
    struct sample_list *new_sample_buffer = malloc(sizeof(struct sample_list));
    size_t buffer_size = sample_list->data_head - sample_list->data_tail;

    if (sample_list->data_head < sample_list->data_tail)
    {
        /* the buffer is a ring buffer and we need to explore both parts of the "ring": */
        // ------------------------------------------------------
        // | second_block   |                  |first_block      |
        // -------------------------------------------------------
        //               data_head          data_tail        buffer_size
        buffer_size = sample_list->buffer_size - sample_list->data_tail + sample_list->data_head;
    }

    new_sample_buffer->buffer = malloc(buffer_size);
    new_sample_buffer->access_type = sample_list->access_type;
    new_sample_buffer->buffer_size = buffer_size;

    new_sample_buffer->data_tail = 0;
    new_sample_buffer->data_head = buffer_size;

    if (sample_list->data_head < sample_list->data_tail)
    {
        /* copy the first block */
        size_t first_block_size = sample_list->buffer_size - sample_list->data_tail;
        uintptr_t start_addr = (uintptr_t)sample_list->buffer + (uintptr_t)sample_list->data_tail;
        memcpy(new_sample_buffer->buffer, (void *)start_addr, first_block_size);

        size_t second_block_size = sample_list->data_head;
        start_addr = (uintptr_t)new_sample_buffer->buffer + (uintptr_t)first_block_size;
        memcpy((void *)start_addr, &sample_list->buffer[0], second_block_size);
    }
    else
    {
        /* data is already contiguous */
        uintptr_t start_addr = (uintptr_t)sample_list->buffer + (uintptr_t)sample_list->data_tail;
        memcpy(new_sample_buffer->buffer, (void *)start_addr, buffer_size);
    }
    struct perf_event_header *event = (struct perf_event_header *)(new_sample_buffer->buffer);
    assert(event->type < PERF_RECORD_MAX);
    assert(event->size > 0);

    new_sample_buffer->start_date = sample_list->start_date;
    new_sample_buffer->stop_date = sample_list->stop_date;
    new_sample_buffer->thread_rank = sample_list->thread_rank;

    pthread_mutex_lock(&sample_list_lock);
    new_sample_buffer->next = samples;
    samples = new_sample_buffer;
    nb_sample_buffers++;
    pthread_mutex_unlock(&sample_list_lock);

    // printf("\nfunc=%s, test010...\n", __func__);
    while (samples)
    {
        // printf("\nfunc=%s, test011...\n", __func__);
        int nb_samples1 = 0;
        int found_samples1 = 0;
        // __analyze_buffer(samples, &nb_samples1, &found_samples1);
        __analyze_buffer(thread_fp, samples, &nb_samples1, &found_samples1);
        nb_samples_total += nb_samples1;
        total_buffer_size += samples->buffer_size;
        struct sample_list *prev = samples;
        samples = samples->next;
    }
}

void mem_sampling_collect_samples() {

  // Stop memory read access sampling
//   int res = numap_sampling_read_stop(&sm);
//   if(res < 0) {
//     printf("numap_sampling_stop error : %s\n", numap_error_message(res));
//     abort();
//   }

  // Stop memory write access sampling if needed
  if (numap_sampling_write_supported()) {
    int res = numap_sampling_write_stop(&sm_wr);
    if(res < 0) {
      printf("numap_sampling_stop error : %s\n", numap_error_message(res));
      abort();
    }
  }

  // Analyze samples
//   start_tick(analyze_samples);
//   __process_samples(thread_ranks[rank].thread_fp, &sm, ACCESS_READ);
  if (numap_sampling_write_supported()) {
    __process_samples(thread_ranks[rank].thread_fp, &sm_wr, ACCESS_WRITE);
  }
}



// void _process_samples(struct numap_sampling_measure *sm,
//                       enum access_type access_type)
void _process_samples(FILE *thread_fp,
                      struct numap_sampling_measure *sm,
                      enum access_type access_type)
{
    pthread_mutex_init(&sample_list_lock, NULL);
    int nb_samples = 0;
    int found_samples = 0;
    for (int thread = 0; thread < sm->nb_threads; thread++)
    {
        struct perf_event_mmap_page *metadata_page = sm->metadata_pages_per_tid[thread];
        uint64_t data_head = metadata_page->data_head % metadata_page->data_size;
        rmb();

        // int rank = get_thread_info(sm->tids[thread])->rank;
        //  assert(rank >=0);
        struct sample_list samples = {
            .next = NULL,
            .buffer = (struct perf_event_header *)((uint8_t *)metadata_page + metadata_page->data_offset),
            .data_tail = metadata_page->data_tail,
            .data_head = data_head,
            .buffer_size = metadata_page->data_size,
            .access_type = access_type,
            //   .start_date = start_date,
            //   .stop_date = new_date(),
            //   .thread_rank = rank,
        };
        // printf("\nfunc=%s, test001\n", __func__);
        __copy_buffer(thread_fp,&samples, &nb_samples, &found_samples);

        metadata_page->data_tail = data_head;
    }
}

// int numap_sampling_print(struct numap_sampling_measure *measure, char print_samples)
int numap_sampling_print(FILE *thread_fp, struct numap_sampling_measure *measure, char print_samples)
{
    init_mem_counter(&global_counters[0]);
    init_mem_counter(&global_counters[1]);

    for (int thread = 0; thread < measure->nb_threads; thread++)
    {
        // printf("\nfunc=%s, test000\n", __func__);
        // _process_samples(measure, ACCESS_READ);
        // _process_samples(measure, ACCESS_WRITE);
        _process_samples(thread_fp,measure, ACCESS_WRITE);
        printf("\nb_sample_buffers=%d\n", nb_sample_buffers);
        // printf("\n");
        // printf("\nfunc=%s, total header cnt=%lld\n", __func__, headercnt);
        // printf("head = %" PRIu64 " compared to max = %zu\n", head, measure->mmap_len);
        // printf("Thread %d: %-8d samples\n", thread, total_count);
        // printf("Thread %d: %-8d %-30s %0.3f%%\n", thread, cache1_count, "local cache 1", (100.0 * cache1_count / total_count));
        // printf("Thread %d: %-8d %-30s %0.3f%%\n", thread, cache2_count, "local cache 2", (100.0 * cache2_count / total_count));
        // printf("Thread %d: %-8d %-30s %0.3f%%\n", thread, cache3_count, "local cache 3", (100.0 * cache3_count / total_count));
        // printf("Thread %d: %-8d %-30s %0.3f%%\n", thread, lfb_count, "local cache LFB", (100.0 * lfb_count / total_count));
        // printf("Thread %d: %-8d %-30s %0.3f%%\n", thread, memory_count, "local memory", (100.0 * memory_count / total_count));
        // printf("Thread %d: %-8d %-30s %0.3f%%\n", thread, remote_cache_count, "remote cache or local memory", (100.0 * remote_cache_count / total_count));
        // printf("Thread %d: %-8d %-30s %0.3f%%\n", thread, remote_memory_count, "remote memory", (100.0 * remote_memory_count / total_count));
        // printf("Thread %d: %-8d %-30s %0.3f%%\n", thread, na_miss_count, "unknown l3 miss", (100.0 * na_miss_count / total_count));
    }
    printf("\n");
    printf("%llu bytes processed, nb_samples_total=%llu\n", total_buffer_size, nb_samples_total);
    return 0;
}

int numap_sampling_read_print(FILE *thread_fp, struct numap_sampling_measure *measure, char print_samples)
{
    return numap_sampling_print(thread_fp, measure, print_samples);
}

int numap_sampling_write_print(FILE *thread_fp, struct numap_sampling_measure *measure, char print_samples)
{
    return numap_sampling_print(thread_fp, measure, print_samples);
}
