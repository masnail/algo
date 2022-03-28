#include <iostream>
#include <vector>

using namespace std;

int jump(vector<int>& nums) {
    for (auto &item : nums) {
        cout << item << ",";
    }
    cout << endl;

    int res = 0;
    int end = 0;
    int maxpos = 0;
    for (int i = 0; i < nums.size()-1; ++i) {
        //  在范围内累计最大值，下次边界就是该数值
        //  累计值: 当前位置和位置所在值的和
        maxpos = max(maxpos, i+nums[i]);
        if (i == end) {
            res++;
            end = maxpos;
        }
    }
    return res;
}

int main() {
    int arr[] = {1, 1, 1, 1, 4, 5, 6};
    vector<int> nums(arr, arr+4);
    int min = jump(nums);
    cout << "min:" << min << endl;
    return 0;
}