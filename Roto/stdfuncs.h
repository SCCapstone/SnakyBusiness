#ifndef STDFUNCS_H
#define STDFUNCS_H

#include <time.h>
#include <chrono>
using namespace std::chrono;
using std::chrono::milliseconds;

class stdFuncs
{
public:

    template<typename T>
    inline static T clamp(T value, T min, T max) {
        return value > max ? max : (value < min ? min : value);
    }

    static long long getTime() {
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }

    static long long getChange(long long initial) {
        return getTime() - initial;
    }

};

#endif // STDFUNCS_H
