#include <iostream>
#include <stdio.h>

//  选择排序

void ChooseSort(int vec[], int start, int end) {
    for(int i = 0; i <= end; ++i){
        int tmp = i;
        for (int j = i; j <= end ; ++j) {
            if (vec[tmp] > vec[j]) {
                tmp = j;
            }
        }
        std::swap(vec[i], vec[tmp]);
    }
}

int main() {
    int vec[] = {2, 43, 54, 1, 8, 5, 0, 2, 32, 78, 10};
    ChooseSort(vec, 0, 10);
    for (int i = 0; i <= 10; ++i) {
        std::cout << vec[i] << ", ";
    }
    std::cout << std::endl;

    int a = 10;
    int b = 9;
    std::cout << a << ":" << b << std::endl;
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;
    std::cout << a << ":" << b << std::endl;
    return 0;
}