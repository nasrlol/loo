#ifndef BASE_MATH_H
#define BASE_MATH_H

#define Vec2(x, y) (Vec2){x, y}

typedef struct Vec2 Vec2;
struct Vec2
{
    f32 x,y;
};

typedef struct Vec3 Vec3;
struct Vec3 {

    f32 x,y, z;

};

typedef struct Plane Plane;
struct Plane {

    Vec2 n;
    f32 d;

};


#endif // BASE_MATH_H
