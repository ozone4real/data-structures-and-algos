#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// heapify

// sift_down

// heap_sort

void swap(int *a, int *b)
{
  int c = *a;
  *a = *b;
  *b = c;
}

static inline size_t left_child(size_t index)
{
  return (2 * index) + 1;
}

static inline size_t right_child(size_t index)
{
  return (2 * index) + 2;
}

static inline size_t parent(size_t index)
{
  return (index - 1) / 2;
}

static int counter = 0;

int *sift_down(int *arr, size_t parent, size_t arr_length)
{
  while (left_child(parent) < arr_length)
  {
    counter++;
    size_t child = left_child(parent);
    if (child + 1 < arr_length && arr[child + 1] > arr[child])
      child += 1;
    if (arr[child] > arr[parent])
      swap(arr + child, arr + parent);
    parent = child;
  }
  return arr;
}

int *heapify(int *arr, size_t arr_length)
{
  size_t last_elem_idx = arr_length - 1;
  size_t start = parent(last_elem_idx) + 1;

  while (start > 0)
  {
    start -= 1;
    size_t parent = start;
    sift_down(arr, parent, arr_length);
  }
  return arr;
}

int *heap_sort(int *arr, size_t arr_length)
{
  size_t end = arr_length;
  heapify(arr, end);
  while (end > 1)
  {
    end -= 1;
    swap(arr + end, arr);
    sift_down(arr, 0, end);
  }
  return arr;
}

int main(void)
{
  size_t const arr_length = 21;
  int arr[arr_length] = {336, 872, 760, 45, 510, 99, 608, 327, 731, 975, 76, 65, 214, 378, 660, 753, 865, 223, 614, 525, 9000};
  int *heapified_arr = heap_sort(arr, arr_length);
  for (size_t i = 0; i < arr_length; i++)
  {
    printf("%i\n", heapified_arr[i]);
  }
  printf("There were %i iterations\n", counter);
}