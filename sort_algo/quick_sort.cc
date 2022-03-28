#include <iostream>
#include <stdio.h>

//  快速排序

int ParititionSort(int vec[], int start, int end) {
    int tmp = vec[start];
    while (start < end) {
        while (start < end && tmp <= vec[end]) --end;
        vec[start] = vec[end];
        while (start < end && tmp >= vec[start]) ++start;
        vec[end] = vec[start];
    }
    vec[start] = tmp;
    return start;
}

void QuickSort(int vec[], int start, int end) {
    if (start < end) {
        int idx = ParititionSort(vec, start, end);
        QuickSort(vec, start,idx - 1);
        QuickSort(vec, idx + 1,end);
    }
}

int main() {
    int vec[] = {2, 43, 54, 1, 8, 5, 0, 2, 32, 78, 10};
    QuickSort(vec, 0, 10);
    for (int i = 0; i <= 10; ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;
    return 0;
}