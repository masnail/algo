#include <iostream>
#include <vector>
#include <stdio.h>
using namespace std;

string str[] = {"","abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
void core(vector<string> & res, vector<string> &des, int i, int j, string s) {
    for (int n = i; n < des.size(); ++n) {
        for (int m = 0; m < des[i].size(); ++m) {
            core(res, des, n+1, m, s+des[n][m]);
        }
    }
    if (s.size() >= des.size()) {
        res.push_back(s);
    }
}
vector<string> letterCombinations(string digits) {
    vector<string> res;
    vector<string> tmp;
    for (int i = 0; i < digits.size(); ++i) {
        tmp.push_back(str[digits[i]-'1']);
    }
    core(res, tmp, 0, 0, "");

    for (auto &item : res) {
        cout << item << endl;
    }
    cout << endl;
    return res;

}
int main() {
    letterCombinations("2");
    return 0;
}