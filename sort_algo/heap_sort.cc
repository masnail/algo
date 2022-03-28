#include <iostream>
#include <stdio.h>

//  堆排序
void Heap(int vec[], int start, int end) {
    int head = start;
    int son = head * 2 + 1;
    while(son <= end) {
        if (son + 1 <= end && vec[son] < vec[son+1]) son += 1;
        if (vec[son] < vec[head]) break;
        else {
            std::swap(vec[head], vec[son]);
            head = son;
            son = head * 2 +1;
            // std::cout << head << ":" << son << ";" << start << ":" << end << std::endl;
        }
        // std::cout << head << ":" << son << ";" << start << ":" << end << std::endl;
    }
}

void HeapSort(int vec[], int start, int end) {
    int len = end - start + 1;
    for (int i = len / 2 - 1; i >= 0; --i) {
        Heap(vec, i, end);
    }
    for (int i = len - 1; i >= 0; --i) {
        std::swap(vec[0], vec[i]);
        Heap(vec, 0, i - 1);
    }
}

int main() {
    int vec[] = {2, 43, 54, 1, 8, 5, 0, 2, 32, 78, 10};
    int vec_len = 10;
    HeapSort(vec, 0, vec_len-1);
    for (int i = 0; i <= vec_len; ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;
    return 0;
}