/* Inclusions */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <memory.h>
#include <malloc.h>
#include <cpuid.h>
#include <time.h>
#include "papi.h"
#include "emmintrin.h"

/* Definitions */
#define FAILURE 0
#define SUCCESS 1
#define IJK 0
#define IKJ 1
#define JIK 2
#define JKI 3
#define KIJ 4
#define KJI 5
#define ORDER_COUNT 6
#define EVENT_COUNT 4
#define INDEX_COUNT 8
#define NANO 1000000000

/* Macros */
#define FENCE _mm_mfence(); __get_cpuid(level, &eax, &ebx, &ecx, &edx);        \

#define SINGLE_MULT r[i][j] = r[i][j] + a[i][k] * b[k][j]; }

#define LOOP_IJK { FENCE for (i = 0; i < index; i++) for(j = 0; j < index; j++)\
  for(k = 0; k < index; k++) SINGLE_MULT

#define LOOP_IKJ { FENCE for (i = 0; i < index; i++) for(k = 0; k < index; k++)\
  for(j = 0; j < index; j++) SINGLE_MULT

#define LOOP_JIK { FENCE for (j = 0; j < index; j++) for(i = 0; i < index; i++)\
  for(k = 0; k < index; k++) SINGLE_MULT

#define LOOP_JKI { FENCE for (j = 0; j < index; j++) for(k = 0; k < index; k++)\
  for(i = 0; i < index; i++) SINGLE_MULT

#define LOOP_KIJ { FENCE for (k = 0; k < index; k++) for(i = 0; i < index; i++)\
  for(j = 0; j < index; j++) SINGLE_MULT

#define LOOP_KJI { FENCE for (k = 0; k < index; k++) for(j = 0; j < index; j++)\
  for(i = 0; i < index; i++) SINGLE_MULT


/* Type Definitions */
typedef struct m_struct {
  double** matrixa;
  double** matrixb;
  double** mresult;
} m_struct;

/* Function prototypes */
// Matrix multiplication - high level
static void papi_MxM(m_struct matrices, int index, int order, int iteration);
static void clock_MxM(m_struct matrices, int index, int order);
// I/O initialization
static void init_file();
static int init_cache_buffer(void** buffer);
static void clear_cache(void* buffer, int cache_size);
// Timers
static void init_timers();
static void start_timers();
static void end_timers();
static void output_time_results();
// PAPI
static void init_papi();
static inline void clear_papi_values();
static void start_papi(int iteration);
static void stop_papi(int iteration);
static void output_papi_results(int index, int order, int iteration);
static void end_papi();
// Matrix multiplication - low level
static m_struct init_matrices(int index);
static double** alloc_array(int index);
static void load_matrices(m_struct matrices, int index);
static void free_matrices(m_struct matrices, int index);
static void multiply_matrices(m_struct matrices, int order, int index);

// I/O clean up
static void end(int status) __attribute__((noreturn));