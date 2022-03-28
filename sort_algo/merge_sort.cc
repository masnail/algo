#include <iostream>
#include <stdio.h>

//  归并排序
void Merge(int vec[], int start, int mid, int end) {
    int  i = start, j = mid+1, k =0;
    int *arr = new int[end-start+1];
    while (i <= mid && j <= end) {
        if (vec[i] > vec[j]) arr[k++] = vec[j++];
        else arr[k++] = vec[i++];
    }
    while(i <= mid) arr[k++] = vec[i++];
    while(j <= end) arr[k++] = vec[j++];
    for (int n = start, k = 0; n <= end; ++n, ++k) {
        vec[n] = arr[k];
    }
    delete []arr;
}

void MergeSort(int vec[], int start, int end) {
    if (start >= end) return;
    int mid = (start + end) / 2;
    MergeSort(vec, start, mid);
    MergeSort(vec, mid+1, end);
    Merge(vec, start, mid, end);
}

int main() {
    int vec[] = {2, 43, 54, 1, 8, 5, 0, 2, 32, 78, 10};
    MergeSort(vec, 0, 10);
    for (int i = 0; i <= 10; ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;
    return 0;
}