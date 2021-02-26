#ifndef STDFUNCS_H
#define STDFUNCS_H

#include <iostream>
#include <time.h>
#include <chrono>
using std::cout;
using std::endl;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;


class stdFuncs
{
public:

    template<typename T1, typename T2, typename T3>
    inline static T1 clamp(T1 value, T2 min, T3 max) {
        return value > static_cast<T1>(max) ? static_cast<T1>(max) : (value < static_cast<T1>(min) ? static_cast<T1>(min) : value);
    }

    static long long getTime() {
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }

    static long long getChange(long long initial) {
        return getTime() - initial;
    }

};

#endif // STDFUNCS_H

