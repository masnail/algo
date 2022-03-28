#include <iostream>
#include <stdio.h>

//  插入排序

void InsertSort(int vec[], int start, int end) {
    for(int i = 0; i <= end; ++i){
        for ( int j = i-1; j >= 0 ; --j) {
            if (vec[j+1] <= vec[j]) {
                std::swap(vec[j+1], vec[j]);
            }
        }
    }
}

int main() {
    int vec[] = {2, 43, 54, 1, 8, 5, 0, 2, 32, 78, 10};
    InsertSort(vec, 0, 10);
    for (int i = 0; i <= 10; ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;
    return 0;
}