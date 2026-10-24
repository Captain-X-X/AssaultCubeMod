// #include "pch.h"
#include "maths_func.h"

namespace Maths
{

#define UFINVSQRT(x)  union { int d; float f; } u; u.f = x; u.d = 0x5f3759df - (u.d >> 1)
    float ufInvSqrt (float x) { UFINVSQRT(x); return u.f; } // about 3.5% of error
    float fInvSqrt (float x) { UFINVSQRT(x); return 0.5f * u.f * ( 3.00175f - x * u.f * u.f ); } // about 0.1% of error
    float fSqrt (float x) { return x * fInvSqrt(x); }
    float ufSqrt (float x) { return x * ufInvSqrt(x); }
    float fACos( float x )
    {
        int s = 1;
        float y, r = 0;
        if ( x < 0 )
        {
            s = -1;
            r = 2.0f;
            y = 1.0f + x;
        }
        else y = 1.0f - x;
        UFINVSQRT(y);
        u.f = 0.5f * u.f * ( 3.0f - y * u.f * u.f );
        u.f = y * 0.5f * u.f * ( 3.0f - y * u.f * u.f );
        return 1.57079632f * ( r + s * u.f * ( 0.9003163f + y * ( 0.07782684f + y * ( 0.006777598f + y * 0.015079262f ) ) ) );
    }
#undef UFINVSQRT

    float ufS2C(float x) 
    { 
        x *= x; 
        return x < 1.0f ? ufSqrt(1.0f - x) : 0.0f; 
    }

    float det2x2(float a, float b, float c, float d) 
    { 
        return a*d - b*c; 
    }
    
    float det3x3(float a1, float a2, float a3,
        float b1, float b2, float b3,
        float c1, float c2, float c3)
    {
        return a1 * det2x2(b2, b3, c2, c3)
            - b1 * det2x2(a2, a3, c2, c3)
            + c1 * det2x2(a2, a3, b2, b3);
    }
}
