#include "base85.h"

base85::base85() {
    for (char c = 38; c <= 122; ++c)
        baseArr.push_back(c);
}

string base85::toBase85(unsigned int n, int maxLen) {
    string ret = "";
    while (n > 0) {
        ret += baseArr[n % 85];
        n /= 85;
    }
    ret += string(maxLen - ret.length(), 38);
    reverse(ret.begin(), ret.end());
    return ret;
}

unsigned int base85::fromBase85(string s) {
    unsigned int ret = 0;
    for (size_t i = 0; i < s.length(); ++i) {
        ret *= 85;
        ret += s[i] - 38;
    }
    return ret;
}
