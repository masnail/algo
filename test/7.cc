#include <iostream>
using namespace std;

int revertnum(int num) {
    int res = 0;
    while (num != 0)
    {
        int r = num % 10;
        num /= 10;
        res *= 10;
        res += r;
    }
    cout << res << endl;
    return res;
    
}

int main() {
    revertnum(345678909);
    return 0;
}