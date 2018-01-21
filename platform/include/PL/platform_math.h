/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/
#pragma once

#include "platform.h"

// Base Defines

#define PL_PI           3.14159265358979323846264338327950288f
#define PL_PI_DIV_180   PL_PI / 180.f

#define PL_TAU      6.28318530717958647692528676655900576f
#define PL_EPSILON  1.19209290e-7f

enum {
    // Colours
    PL_RED = 0,
    PL_GREEN,
    PL_BLUE,
    PL_ALPHA
};

#define plFloatToByte(a)    (uint8_t)round((a) * 255)
#define plByteToFloat(a)    ((a) / (float)255)

PL_INLINE static bool plIsPowerOfTwo(unsigned int num) {
    return (bool)((num != 0) && ((num & (~num + 1)) == num));
}

PL_INLINE static float plDegreesToRadians(float degrees) {
    return degrees * (PL_PI / 180);
}

PL_INLINE static float plRadiansToDegrees(float radians) {
    return radians * (180 / PL_PI);
}

/////////////////////////////////////////////////////////////////////////////////////
// Vectors

// 2D

typedef struct PLVector2 {
    float x, y;

#ifdef __cplusplus
    PLVector2(float a, float b) : x(a), y(b) {}
    PLVector2() : x(0), y(0) {}

    PLVector2 &operator=(float a) { x = a; y = a; return *this; }

    void operator*=(PLVector2 a) { x *= a.x; y *= a.y; }
    void operator*=(float a) { x *= a; y *= a; }

    void operator/=(PLVector2 a) { x /= a.x; y /= a.y; }
    void operator/=(float a) { x /= a; y /= a; }

    void operator+=(PLVector2 a) { x += a.x; y += a.y; }
    void operator+=(float a) { x += a; y += a; }

    bool operator==(PLVector2 a) const { return ((x == a.x) && (y == a.y)); }
    bool operator==(float a) const { return ((x == a) && (y == a)); }

    PLVector2 operator*(PLVector2 a) const { return PLVector2(x * a.x, y * a.y); }
    PLVector2 operator*(float a) const { return PLVector2(x * a, y * a); }

    PLVector2 operator/(PLVector2 a) const { return PLVector2(x / a.x, y / a.y); }
    PLVector2 operator/(float a) const { return PLVector2(x / a, y / a); }

    PLVector2 operator+(PLVector2 a) const { return PLVector2(x + a.x, y + a.y); }
    PLVector2 operator+(float a) const { return PLVector2(x + a, y + a); }

    PLVector2 operator-(PLVector2 a) const { return PLVector2(x - a.x, y - a.y); }
    PLVector2 operator-(float a) const { return PLVector2(x - a, y - a); }

    float Length() { return std::sqrt(x * x + y * y); }

    PLVector2 Normalize() {
        PLVector2 out;
        float length = Length();
        if (length != 0) {
            out.Set(x / length, y / length);
        }
        return out;
    }

    void Set(float a, float b) { x = a; y = b; }
    void Clear() { x = 0; y = 0; }
#endif
} PLVector2;

PL_INLINE static void plClearVector2(PLVector2 *v) {
    memset(v, 0, sizeof(PLVector2));
}

PL_INLINE static void plAddVector2(PLVector2 *v, PLVector2 v2) {
    v->x = v2.x; v->y = v2.y;
}

PL_INLINE static void plDivideVector2(PLVector2 *v, PLVector2 v2) {
    v->x /= v2.x; v->y /= v2.y;
}

PL_INLINE static bool plCompareVector2(PLVector2 v, PLVector2 v2) {
    return ((v.x == v2.x) && (v.y == v2.y));
}

// 3D

typedef struct PLVector3 {
    float x, y, z;

#ifdef __cplusplus
    PL_INLINE PLVector3() : x(0), y(0), z(0) {}
    PL_INLINE PLVector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    PL_INLINE explicit PLVector3(const float *v) {
        x = v[0]; y = v[1]; z = v[2];
    }

    PL_INLINE PLVector3 &operator = (PLVector2 v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    PL_INLINE PLVector3 &operator = (float a) {
        x = a;
        y = a;
        z = a;
        return *this;
    }

    PL_INLINE void operator *= (const PLVector3 &v) {
        x *= v.x; y *= v.y; z *= v.z;
    }

    PL_INLINE void operator *= (float a) {
        x *= a; y *= a; z *= a;
    }

    PL_INLINE void operator += (PLVector3 a) {
        x += a.x; y += a.y; z += a.z;
    }

    PL_INLINE void operator += (float a) {
        x += a; y += a; z += a;
    }

    PL_INLINE bool operator == (const PLVector3 &a) const {
        return ((x == a.x) && (y == a.y) && (z == a.z));
    }

    PL_INLINE bool operator == (float f) const {
        return ((x == f) && (y == f) && (z == f));
    }

    PL_INLINE bool operator != (const PLVector3 &a) const {
        return !(a == *this);
    }

    PL_INLINE bool operator != (float f) const {
        return ((x != f) && (y != f) && (z != f));
    }

    PL_INLINE PLVector3 operator * (PLVector3 a) const {
        return {x * a.x, y * a.y, z * a.z};
    }

    PL_INLINE PLVector3 operator * (float a) const {
        return {x * a, y * a, z * a};
    }

    PL_INLINE PLVector3 operator - (PLVector3 a) const {
        return {x - a.x, y - a.y, z - a.z};
    }

    PL_INLINE PLVector3 operator - (float a) const {
        return {x - a, y - a, z - a};
    }

    PLVector3 PL_INLINE operator - () const {
        return {-x, -y, -z};
    }

    PLVector3 PL_INLINE operator + (PLVector3 a) const {
        return {x + a.x, y + a.y, z + a.z};
    }

    PLVector3 PL_INLINE operator + (float a) const {
        return {x + a, y + a, z + a};
    }

    PLVector3 PL_INLINE operator / (const PLVector3 &a) const {
        return {x / a.x, y / a.y, z / a.z};
    }

    PLVector3 PL_INLINE operator / (float a) const {
        return {x / a, y / a, z / a};
    }

    PL_INLINE float& operator [] (const unsigned int i) {
        return *((&x) + i);
    }

    PL_INLINE bool operator > (const PLVector3 &v) const {
        return ((x > v.x) && (y > v.y) && (z > v.z));
    }

    PL_INLINE bool operator < (const PLVector3 &v) const {
        return ((x < v.x) && (y < v.y) && (z < v.z));
    }

    PL_INLINE bool operator >= (const PLVector3 &v) const {
        return ((x >= v.x) && (y >= v.y) && (z >= v.z));
    }

    PL_INLINE bool operator <= (const PLVector3 &v) const {
        return ((x <= v.x) && (y <= v.y) && (z <= v.z));
    }

    PL_INLINE float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    PL_INLINE float DotProduct(PLVector3 a) const {
        return (x * a.x + y * a.y + z * a.z);
    }

    PL_INLINE PLVector3 CrossProduct(PLVector3 a) const {
        return {y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x};
    }

    PL_INLINE PLVector3 Normalize() const {
        return (*this) / Length();
    }

    PL_INLINE float Difference(PLVector3 v) const {
        return ((*this) - v).Length();
    }

    void PL_INLINE Set(float _x, float _y, float _z) {
        x = _x; y = _y; z = _z;
    }
#endif
} PLVector3;

PL_INLINE static void plAddVector3(PLVector3 *v, PLVector3 v2) {
    v->x += v2.x; v->y += v2.y; v->z += v2.z;
}

PL_INLINE static void plSubtractVector3(PLVector3 *v, PLVector3 v2) {
    v->x -= v2.x; v->y -= v2.y; v->z -= v2.z;
}

PL_INLINE static void plScaleVector3(PLVector3 *v, PLVector3 v2) {
    v->x *= v2.x; v->y *= v2.y; v->z *= v2.z;
}

PL_INLINE static void plScaleVector3f(PLVector3 *v, float f) {
    v->x *= f; v->y *= f; v->z *= f;
}

PL_INLINE static void plDivideVector3(PLVector3 *v, PLVector3 v2) {
    v->x /= v2.x; v->y /= v2.y; v->z /= v2.z;
}

PL_INLINE static void plDivideVector3f(PLVector3 *v, float v2) {
    v->x /= v2;
    v->y /= v2;
    v->z /= v2;
}

PL_INLINE static void plInverseVector3(PLVector3 *v) {
    v->x = -v->x; v->y = -v->y; v->z = -v->z;
}

PL_INLINE static void plClearVector3(PLVector3 *v) {
    memset(v, 0, sizeof(PLVector3));
}

PL_INLINE static bool plCompareVector3(PLVector3 v, PLVector3 v2) {
    return ((v.x == v2.x) && (v.y == v2.y) && (v.z == v2.z));
}

PL_INLINE static PLVector3 plVector3CrossProduct(PLVector3 v, PLVector3 v2) {
    return (PLVector3){
            v.y * v2.z - v.z * v2.y,
            v.z * v2.x - v.x * v2.z,
            v.x * v2.y - v.y * v2.x
    };
}

PL_INLINE static PLVector3 plVector3Max(PLVector3 v, PLVector3 v2) {
    return (PLVector3) {
            v.x > v2.x ? v.x : v2.x,
            v.y > v2.y ? v.y : v2.y,
            v.z > v2.z ? v.z : v2.z
    };
}

PL_INLINE static PLVector3 plVector3Min(PLVector3 v, PLVector3 v2) {
    return (PLVector3) {
            v.x < v2.x ? v.x : v2.x,
            v.y < v2.y ? v.y : v2.y,
            v.z < v2.z ? v.z : v2.z
    };
}

PL_INLINE static float plVector3DotProduct(PLVector3 v, PLVector3 v2) {
    return (v.x * v2.x + v.y * v2.y + v.z * v2.z);
}

PL_INLINE static float plVector3Length(PLVector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

PL_INLINE static PLVector3 plNormalizeVector3(PLVector3 v) {
    float length = plVector3Length(v);
    if(length != 0) {
        v.x /= length; v.y /= length; v.z /= length;
    }
    return v;
}

PL_INLINE static const char *plPrintVector3(PLVector3 v) {
    static char s[32] = { 0 };
    snprintf(s, 32, "%i %i %i", (int)v.x, (int)v.y, (int)v.z);
    return s;
}

// 4D

typedef struct PLVector4 {
    float x, y, z, w;
} PLVector4;

#ifndef __cplusplus

#   define PLVector3(x, y, z)   (PLVector3){x, y, z}
#   define PLVector2(x, y)      (PLVector2){x, y}

#endif

/////////////////////////////////////////////////////////////////////////////////////
// Colour

#define PL_COLOUR_WHITE 255,    255,    255,    255
#define PL_COLOUR_BLACK 0,      0,      0,      255
#define PL_COLOUR_RED   255,    0,      0,      255
#define PL_COLOUR_GREEN 0,      255,    0,      255
#define PL_COLOUR_BLUE  0,      0,      255,    255

typedef struct PLColour {
    uint8_t r, g, b, a;

#ifdef __cplusplus
    PLColour() : PLColour(PL_COLOUR_WHITE) {

    }

    PLColour(uint8_t c, uint8_t c2, uint8_t c3, uint8_t c4 = 255) : r(c), g(c2), b(c3), a(c4) {

    }

    PLColour(int c, int c2, int c3, int c4 = 255) :
            PLColour((uint8_t) c, (uint8_t) c2, (uint8_t) c3, (uint8_t) c4) {

    }

    PLColour(float c, float c2, float c3, float c4 = 1) :
    r(plFloatToByte(c)),
    g(plFloatToByte(c2)),
    b(plFloatToByte(c3)),
    a(plFloatToByte(c4)) {

    }

    PL_INLINE void operator *= (const PLColour &v) {
        r *= v.r; g *= v.g; b *= v.b; a *= v.a;
    }

    PL_INLINE void operator *= (float c) {
        r *= plFloatToByte(c); g *= plFloatToByte(c); b *= plFloatToByte(c); a *= plFloatToByte(c);
    }

    PL_INLINE void operator *= (uint8_t c) {
        r *= c; g *= c; g *= c; a *= c;
    }

    PL_INLINE void operator += (PLColour v) {
        r += v.r; g += v.g; b += v.b; a += v.a;
    }

    PL_INLINE void operator += (float c) {
        r += plFloatToByte(c); g += plFloatToByte(c); b += plFloatToByte(c); a += plFloatToByte(c);
    }

    PL_INLINE void operator /= (PLColour v) {
        r /= v.r; g /= v.g; b /= v.b; a /= v.a;
    }

    PL_INLINE void operator /= (float c) {
        r /= c; g /= c; b /= c; a /= c;
    }

    PL_INLINE PLColour operator - (PLColour c) const {
        return {r - c.r, g - c.g, b - c.b, a - c.a};
    }

    PL_INLINE PLColour operator - (float c) const {
        return {r - plFloatToByte(c), g - plFloatToByte(c), b - plFloatToByte(c), a - plFloatToByte(c)};
    }

    PL_INLINE PLColour operator - (uint8_t c) const {
        return {r - c, g - c, b - c, a - c};
    }

    PL_INLINE PLColour operator - () const {
        return PLColour(-r, -g, -b, -a);
    }

    PL_INLINE PLColour operator * (PLColour v) const {
        return PLColour(r * v.r, g * v.g, b * v.b, a * v.a);
    }

    PL_INLINE PLColour operator + (PLColour v) const {
        return PLColour(r + v.r, g + v.g, b + v.b, a + v.a);
    }

    PL_INLINE PLColour operator + (float c) const {
        return {r + plFloatToByte(c), g + plFloatToByte(c), b + plFloatToByte(c), a + plFloatToByte(c)};
    }

    PL_INLINE PLColour operator / (const PLColour &v) const {
        return {r / v.r, g / v.g, b / v.b, a / v.a};
    }

    PL_INLINE PLColour operator / (float c) const {
        return PLColour(r / plFloatToByte(c), g / plFloatToByte(c), b / plFloatToByte(c), a / plFloatToByte(c));
    }

    PL_INLINE PLColour operator / (uint8_t c) const {
        return PLColour(r / c, g / c, b / c, a / c);
    }

    PL_INLINE uint8_t& operator [] (const unsigned int i) {
        return *((&r) + i);
    }

    PL_INLINE bool operator > (const PLColour &v) const {
        return ((r > v.r) && (g > v.g) && (b > v.b) && (a > v.a));
    }

    PL_INLINE bool operator < (const PLColour &v) const {
        return ((r < v.r) && (g < v.g) && (b < v.b) && (a < v.a));
    }

    PL_INLINE bool operator >= (const PLColour &v) const {
        return ((r >= v.r) && (g >= v.g) && (b >= v.b) && (a >= v.a));
    }

    PL_INLINE bool operator <= (const PLColour &v) const {
        return ((r <= v.r) && (g <= v.g) && (b <= v.b) && (a <= v.a));
    }
#endif
} PLColour;

PL_INLINE static PLColour plCreateColour4b(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    PLColour c = { r, g, b, a };
    return c;
}

PL_INLINE static PLColour plCreateColour4f(float r, float g, float b, float a) {
    PLColour c = {
        plFloatToByte(r),
        plFloatToByte(g),
        plFloatToByte(b),
        plFloatToByte(a)
    };
    return c;
}

PL_INLINE static void plClearColour(PLColour *c) {
    memset(c, 0, sizeof(PLColour));
}

PL_INLINE static void plSetColour4b(PLColour *c, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    c->r = r; c->g = g; c->b = b; c->a = a;
}

PL_INLINE static void plSetColour4f(PLColour *c, float r, float g, float b, float a) {
    c->r = plFloatToByte(r);
    c->g = plFloatToByte(g);
    c->b = plFloatToByte(b);
    c->a = plFloatToByte(a);
}

PL_INLINE static bool plCompareColour(PLColour c, PLColour c2) {
    return ((c.r == c2.r) && (c.g == c2.g) && (c.b == c2.b) && (c.a == c2.a));
}

PL_INLINE static void plCopyColour(PLColour *c, PLColour c2) {
    c->r = c2.r; c->g = c2.g; c->b = c2.b; c->a = c2.a;
}

PL_INLINE static void plMultiplyColour(PLColour *c, PLColour c2) {
    c->r *= c2.r; c->g *= c2.g; c->b *= c2.b; c->a *= c2.a;
}

PL_INLINE static void plMultiplyColourf(PLColour *c, float a) {
    uint8_t a2 = plFloatToByte(a);
    c->r *= a2; c->g *= a2; c->b *= a2; c->a *= a2;
}

PL_INLINE static void plDivideColour(PLColour *c, PLColour c2) {
    c->r /= c2.r; c->g /= c2.g; c->b /= c2.b; c->a /= c2.a;
}

PL_INLINE static void plDivideColourf(PLColour *c, float a) {
    uint8_t a2 = plFloatToByte(a);
    c->r /= a2; c->g /= a2; c->b /= a2; c->a /= a2;
}

PL_INLINE static const char *plPrintColour(PLColour c) {
    static char s[16] = { '\0' };
    snprintf(s, 16, "%i %i %i %i", c.r, c.g, c.b, c.a);
    return s;
}

#ifndef __cplusplus

#   define PLColour(r, g, b, a) (PLColour){r, g, b, a}
#   define PLColourRGB(r, g, b) (PLColour){r, g, b, 255}
#   define PLColourR(r)         (PLColour){r, 255, 255, 255}
#   define PLColourG(g)         (PLColour){255, g, 255, 255}
#   define PLColourB(b)         (PLColour){255, 255, b, 255}
#   define PLColourA(a)         (PLColour){255, 255, 255, a}

#endif

/////////////////////////////////////////////////////////////////////////////////////
// Matrices
// todo, none of this is correct yet

typedef struct PLMatrix3x3 {
    float m[9];
    /* 0 0 0
     * 0 0 0
     * 0 0 0
     */
} PLMatrix3x3;

typedef struct PLMatrix3x4 {
    float m[12];
    /* 0 0 0 0
     * 0 0 0 0
     * 0 0 0 0
     */
} PLMatrix3x4;

typedef struct PLMatrix4x4 {
    float m[16];
    /* 0 0 0 0
     * 0 0 0 0
     * 0 0 0 0
     * 0 0 0 0
     */
} PLMatrix4x4;

#define plSetMatrix4x4(mat, col, row, val) (mat).m[(col) * 4 + (row)] = (val)

PL_INLINE static void plClearMatrix(PLMatrix4x4 *m) {
    memset(m, 0, sizeof(PLMatrix4x4));
}

PL_INLINE static void plIdentityMatrix(PLMatrix4x4 *m) {
    *m = (PLMatrix4x4){{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }};
}

PL_INLINE static void plAddMatrix(PLMatrix4x4 m, const PLMatrix4x4 m2) {

}

PL_INLINE static void plScaleMatrix(PLMatrix4x4 *m, PLVector3 scale) {

}

PL_INLINE static void plRotateMatrix(PLMatrix4x4 *m, float angle, PLVector3 axis) {

}

// Quaternion

typedef struct PLQuaternion {
    float x, y, z, w;

#ifdef __cplusplus
    PLQuaternion(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {}
    PLQuaternion(float a, float b, float c) : x(a), y(b), z(c), w(0) {}
    PLQuaternion() : x(0), y(0), z(0), w(0) {}

    PL_INLINE void operator *= (float a) {
        x *= a; y *= a; z *= a; w *= a;
    }

    PL_INLINE void operator *= (PLQuaternion a) {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        w *= a.w;
    }

    PL_INLINE bool operator == (PLQuaternion a) const {
        return ((x == a.x) && (y == a.y) && (z == a.z) && (w == a.w));
    }

    PL_INLINE PLQuaternion operator * (float a) {
        return {x * a, y * a, z * a, w * a};
    }

    PL_INLINE PLQuaternion operator * (PLQuaternion a) {
        return {x * a.x, y * a.y, z * a.z, w * a.w};
    }

    PL_INLINE void Set(float a, float b, float c, float d) {
        x = a; y = b; z = c; w = d;
    }

    PL_INLINE void Set(float a, float b, float c) {
        x = a;
        y = b;
        z = c;
    }

    PL_INLINE const char *String() {
        static char s[32] = { 0 };
        snprintf(s, 32, "%i %i %i %i", (int) x, (int) y, (int) z, (int) w);
        return s;
    }

    PL_INLINE float Length() {
        return std::sqrt((x * x + y * y + z * z + w * w));
    }

    PL_INLINE PLQuaternion Scale(float a) {
        return {x * a, y * a, z * a, w * a};
    }

    PL_INLINE PLQuaternion Inverse() {
        return {-x, -y, -z, w};
    }

#if 0
    PL_INLINE PLQuaternion Normalize() {
        float l = Length();
        if (l > 0) {
            float i = 1 / l;
            return Scale(i);
        }
    }
#endif
#endif
} PLQuaternion;

PL_INLINE static void plClearQuaternion(PLQuaternion *q) {
    memset(q, 0, sizeof(PLQuaternion));
}

PL_INLINE static void plMultiplyQuaternion(PLQuaternion *q, PLQuaternion q2) {
    q->x *= q2.x;
    q->y *= q2.y;
    q->z *= q2.z;
    q->w *= q2.w;
}

PL_INLINE static void plScaleQuaternion(PLQuaternion *q, float a) {
    q->x *= a;
    q->y *= a;
    q->z *= a;
    q->w *= a;
}

PL_INLINE static void plAddQuaternion(PLQuaternion *q, PLQuaternion q2) {
    q->x += q2.x;
    q->y += q2.y;
    q->z += q2.z;
    q->w += q2.w;
}

PL_INLINE static void plAddQuaternionf(PLQuaternion *q, float a) {
    q->x += a;
    q->y += a;
    q->z += a;
    q->w += a;
}

PL_INLINE static void plInverseQuaternion(PLQuaternion *q) {
    q->x = -q->x;
    q->y = -q->y;
    q->z = -q->z;
    q->w = -q->w;
}

PL_INLINE static void plQuaternionFromMatrix(PLQuaternion *q, PLMatrix4x4 mat) {
    // todo
}

PL_INLINE static float plQuaternionLength(const PLQuaternion *q) {
    return sqrtf(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
}

PL_INLINE static void plNormalizeQuaternion(PLQuaternion *q) {
    float l = plQuaternionLength(q);
    if(l > 0) {
        float i = 1 / l;
        plScaleQuaternion(q, i);
    }
}

PL_INLINE static const char *plPrintQuaternion(PLQuaternion q) {
    static char s[32] = { '\0' };
    snprintf(s, 32, "%i %i %i %i", (int)q.x, (int)q.y, (int)q.z, (int)q.w);
    return s;
}

/////////////////////////////////////////////////////////////////////////////////////
// Primitives

// Cube

// Sphere

typedef struct PLSphere {
    PLVector3 position;
    float radius;

    PLColour colour;
} PLSphere;

// Line

typedef struct PLLine3D {
    PLVector3 a, b;
    unsigned int width;

    PLColour a_colour, b_colour;
} PLLine3D;

PL_INLINE static void plClearLine(PLLine3D *l) {
    memset(l, 0, sizeof(PLLine3D));
}

PL_INLINE static void plSetLineColour(PLLine3D *line, PLColour a, PLColour b) {
    line->a_colour = a; line->b_colour = b;
}

// Rectangle 2D

typedef struct PLRectangle2D {
    PLVector2 xy;
    PLVector2 wh;

    PLColour ul, ur, ll, lr;
} PLRectangle2D;

PL_INLINE static PLRectangle2D plCreateRectangle(
        PLVector2 xy, PLVector2 wh,
        PLColour ul, PLColour ur,
        PLColour ll, PLColour lr
) {
    return (PLRectangle2D){
            xy, wh,
            ul, ur,
            ll, lr
    };
}

PL_INLINE static void plClearRectangle(PLRectangle2D *r) {
    memset(r, 0, sizeof(PLRectangle2D));
}

PL_INLINE static void plSetRectangleUniformColour(PLRectangle2D *r, PLColour colour) {
    r->ll = r->lr = r->ul = r->ur = colour;
}

/////////////////////////////////////////////////////////////////////////////////////
// Randomisation

// http://stackoverflow.com/questions/7978759/generate-float-random-values-also-negative
PL_INLINE static double plUniform0To1Random(void) {
    return (rand()) / ((double) RAND_MAX + 1);
}

PL_INLINE static double plGenerateUniformRandom(double minmax) {
    return (minmax * 2) * plUniform0To1Random() - minmax;
}

PL_INLINE static double plGenerateRandomd(double max) {
    return (double) (rand()) / (RAND_MAX / max);
}

PL_INLINE static float plGenerateRandomf(float max) {
    return (float) (rand()) / (RAND_MAX / max);
}

/////////////////////////////////////////////////////////////////////////////////////
// Interpolation
// http://paulbourke.net/miscellaneous/interpolation/

PL_INLINE static float plLinearInterpolate(float y1, float y2, float mu) {
    return (y1 * (1 - mu) + y2 * mu);
}

PL_INLINE static float plCosineInterpolate(float y1, float y2, float mu) {
    float mu2 = (1 - cosf(mu * (float) PL_PI)) / 2;
    return (y1 * (1 - mu2) + y2 * mu2);
}

// http://probesys.blogspot.co.uk/2011/10/useful-math-functions.html

PL_INLINE static float plOutPow(float x, float p) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    int sign = (int) p % 2 == 0 ? -1 : 1;
    return (sign * (powf(x - 1.0f, p) + sign));
}

PL_INLINE static float plLinear(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }
    return x;
}

PL_INLINE static float plInPow(float x, float p) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }
    return powf(x, p);
}

PL_INLINE static float plInSin(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }
    return -cosf(x * ((float) PL_PI / 2.0f)) + 1.0f;
}

PL_INLINE static float plOutSin(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }
    return sinf(x * ((float) PL_PI / 2.0f));
}

PL_INLINE static float plInExp(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return powf(2.0f, 10.0f * (x - 1.0f));
}

PL_INLINE static float plOutExp(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }
    return -powf(2.0f, -1.0f * x) + 1.0f;
}

PL_INLINE static float plInOutExp(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }
    return x < 0.5f ? 0.5f * powf(2.0f, 10.0f * (2.0f * x - 1.0f)) :
           0.5f * (-powf(2.0f, 10.0f * (-2.0f * x + 1.0f)) + 1.0f);
}

PL_INLINE static float plInCirc(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }
    return -(sqrtf(1.0f - x * x) - 1.0f);
}

PL_INLINE static float plOutBack(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }
    return (x - 1.0f) * (x - 1.0f) * ((1.70158f + 1.0f) * (x - 1.0f) + 1.70158f) + 1.0f;
}

// The variable, k, controls the stretching of the function.
PL_INLINE static float plImpulse(float x, float k) {
    float h = k * x;
    return h * expf(1.0f - h);
}

PL_INLINE static float plRebound(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    if (x < (1.0f / 2.75f)) {
        return 1.0f - 7.5625f * x * x;
    } else if (x < (2.0f / 2.75f)) {
        return 1.0f - (7.5625f * (x - 1.5f / 2.75f) *
                       (x - 1.5f / 2.75f) + 0.75f);
    } else if (x < (2.5f / 2.75f)) {
        return 1.0f - (7.5625f * (x - 2.25f / 2.75f) *
                       (x - 2.25f / 2.75f) + 0.9375f);
    } else {
        return 1.0f - (7.5625f * (x - 2.625f / 2.75f) * (x - 2.625f / 2.75f) +
                       0.984375f);
    }
}

PL_INLINE static float plExpPulse(float x, float k, float n) {
    return expf(-k * powf(x, n));
}

PL_INLINE static float plInOutBack(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return x < 0.5f ? 0.5f * (4.0f * x * x * ((2.5949f + 1.0f) * 2.0f * x - 2.5949f)) :
           0.5f * ((2.0f * x - 2.0f) * (2.0f * x - 2.0f) * ((2.5949f + 1.0f) * (2.0f * x - 2.0f) +
                                                            2.5949f) + 2.0f);
}

PL_INLINE static float plInBack(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return x * x * ((1.70158f + 1.0f) * x - 1.70158f);
}

PL_INLINE static float plInOutCirc(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return x < 1.0f ? -0.5f * (sqrtf(1.0f - x * x) - 1.0f) :
           0.5f * (sqrtf(1.0f - ((1.0f * x) - 2.0f) * ((2.0f * x) - 2.0f)) + 1.0f);
}

PL_INLINE static float plOutCirc(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return sqrtf(1.0f - (x - 1.0f) * (x - 1.0f));
}

PL_INLINE static float plInOutSin(float x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return -0.5f * (cosf((float) PL_PI * x) - 1.0f);
}

PL_INLINE static float plInOutPow(float x, float p) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    int sign = (int) p % 2 == 0 ? -1 : 1;
    return (sign / 2.0f * (powf(x - 2.0f, p) + sign * 2.0f));
}

//////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS

PL_INLINE static float plToRadians(float degrees) {
    return degrees * (PL_PI / 180.f);
}

#define plClamp(min, val, max) (val) < (min) ? (min) : ((val) > (max) ? (max) : (val))

//////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS

#if defined(PL_INTERNAL)

void _plDebugMath(void);

#endif