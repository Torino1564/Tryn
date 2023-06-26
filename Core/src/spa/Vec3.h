#pragma once 

namespace tryn::spa
{
    template<typename T>
    struct Vec3T
    {
        // functions 
        Vec3T& operator+=(const Vec3T& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }
        Vec3T& operator-=(const Vec3T& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }
        Vec3T operator+(const Vec3T& rhs) const
        {
            return Vec3T{ *this } += rhs;
        }
        Vec3T operator-(const Vec3T& rhs) const
        {
            return Vec3T{ *this } -= rhs;
        }
        Vec3T& operator*=(const T& rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            return *this;
        }
        Vec3T operator*(const T& rhs)
        {
            return Vec3T{ *this } *rhs;
        }
        bool operator==(const Vec3T& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
        // data 
        T x, y, z;
    };

    using Vec3F = Vec3T<float>;
    using Vec3I = Vec3T<int>;
}
