#ifndef STDFUNCS_H
#define STDFUNCS_H


class stdFuncs
{
public:
    template<typename T>
    inline static T min(T value, T min) {
        return value < min ? min : value;
    }

    template<typename T>
    inline static T max(T value, T max) {
        return value > max ? max : value;
    }

    template<typename T>
    inline static T clamp(T value, T min, T max) {
        return value > max ? max : (value < min ? min : value);
    }

};

#endif // STDFUNCS_H
