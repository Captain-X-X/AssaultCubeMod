#ifndef ASPENMATHSFUNC_H
#define ASPENMATHSFUNC_H

namespace Maths
{
#ifdef min
#undef min
#endif
    template<class T>
    static inline T min(T a, T b)
    {
        return a < b ? a : b;
    }

#ifdef max
#undef max
#endif
    template<class T>
    static inline T max(T a, T b)
    {
        return a > b ? a : b;
    }

    // Fast Inverse Sqrt
    // http://www.gamedev.net/community/forums/topic.asp?topic_id=139956
    // http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
    // http://www.mceniry.net/papers/Fast%20Inverse%20Square%20Root.pdf
    // http://en.wikipedia.org/wiki/Fast_inverse_square_root
    float ufInvSqrt (float x); // about 3.5% of error
    float fInvSqrt (float x); // about 0.1% of error
    float fSqrt (float x);
    float ufSqrt (float x);
    float fACos( float x );
    float ufS2C(float x);

    template <typename T> 
    static inline T pow2(T x) { 
        return (x * x); 
    }

    float det2x2(
        float a, float b, float c, float d
    );
    float det3x3(
        float a1, float a2, float a3,
        float b1, float b2, float b3,
        float c1, float c2, float c3
    );
}

#define clamp(a,b,c) (Maths::max(b, Maths::min(a, c)))

#define PI  (3.1415927f)
#define PI2 (2*PI)
#define SQRT2 (1.4142136f)
#define SQRT3 (1.7320508f)
#define RAD (PI / 180.0f)

#endif