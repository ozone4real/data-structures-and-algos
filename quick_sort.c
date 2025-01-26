#include <stdio.h>
#include <stdlib.h>

static int counter = 0;

void swap(int *a, int *b)
{
  int temp = *a;
  *a = *b;
  *b = temp;
};

int find_pivot_hoare(int low, int high, int *arr)
{
  int mid = (low + high) / 2;
  if (arr[mid] < arr[low])
    swap(&arr[low], &arr[mid]);
  if (arr[high] < arr[low])
    swap(&arr[high], &arr[low]);
  if (arr[high] < arr[mid])
    swap(&arr[high], &arr[mid]);
  return mid;
}

int find_pivot_lomuto(int low, int high, int *arr)
{
  int mid = (low + high) / 2;
  if (arr[mid] < arr[low])
    swap(&arr[low], &arr[mid]);
  if (arr[high] < arr[low])
    swap(&arr[high], &arr[low]);
  if (arr[high] > arr[mid])
    swap(&arr[high], &arr[mid]);
  return high;
}

int lomuto_partition(int low, int high, int *arr)
{
  // int pivot = arr[high];
  int pivot = arr[find_pivot_lomuto(low, high, arr)];
  int j = low;

  for (int i = low; i < high; i++)
  {
    counter += 1;
    if (arr[i] <= pivot)
    {
      swap(&arr[j], &arr[i]);
      j++;
    }
  }
  swap(&arr[j], &arr[high]);
  return j;
}

int hoare_partition(int low, int high, int *arr)
{
  int pivot = arr[find_pivot_hoare(low, high, arr)];
  // int pivot = arr[low];
  int i = low - 1;
  int j = high + 1;

  for (;;)
  {
    do
    {
      counter += 1;
      i++;
    } while (arr[i] < pivot);

    do
    {
      counter += 1;
      j--;
    } while (arr[j] > pivot);

    if (i >= j)
      return j;

    swap(&arr[i], &arr[j]);
  }
}

int *quick_sort(int low, int high, int *arr)
{
  if (low >= high)
    return arr;

  int pivot_index = lomuto_partition(low, high, arr);
  quick_sort(low, pivot_index - 1, arr);
  quick_sort(pivot_index + 1, high, arr);

  // int pivot_index = hoare_partition(low, high, arr);
  // quick_sort(low, pivot_index, arr);
  // quick_sort(pivot_index + 1, high, arr);

  return arr;
}

int main(void)
{
  int const ARR_LENGTH = 200;
  int arr[ARR_LENGTH] = {684, 391, 838, 812, 836, 416, 471, 503, 574, 813, 782, 456, 301, 987, 308, 954, 248, 606, 9, 534, 920, 615, 686, 803, 672, 53, 644, 789, 730, 824, 305, 868, 688, 349, 611, 668, 388, 588, 646, 253, 977, 686, 774, 369, 52, 640, 132, 195, 820, 247, 378, 682, 894, 707, 949, 857, 514, 774, 745, 825, 687, 258, 355, 266, 219, 748, 457, 111, 479, 863, 118, 626, 335, 566, 569, 192, 178, 609, 834, 868, 701, 825, 925, 400, 464, 165, 63, 564, 937, 859, 740, 426, 586, 83, 16, 177, 403, 369, 774, 284, 949, 955, 420, 840, 361, 98, 733, 156, 74, 977, 977, 934, 675, 814, 983, 341, 222, 86, 904, 961, 464, 693, 972, 563, 678, 585, 153, 275, 871, 634, 288, 604, 260, 259, 740, 231, 691, 434, 215, 992, 662, 185, 637, 491, 631, 1, 214, 489, 430, 368, 960, 622, 221, 782, 431, 314, 694, 811, 642, 76, 381, 356, 810, 474, 297, 545, 218, 542, 230, 209, 415, 465, 432, 589, 946, 298, 582, 651, 200, 964, 336, 147, 521, 529, 273, 597, 790, 22, 88, 479, 324, 183, 193, 964, 555, 828, 531, 903, 839, 882};
  int *result = quick_sort(0, ARR_LENGTH - 1, arr);
  printf("There were %i calls\n", counter);
  for (int i = 0; i < ARR_LENGTH; i++)
  {
    printf("%i\n", result[i]);
  }
};
