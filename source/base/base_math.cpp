

internal inline f32 normalize_vec2(Vec2 point) { return sqrt((point.x*point.x) + (point.y * point.y)); }
	
    // requires point to be scaled to  0 <= T <= 1
    // segmeent defined by S(t) = A + t(B - A)
    internal b32 intersect_segment_2d(Vec2 a, Vec2 b)
    {
// TODO: ...
return false;
		
    }
	
    internal inline  s32 dot(Vec2 a, Vec2 b) { return (a.x * b.x) + (a.y * b.y); }
	
    internal Plane compute_plane(Vec2 a, Vec2 b, Vec3 c)
    {
        // Plane p;
        // p.n = normalize_vec2(a); // TODO: do we even nnned to normalize using q sqaure root in 2d space
        // p.d = dot(p.n, a);

		
        // return p;
		
             return {};
}

internal Vec2 multiply_vec2(Vec2 point, f32 scalar)
{

Vec2 result;
	result.x = point.x * scalar;
	result.y = point.y * scalar;

return result;
}
	
	
internal Vec2 normalize_vec2_position_to_screen(Vec2 pos, s32 width, s32 height)
{
return Vec2(Clamp((pos.x + 1.0f) * 0.5f * width, 0, width - 1), Clamp((pos.y + 1.0f) * 0.5f * height, 0, width - 1));
}

