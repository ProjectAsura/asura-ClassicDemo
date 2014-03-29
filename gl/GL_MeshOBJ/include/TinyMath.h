#ifndef __VEC_H__
#define __VEC_H__

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <cmath>


/////////////////////////////////////////////////////////////////////////////////////////////
// Vec2
/////////////////////////////////////////////////////////////////////////////////////////////
struct Vec2
{
    float x;
    float y;

    Vec2()
    : x( 0.0f )
    , y( 0.0f )
    { /* DO_NOTHING */ }

    Vec2( const float nx, const float ny )
    : x( nx )
    , y( ny )
    { /* DO_NOTHING */ }

    Vec2( const Vec2& value )
    : x( value.x )
    , y( value.y )
    { /* DO_NOTHING */ }

    Vec2 operator + ( const Vec2& value ) const
    { return Vec2( x + value.x, y + value.y ); }

    Vec2 operator - ( const Vec2& value ) const
    { return Vec2( x - value.x, y - value.y ); }

    Vec2 operator * ( const float value ) const
    { return Vec2( x * value, y * value); }

    float Length()
    { return sqrtf( ( x * x ) + ( y * y ) ); }

    operator float* ()
    { return &x; }

    operator const float* () const
    { return &x; }

    static Vec2 Max( const Vec2& a, const Vec2& b )
    {
        Vec2 result;
        result.x = ( a.x > b.x ) ? a.x : b.x;
        result.y = ( a.y > b.y ) ? a.y : b.y;
        return result;
    }

    static Vec2 Min( const Vec2& a, const Vec2& b )
    {
        Vec2 result;
        result.x = ( a.x < b.x ) ? a.x : b.x;
        result.y = ( a.y < b.y ) ? a.y : b.y;
        return result;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Vec3
/////////////////////////////////////////////////////////////////////////////////////////////
struct Vec3
{
    float x;
    float y;
    float z;

    Vec3()
    : x( 0.0f )
    , y( 0.0f )
    , z( 0.0f )
    { /* DO_NOTHING */ }

    Vec3( const float nx, const float ny, const float nz )
    : x( nx )
    , y( ny )
    , z( nz )
    { /* DO_NOTHING */ }

    Vec3( const Vec3& value )
    : x( value.x )
    , y( value.y )
    , z( value.z )
    { /* DO_NOTHING */ }

    Vec3 operator + ( const Vec3& value ) const
    { return Vec3( x + value.x, y + value.y, z + value.z ); }

    Vec3 operator - ( const Vec3& value ) const
    { return Vec3( x - value.x, y - value.y, z- value.z ); }

    Vec3 operator * ( const float value ) const
    { return Vec3( x * value, y * value, z * value ); }

    float Length()
    { return sqrtf( ( x * x ) + ( y * y ) + ( z * z ) ); }

    operator float* ()
    { return &x; }

    operator const float* () const
    { return &x; }

    static Vec3 Max( const Vec3& a, const Vec3& b )
    {
        Vec3 result;
        result.x = ( a.x > b.x ) ? a.x : b.x;
        result.y = ( a.y > b.y ) ? a.y : b.y;
        result.z = ( a.z > b.z ) ? a.z : b.z;
        return result;
    }

    static Vec3 Min( const Vec3& a, const Vec3& b )
    {
        Vec3 result;
        result.x = ( a.x < b.x ) ? a.x : b.x;
        result.y = ( a.y < b.y ) ? a.y : b.y;
        result.z = ( a.z < b.z ) ? a.z : b.z;
        return result;
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// Vec4
/////////////////////////////////////////////////////////////////////////////////////////////
struct Vec4
{
    float x;
    float y;
    float z;
    float w;

    Vec4()
    : x( 0.0f )
    , y( 0.0f )
    , z( 0.0f )
    , w( 0.0f )
    { /* DO_NOTHING */ }

    Vec4( const float nx, const float ny, const float nz, const float nw )
    : x( nx )
    , y( ny )
    , z( nz )
    , w( nw )
    { /* DO_NOTHING */ }

    Vec4( const Vec4& value )
    : x( value.x )
    , y( value.y )
    , z( value.z )
    , w( value.w )
    { /* DO_NOTHING */ }

    Vec4 operator + ( const Vec4& value ) const
    { return Vec4( x + value.x, y + value.y, z + value.z, w + value.w ); }

    Vec4 operator - ( const Vec4& value ) const
    { return Vec4( x - value.x, y - value.y, z- value.z, w - value.w ); }

    Vec4 operator * ( const float value ) const
    { return Vec4( x * value, y * value, z * value, w * value ); }

    float Length()
    { return sqrtf( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) ); }

    operator float* ()
    { return &x; }

    operator const float* () const
    { return &x; }

    static Vec4 Max( const Vec4& a, const Vec4& b )
    {
        Vec4 result;
        result.x = ( a.x > b.x ) ? a.x : b.x;
        result.y = ( a.y > b.y ) ? a.y : b.y;
        result.z = ( a.z > b.z ) ? a.z : b.z;
        result.w = ( a.w > b.w ) ? a.w : b.w;
        return result;
    }

    static Vec4 Min( const Vec4& a, const Vec4& b )
    {
        Vec4 result;
        result.x = ( a.x < b.x ) ? a.x : b.x;
        result.y = ( a.y < b.y ) ? a.y : b.y;
        result.z = ( a.z < b.z ) ? a.z : b.z;
        result.w = ( a.w < b.w ) ? a.w : b.w;
        return result;
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// BoundingBox
/////////////////////////////////////////////////////////////////////////////////////////////
struct BoundingBox
{
    Vec3 maxi;
    Vec3 mini;
    Vec3 size;

    BoundingBox()
    : maxi( -3.402823466e+38F, -3.402823466e+38F, -3.402823466e+38F )
    , mini(  3.402823466e+38F,  3.402823466e+38F,  3.402823466e+38F)
    , size( 0.0f, 0.0f, 0.0f )
    { /* DO_NOTHING */ }

    BoundingBox( const Vec3& value )
    : maxi( value )
    , mini( value )
    , size( 0.0f, 0.0f, 0.0f )
    { /* DO_NOTHING */ }

    void Merge( const Vec3& value )
    {
        maxi = Vec3::Max( value, maxi );
        mini = Vec3::Min( value, mini );
        size = ( maxi - mini );
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////
// BoundingSphere
/////////////////////////////////////////////////////////////////////////////////////////////
struct BoundingSphere
{
    Vec3 center;
    float radius;

    BoundingSphere() 
    : center()
    , radius( 0.0f )
    { /* DO_NOTHING */ }

    BoundingSphere( const BoundingBox& value )
    {
        center = ( value.maxi + value.mini ) * 0.5f;
        Vec3 diff = value.maxi - value.mini;
        radius = diff.Length() * 0.5f;
    }
};

#endif//__VEC_H__