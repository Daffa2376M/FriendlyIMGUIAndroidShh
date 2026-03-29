
#pragma once

#define _USE_MATH_DEFINES
#include <math.h>


struct Vec3
{
    float x;
    float y;
    float z;

    Vec3();
    Vec3(float x, float y, float z);
    ~Vec3();


    /**
     * Constructors.
     */


    /**
     * Constants for common vectors.
     */
    static inline Vec3 zero();
    static inline Vec3 One();
    static inline Vec3 Right();
    static inline Vec3 Left();
    static inline Vec3 Up();
    static inline Vec3 Down();
    static inline Vec3 Forward();
    static inline Vec3 Backward();


    /**
     * Returns the angle between two vectors in radians.
     * @param a: The first vector.
     * @param b: The second vector.
     * @return: A scalar value.
     */
    static inline float Angle(Vec3 a, Vec3 b);

    /**
     * Returns a vector with its magnitude clamped to maxLength.
     * @param vector: The target vector.
     * @param maxLength: The maximum length of the return vector.
     * @return: A new vector.
     */
    static inline Vec3 ClampMagnitude(Vec3 vector, float maxLength);

    /**
     * Returns the component of a in the direction of b (scalar projection).
     * @param a: The target vector.
     * @param b: The vector being compared against.
     * @return: A scalar value.
     */
    static inline float Component(Vec3 a, Vec3 b);

    /**
     * Returns the cross product of two vectors.
     * @param lhs: The left side of the multiplication.
     * @param rhs: The right side of the multiplication.
     * @return: A new vector.
     */
    static inline Vec3 Cross(Vec3 lhs, Vec3 rhs);

    /**
     * Returns the distance between a and b.
     * @param a: The first point.
     * @param b: The second point.
     * @return: A scalar value.
     */
    static inline float Distance(Vec3 a, Vec3 b);

    static inline char ToChar(Vec3 a);

    /**
     * Returns the dot product of two vectors.
     * @param lhs: The left side of the multiplication.
     * @param rhs: The right side of the multiplication.
     * @return: A scalar value.
     */
    static inline float Dot(Vec3 lhs, Vec3 rhs);

    /**
     * Converts a spherical representation of a vector into cartesian
     * coordinates.
     * This uses the ISO convention (radius r, inclination theta, azimuth phi).
     * @param rad: The magnitude of the vector.
     * @param theta: The angle in the xy plane from the x axis.
     * @param phi: The angle from the positive z axis to the vector.
     * @return: A new vector.
     */
    static inline Vec3 FromSpherical(float rad, float theta, float phi);

    /**
     * Returns a vector linearly interpolated between a and b, moving along
     * a straight line. The vector is clamped to never go beyond the end points.
     * @param a: The starting point.
     * @param b: The ending point.
     * @param t: The interpolation value [0-1].
     * @return: A new vector.
     */
    static inline Vec3 Lerp(Vec3 a, Vec3 b, float t);

    /**
     * Returns a vector linearly interpolated between a and b, moving along
     * a straight line.
     * @param a: The starting point.
     * @param b: The ending point.
     * @param t: The interpolation value [0-1] (no actual bounds).
     * @return: A new vector.
     */
    static inline Vec3 LerpUnclamped(Vec3 a, Vec3 b, float t);

    /**
     * Returns the magnitude of a vector.
     * @param v: The vector in question.
     * @return: A scalar value.
     */
    static inline float Magnitude(Vec3 v);

    /**
     * Returns a vector made from the largest components of two other vectors.
     * @param a: The first vector.
     * @param b: The second vector.
     * @return: A new vector.
     */
    static inline Vec3 Max(Vec3 a, Vec3 b);

    /**
     * Returns a vector made from the smallest components of two other vectors.
     * @param a: The first vector.
     * @param b: The second vector.
     * @return: A new vector.
     */
    static inline Vec3 Min(Vec3 a, Vec3 b);

    /**
     * Returns a vector "maxDistanceDelta" units closer to the target. This
     * interpolation is in a straight line, and will not overshoot.
     * @param current: The current position.
     * @param target: The destination position.
     * @param maxDistanceDelta: The maximum distance to move.
     * @return: A new vector.
     */
    static inline Vec3 MoveTowards(Vec3 current, Vec3 target,
                                   float maxDistanceDelta);

    /**
     * Returns a new vector with magnitude of one.
     * @param v: The vector in question.
     * @return: A new vector.
     */
    static inline Vec3 Normalized(Vec3 v);

    /**
     * Returns an arbitrary vector orthogonal to the input.
     * This vector is not normalized.
     * @param v: The input vector.
     * @return: A new vector.
     */
    static inline Vec3 Orthogonal(Vec3 v);

    /**
     * Creates a new coordinate system out of the three vectors.
     * Normalizes "normal", normalizes "tangent" and makes it orthogonal to
     * "normal" and normalizes "binormal" and makes it orthogonal to both
     * "normal" and "tangent".
     * @param normal: A reference to the first axis vector.
     * @param tangent: A reference to the second axis vector.
     * @param binormal: A reference to the third axis vector.
     */
    static inline void OrthoNormalize(Vec3 &normal, Vec3 &tangent,
                                      Vec3 &binormal);

    /**
     * Returns the vector projection of a onto b.
     * @param a: The target vector.
     * @param b: The vector being projected onto.
     * @return: A new vector.
     */
    static inline Vec3 Project(Vec3 a, Vec3 b);

    /**
     * Returns a vector projected onto a plane orthogonal to "planeNormal".
     * This can be visualized as the shadow of the vector onto the plane, if
     * the light source were in the direction of the plane normal.
     * @param vector: The vector to project.
     * @param planeNormal: The normal of the plane onto which to project.
     * @param: A new vector.
     */
    static inline Vec3 ProjectOnPlane(Vec3 vector, Vec3 planeNormal);

    /**
     * Returns a vector reflected off the plane orthogonal to the normal.
     * The input vector is pointed inward, at the plane, and the return vector
     * is pointed outward from the plane, like a beam of light hitting and then
     * reflecting off a mirror.
     * @param vector: The vector traveling inward at the plane.
     * @param planeNormal: The normal of the plane off of which to reflect.
     * @return: A new vector pointing outward from the plane.
     */
    static inline Vec3 Reflect(Vec3 vector, Vec3 planeNormal);

    /**
     * Returns the vector rejection of a on b.
     * @param a: The target vector.
     * @param b: The vector being projected onto.
     * @return: A new vector.
     */
    static inline Vec3 Reject(Vec3 a, Vec3 b);

    /**
     * Rotates vector "current" towards vector "target" by "maxRadiansDelta".
     * This treats the vectors as directions and will linearly interpolate
     * between their magnitudes by "maxMagnitudeDelta". This function does not
     * overshoot. If a negative delta is supplied, it will rotate away from
     * "target" until it is pointing the opposite direction, but will not
     * overshoot that either.
     * @param current: The starting direction.
     * @param target: The destination direction.
     * @param maxRadiansDelta: The maximum number of radians to rotate.
     * @param maxMagnitudeDelta: The maximum delta for magnitude interpolation.
     * @return: A new vector.
     */
    static inline Vec3 RotateTowards(Vec3 current, Vec3 target,
                                     float maxRadiansDelta,
                                     float maxMagnitudeDelta);

    /**
     * Multiplies two vectors element-wise.
     * @param a: The lhs of the multiplication.
     * @param b: The rhs of the multiplication.
     * @return: A new vector.
     */
    static inline Vec3 Scale(Vec3 a, Vec3 b);

    /**
     * Returns a vector rotated towards b from a by the percent t.
     * Since interpolation is done spherically, the vector moves at a constant
     * angular velocity. This rotation is clamped to 0 <= t <= 1.
     * @param a: The starting direction.
     * @param b: The ending direction.
     * @param t: The interpolation value [0-1].
     */
    static inline Vec3 Slerp(Vec3 a, Vec3 b, float t);

    /**
     * Returns a vector rotated towards b from a by the percent t.
     * Since interpolation is done spherically, the vector moves at a constant
     * angular velocity. This rotation is unclamped.
     * @param a: The starting direction.
     * @param b: The ending direction.
     * @param t: The interpolation value [0-1].
     */
    static inline Vec3 SlerpUnclamped(Vec3 a, Vec3 b, float t);

    /**
     * Returns the squared magnitude of a vector.
     * This is useful when comparing relative lengths, where the exact length
     * is not important, and much time can be saved by not calculating the
     * square root.
     * @param v: The vector in question.
     * @return: A scalar value.
     */
    static inline float SqrMagnitude(Vec3 v);

    /**
     * Calculates the spherical coordinate space representation of a vector.
     * This uses the ISO convention (radius r, inclination theta, azimuth phi).
     * @param vector: The vector to convert.
     * @param rad: The magnitude of the vector.
     * @param theta: The angle in the xy plane from the x axis.
     * @param phi: The angle from the positive z axis to the vector.
     */
    static inline void ToSpherical(Vec3 vector, float &rad, float &theta,
                                   float &phi);


    /**
     * Operator overloading.
     */
    inline struct Vec3& operator+=(const float rhs);
    inline struct Vec3& operator-=(const float rhs);
    inline struct Vec3& operator*=(const float rhs);
    inline struct Vec3& operator/=(const float rhs);
    inline struct Vec3& operator+=(const Vec3 rhs);
    inline struct Vec3& operator-=(const Vec3 rhs);
};

inline Vec3 operator-(Vec3 rhs);
inline Vec3 operator+(Vec3 lhs, const float rhs);
inline Vec3 operator-(Vec3 lhs, const float rhs);
inline Vec3 operator*(Vec3 lhs, const float rhs);
inline Vec3 operator/(Vec3 lhs, const float rhs);
inline Vec3 operator+(const float lhs, Vec3 rhs);
inline Vec3 operator-(const float lhs, Vec3 rhs);
inline Vec3 operator*(const float lhs, Vec3 rhs);
inline Vec3 operator/(const float lhs, Vec3 rhs);
inline Vec3 operator+(Vec3 lhs, const Vec3 rhs);
inline Vec3 operator-(Vec3 lhs, const Vec3 rhs);
inline bool operator==(const Vec3 lhs, const Vec3 rhs);
inline bool operator!=(const Vec3 lhs, const Vec3 rhs);



/*******************************************************************************
 * Implementation
 */

Vec3::Vec3() {}
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
Vec3::~Vec3() {}


Vec3 Vec3::zero() { return Vec3(0, 0, 0); }
Vec3 Vec3::One() { return Vec3(1, 1, 1); }
Vec3 Vec3::Right() { return Vec3(1, 0, 0); }
Vec3 Vec3::Left() { return Vec3(-1, 0, 0); }
Vec3 Vec3::Up() { return Vec3(0, 1, 0); }
Vec3 Vec3::Down() { return Vec3(0, -1, 0); }
Vec3 Vec3::Forward() { return Vec3(0, 0, 1); }
Vec3 Vec3::Backward() { return Vec3(0, 0, -1); }


float Vec3::Angle(Vec3 a, Vec3 b)
{
    float v = Dot(a, b) / (Magnitude(a) * Magnitude(b));
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    return acos(v);
}

Vec3 Vec3::ClampMagnitude(Vec3 vector, float maxLength)
{
    float length = Magnitude(vector);
    if (length > maxLength)
        vector *= maxLength / length;
    return vector;
}

float Vec3::Component(Vec3 a, Vec3 b)
{
    return Dot(a, b) / Magnitude(b);
}

Vec3 Vec3::Cross(Vec3 lhs, Vec3 rhs)
{
    float x = lhs.y * rhs.z - lhs.z * rhs.y;
    float y = lhs.z * rhs.x - lhs.x * rhs.z;
    float z = lhs.x * rhs.y - lhs.y * rhs.x;
    return Vec3(x, y, z);
}

float Vec3::Distance(Vec3 a, Vec3 b)
{
    return Vec3::Magnitude(a - b);
}

float Vec3::Dot(Vec3 lhs, Vec3 rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3 Vec3::FromSpherical(float rad, float theta, float phi)
{
    Vec3 v;
    v.x = rad * sin(theta) * cos(phi);
    v.y = rad * sin(theta) * sin(phi);
    v.z = rad * cos(theta);
    return v;
}

Vec3 Vec3::Lerp(Vec3 a, Vec3 b, float t)
{
    if (t < 0) return a;
    else if (t > 1) return b;
    return LerpUnclamped(a, b, t);
}

Vec3 Vec3::LerpUnclamped(Vec3 a, Vec3 b, float t)
{
    return (b - a) * t + a;
}

float Vec3::Magnitude(Vec3 v)
{
    return sqrt(SqrMagnitude(v));
}

Vec3 Vec3::Max(Vec3 a, Vec3 b)
{
    float x = a.x > b.x ? a.x : b.x;
    float y = a.y > b.y ? a.y : b.y;
    float z = a.z > b.z ? a.z : b.z;
    return Vec3(x, y, z);
}

Vec3 Vec3::Min(Vec3 a, Vec3 b)
{
    float x = a.x > b.x ? b.x : a.x;
    float y = a.y > b.y ? b.y : a.y;
    float z = a.z > b.z ? b.z : a.z;
    return Vec3(x, y, z);
}

Vec3 Vec3::MoveTowards(Vec3 current, Vec3 target,
                       float maxDistanceDelta)
{
    Vec3 d = target - current;
    float m = Magnitude(d);
    if (m < maxDistanceDelta || m == 0)
        return target;
    return current + (d * maxDistanceDelta / m);
}

Vec3 Vec3::Normalized(Vec3 v)
{
    float mag = Magnitude(v);
    if (mag == 0)
        return Vec3::zero();
    return v / mag;
}

Vec3 Vec3::Orthogonal(Vec3 v)
{
    return v.z < v.x ? Vec3(v.y, -v.x, 0) : Vec3(0, -v.z, v.y);
}

void Vec3::OrthoNormalize(Vec3 &normal, Vec3 &tangent,
                          Vec3 &binormal)
{
    normal = Normalized(normal);
    tangent = ProjectOnPlane(tangent, normal);
    tangent = Normalized(tangent);
    binormal = ProjectOnPlane(binormal, tangent);
    binormal = ProjectOnPlane(binormal, normal);
    binormal = Normalized(binormal);
}

Vec3 Vec3::Project(Vec3 a, Vec3 b)
{
    float m = Magnitude(b);
    return Dot(a, b) / (m * m) * b;
}

Vec3 Vec3::ProjectOnPlane(Vec3 vector, Vec3 planeNormal)
{
    return Reject(vector, planeNormal);
}

Vec3 Vec3::Reflect(Vec3 vector, Vec3 planeNormal)
{
    return vector - 2 * Project(vector, planeNormal);
}

Vec3 Vec3::Reject(Vec3 a, Vec3 b)
{
    return a - Project(a, b);
}

Vec3 Vec3::RotateTowards(Vec3 current, Vec3 target,
                         float maxRadiansDelta,
                         float maxMagnitudeDelta)
{
    float magCur = Magnitude(current);
    float magTar = Magnitude(target);
    float newMag = magCur + maxMagnitudeDelta *
                            ((magTar > magCur) - (magCur > magTar));
    newMag = fmin(newMag, fmax(magCur, magTar));
    newMag = fmax(newMag, fmin(magCur, magTar));

    float totalAngle = Angle(current, target) - maxRadiansDelta;
    if (totalAngle <= 0)
        return Normalized(target) * newMag;
    else if (totalAngle >= M_PI)
        return Normalized(-target) * newMag;

    Vec3 axis = Cross(current, target);
    float magAxis = Magnitude(axis);
    if (magAxis == 0)
        axis = Normalized(Cross(current, current + Vec3(3.95, 5.32, -4.24)));
    else
        axis /= magAxis;
    current = Normalized(current);
    Vec3 newVector = current * cos(maxRadiansDelta) +
                     Cross(axis, current) * sin(maxRadiansDelta);
    return newVector * newMag;
}

Vec3 Vec3::Scale(Vec3 a, Vec3 b)
{
    return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 Vec3::Slerp(Vec3 a, Vec3 b, float t)
{
    if (t < 0) return a;
    else if (t > 1) return b;
    return SlerpUnclamped(a, b, t);
}

Vec3 Vec3::SlerpUnclamped(Vec3 a, Vec3 b, float t)
{
    float magA = Magnitude(a);
    float magB = Magnitude(b);
    a /= magA;
    b /= magB;
    float dot = Dot(a, b);
    dot = fmax(dot, -1.0);
    dot = fmin(dot, 1.0);
    float theta = acos(dot) * t;
    Vec3 relativeVec = Normalized(b - a * dot);
    Vec3 newVec = a * cos(theta) + relativeVec * sin(theta);
    return newVec * (magA + (magB - magA) * t);
}

float Vec3::SqrMagnitude(Vec3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

void Vec3::ToSpherical(Vec3 vector, float &rad, float &theta,
                       float &phi)
{
    rad = Magnitude(vector);
    float v = vector.z / rad;
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    theta = acos(v);
    phi = atan2(vector.y, vector.x);
}


struct Vec3& Vec3::operator+=(const float rhs)
{
    x += rhs;
    y += rhs;
    z += rhs;
    return *this;
}

struct Vec3& Vec3::operator-=(const float rhs)
{
    x -= rhs;
    y -= rhs;
    z -= rhs;
    return *this;
}

struct Vec3& Vec3::operator*=(const float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

struct Vec3& Vec3::operator/=(const float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

struct Vec3& Vec3::operator+=(const Vec3 rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

struct Vec3& Vec3::operator-=(const Vec3 rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}



Vec3 operator-(Vec3 rhs) { return rhs * -1; }
Vec3 operator+(Vec3 lhs, const float rhs) { return lhs += rhs; }
Vec3 operator-(Vec3 lhs, const float rhs) { return lhs -= rhs; }
Vec3 operator*(Vec3 lhs, const float rhs) { return lhs *= rhs; }
Vec3 operator/(Vec3 lhs, const float rhs) { return lhs /= rhs; }
Vec3 operator+(const float lhs, Vec3 rhs) { return rhs += lhs; }
Vec3 operator-(const float lhs, Vec3 rhs) { return rhs -= lhs; }
Vec3 operator*(const float lhs, Vec3 rhs) { return rhs *= lhs; }
Vec3 operator/(const float lhs, Vec3 rhs) { return rhs /= lhs; }
Vec3 operator+(Vec3 lhs, const Vec3 rhs) { return lhs += rhs; }
Vec3 operator-(Vec3 lhs, const Vec3 rhs) { return lhs -= rhs; }

bool operator==(const Vec3 lhs, const Vec3 rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

bool operator!=(const Vec3 lhs, const Vec3 rhs)
{
    return !(lhs == rhs);
}
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
