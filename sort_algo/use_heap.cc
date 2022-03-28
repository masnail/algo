#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

void PrintHeap(vector<int>& vec) {
    cout << ">>>:";
    for(auto &item : vec) {
        cout << item << ",";
    }
    cout << endl;
}

int main(){
    vector<int> vec = {2,5,1,6,3,0,9,2};
    PrintHeap(vec);

    make_heap(vec.begin(), vec.end(), less<int>());
    PrintHeap(vec);

    vec.push_back(4);
    push_heap(vec.begin(), vec.end(), less<int>());
    PrintHeap(vec);

    pop_heap(vec.begin(), vec.end(), less<int>());
    PrintHeap(vec);

    sort_heap(vec.begin(), vec.end(), less<int>());
    PrintHeap(vec);

    return 0;
}