#include <iostream>
#include <vector>
#include <queue>

using namespace std;

class Solution {
 public:
    vector<vector<int>> KSmallestPairs(vector<int> &nums1, vector<int> &nums2, int k) {
        vector<vector<int>> res;
        auto cmp =[&nums1, &nums2](const pair<int,int>& f, const pair<int,int>& s){
            return nums1[f.first] + nums2[f.second] > nums1[s.first] + nums2[s.second];
        };
        priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> p(cmp);

        int n = nums1.size();
        int m = nums2.size();
        for (int i = 0; i < min(k,n); ++i) {
            p.emplace(i, 0);
        }

        while (k-- > 0 && !p.empty()) {
            auto item = p.top();
            p.pop();
            res.emplace_back(initializer_list<int>{nums1[item.first], nums2[item.second]});
            if (item.second + 1 < m) {
                p.emplace(item.first, item.second+1);
            }
        }

        
        for (auto &item : res) {
            cout << endl;
            for(auto &t : item) {
                cout << t << ", ";
            }
        }
        cout << endl;
        return res;
    }
};

int main() {
    // int arr1[] = {1, 1, 2};
    int arr1[] = {1, 7, 11};
    vector<int> nums1(arr1, arr1+3);
    // int arr2[] = {1, 2, 3};
    int arr2[] = {2, 4, 6};
    vector<int> nums2(arr2, arr2+3);
    Solution s;
    s.KSmallestPairs(nums1, nums2, 10);
    return 0;
}