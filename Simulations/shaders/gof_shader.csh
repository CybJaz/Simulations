#version 430

uniform int upd_buf;
layout (rg32f, binding = 0) uniform image2D u_vals;
layout (rg32f, binding = 1) uniform image2D v_vals;
layout (local_size_x = 1, local_size_y = 1) in;

uint modui(int x, int n) { if(x < 0) return n+x; else if(x>=n) return x-n; else return x;}

void main() {	
	ivec2 glob_pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 loc_pos = ivec2(gl_LocalInvocationID.xy);

	int xc = glob_pos.x, yc = glob_pos.y;

	int n = 256;

	vec2 u_values = imageLoad(u_vals, glob_pos).xy;
	uint r_buf = 1 - upd_buf;

	uint xd = modui(xc-1, n), xu = modui(xc+1, n), yd = modui(yc-1, n), yu = modui(yc+1, n);

	uint neighbours = 0;

	if ((imageLoad(u_vals, ivec2(xd, yc))[r_buf]) > 0.5f) neighbours++;
	if ((imageLoad(u_vals, ivec2(xu, yc))[r_buf]) > 0.5f) neighbours++;
	if ((imageLoad(u_vals, ivec2(xc, yd))[r_buf]) > 0.5f) neighbours++;
	if ((imageLoad(u_vals, ivec2(xc, yu))[r_buf]) > 0.5f) neighbours++;

	if ((imageLoad(u_vals, ivec2(xd, yd))[r_buf]) > 0.5f) neighbours++;
	if ((imageLoad(u_vals, ivec2(xu, yd))[r_buf]) > 0.5f) neighbours++;
	if ((imageLoad(u_vals, ivec2(xd, yu))[r_buf]) > 0.5f) neighbours++;
	if ((imageLoad(u_vals, ivec2(xu, yu))[r_buf]) > 0.5f) neighbours++;

	float new_u = 0.0f;
	float old_u = (imageLoad(u_vals, ivec2(xc, yc))[r_buf]);

	if (old_u < 0.5f) {
		if (neighbours == 3)
			new_u = 1.0f;
	} else {
		if (neighbours > 1 && neighbours < 4)
			new_u = 1.0f;
	}

	// float new_u = (mod(glob_pos.x, 16.f) + mod(loc_pos.y, 16.f));

	if(upd_buf == 0)
	{
		imageStore(u_vals, glob_pos, vec4(new_u, old_u, 0.f, 0.f));
		// imageStore(v_vals, storePos, vec4(new_v, v_values.y, 0.f, 0.f));
	} else {
		imageStore(u_vals, glob_pos, vec4(old_u, new_u, 0.f, 0.f));
		// imageStore(v_vals, storePos, vec4(v_values.x, new_v, 0.f, 0.f));
	}
}