#include <stddef.h>

#define TYPE __INTPTR_TYPE__

#ifndef BENCHMARK

#if CASE & 1
#define CMP(a, b) ((a) < (b))
#else
#define CMP(a, b) (((a) - (b)) < 0)
#endif /* #if CASE & 1 */

#define SWAP(a, b)                                                             \
  do {                                                                         \
    const TYPE _tmp = (a);                                                     \
    (a) = (b);                                                                 \
    (b) = _tmp;                                                                \
  } while (0)

static void heap_sift_down(TYPE *const array, const size_t from,
                           const size_t to) {
#if CASE & 2
  for (size_t root = from; (root + root) <= to;) {
    size_t child = root << 1;
#else
  for (size_t child, root = from; (child = root + root) <= to;) {
#endif /* #if CASE & 2 */
    if (child < to && CMP(array[child], array[child + 1]))
      child++;
    if (!CMP(array[root], array[child]))
      break;
    SWAP(array[child], array[root]);
    root = child;
  }
}

void heapsort(TYPE *const array, const size_t num) {
  if (num > 1) {
    size_t last = num - 1;

    // heapify
    size_t start = num >> 1;
    do
      heap_sift_down(array, start, last);
    while (start--);

    // sort
    while (last > 0) {
      SWAP(array[0], array[last]);
      --last;
      heap_sift_down(array, 0, last);
    }
  }
}

#undef CASE
#undef CMP
#undef heap_sift_down
#undef heapsort

#else /* BENCHMARK */

#undef BENCHMARK

#define CASE 0
#define heap_sift_down heap_sift_down_0
#define heapsort heapsort_0
#include "heapsort.c"

#define CASE 1
#define heap_sift_down heap_sift_down_1
#define heapsort heapsort_1
#include "heapsort.c"

#define CASE 2
#define heap_sift_down heap_sift_down_2
#define heapsort heapsort_2
#include "heapsort.c"

#define CASE 3
#define heap_sift_down heap_sift_down_3
#define heapsort heapsort_3
#include "heapsort.c"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void fill(TYPE *const array, const size_t num) {
  for (size_t i = 0; i < num; ++i) {
    unsigned TYPE v = (TYPE)mrand48();
    array[i] = v >> 1;
  }
}

static int is_sorted(TYPE *const array, const size_t num) {
  for (size_t i = 1; i < num; ++i)
    if (array[i - 1] > array[i])
      return 0;
  return 1;
}

static double now(void) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts)) {
    perror("clock_gettime(CLOCK_MONOTONIC)");
    abort();
  }
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

#define LARGE_SIZE 777777
#define LARGE_NN 25
#define SMALL_SIZE 777
#define SMALL_NN 25000

TYPE array[LARGE_SIZE];

static double pass(void (*sort)(TYPE *const array, const size_t num),
                   const char *caption, size_t nn, size_t size, int seed,
                   const double baseline) {
  srand48(seed);
  double sum = 0;
  for (size_t i = 0; i < nn; ++i) {
    fill(array, size);
    const double start_timestamp = now();
    sort(array, size);
    const double finish_timestamp = now();
    if (!is_sorted(array, size)) {
      fputs("sort failed\n", stderr);
      abort();
    }
    sum += finish_timestamp - start_timestamp;
  }

  if (baseline)
    printf("  %f seconds, %s, %.1f%% of baseline\n", sum, caption,
           sum * 100 / baseline);
  else
    printf("  %f seconds, %s\n", sum, caption);
  return sum;
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  printf("heapsort-bench, cc %s\b", __VERSION__);
  for (int n = 1; n <= 3; ++n) {
    printf("\npass %d, small:\n", n);
    const double small_baseline =
        pass(heapsort_0, "baseline", SMALL_NN, SMALL_SIZE, n, 0);
    pass(heapsort_1, "case-1", SMALL_NN, SMALL_SIZE, n, small_baseline);
    pass(heapsort_2, "case-2", SMALL_NN, SMALL_SIZE, n, small_baseline);
    pass(heapsort_3, "case-1+2", SMALL_NN, SMALL_SIZE, n, small_baseline);
    printf("pass %d, large:\n", n);
    const double large_baseline =
        pass(heapsort_0, "baseline", LARGE_NN, LARGE_SIZE, n, 0);
    pass(heapsort_1, "case-1", LARGE_NN, LARGE_SIZE, n, large_baseline);
    pass(heapsort_2, "case-2", LARGE_NN, LARGE_SIZE, n, large_baseline);
    pass(heapsort_3, "case-1+2", LARGE_NN, LARGE_SIZE, n, large_baseline);
  }
  return EXIT_SUCCESS;
}

#endif /* BENCHMARK */
