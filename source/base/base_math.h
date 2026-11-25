#ifndef BASE_MATH_H
#define BASE_MATH_H

#define Vec2(x, y)      \
(Vec2)              \
{                   \
(f32) x, (f32)y \
}

typedef struct Vec2 Vec2;
struct Vec2 {
	f32 x, y;
};

typedef struct Vec3 Vec3;
struct Vec3 {
	f32 x, y, z;
};

internal inline f32 normalize_vec2(Vec2 point);
internal b32        intersect_segment_2d(Vec2 a, Vec2 b);
internal inline s32 dot(Vec2 a, Vec2 b);
internal Vec2       multiply_vec2(Vec2 point, f32 scalar);
internal Vec2       normalize_vec2_position_to_screen(Vec2 pos, s32 width, s32 height);

#endif // BASE_MATH_H
