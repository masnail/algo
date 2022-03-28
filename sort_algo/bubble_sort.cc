#include <iostream>
#include <stdio.h>

//  冒泡排序

void BubbleSort(int vec[], int start, int end) {
    for(int i = 0; i <= end; ++i){
        for (int j = i; j <= end ; ++j) {
            if (vec[i] > vec[j]) {
                std::swap(vec[i], vec[j]);
            }
        }
    }
}

int main() {
    int vec[] = {2, 43, 54, 1, 8, 5, 0, 2, 32, 78, 10};
    BubbleSort(vec, 0, 10);
    for (int i = 0; i <= 10; ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;
    return 0;
}