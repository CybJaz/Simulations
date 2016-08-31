/* Created by Michał Lipiński
	Compute shader for Turing patterns.

	Shader write and read from the same image, but in one iteration it uses 
	red layer to read and green to write and vice versa.
*/

#version 430
uniform int upd_buf;
uniform int n;

layout (rg32f, binding = 0) uniform image2D u_vals;
layout (rg32f, binding = 1) uniform image2D v_vals;
layout (std430, binding = 2) buffer limits
{
	float minmax[];
};

layout (local_size_x = 1, local_size_y = 1) in;

uint modui(int x, int n) { if(x < 0) return n+x; else if(x>=n) return x-n; else return x;}

void main() {
	// float A = 0.5f, B = 12.0f;
	float A = 1, B = 11;
	// int n = 16;
	int n = int(gl_NumWorkGroups.x);
	float time_step = 0.01f;

	ivec2 glob_pos = ivec2(gl_GlobalInvocationID.xy);

	int xc = glob_pos.x, yc = glob_pos.y;

	uint read_buf = 1 - upd_buf;

	vec2 u_val_curr = imageLoad(u_vals, glob_pos).xy;
	vec2 v_val_curr = imageLoad(v_vals, glob_pos).xy;

	float old_u = u_val_curr[read_buf];
	float old_v = v_val_curr[read_buf];

	// minmax[0] = 1.0f;
	// if (gl_NumWorkGroups.x == gl_NumWorkGroups.y)
	// 	minmax[1] = 15.0f;

	// if (gl_NumWorkGroups.x == 16)
	// 	minmax[1] = 45.0f;

	// Indices on left, right, under and above current index.
	uint xd = modui(xc-1, n), xu = modui(xc+1, n), 
	yd = modui(yc-1, n), yu = modui(yc+1, n);

	// Laplacian for U and V
	float kernel_u = 8 * old_u
		- (imageLoad(u_vals, ivec2(xd, yc))[read_buf] + imageLoad(u_vals, ivec2(xu, yc))[read_buf] 
			+ imageLoad(u_vals, ivec2(xc, yd))[read_buf] + imageLoad(u_vals, ivec2(xc, yu))[read_buf])
		- (imageLoad(u_vals, ivec2(xd, yd))[read_buf] + imageLoad(u_vals, ivec2(xu, yd))[read_buf] 
			+ imageLoad(u_vals, ivec2(xd, yu))[read_buf] + imageLoad(u_vals, ivec2(xu, yu))[read_buf]); 

	float kernel_v = 8 * old_v
		- (imageLoad(v_vals, ivec2(xd, yc))[read_buf] + imageLoad(v_vals, ivec2(xu, yc))[read_buf] 
			+ imageLoad(v_vals, ivec2(xc, yd))[read_buf] + imageLoad(v_vals, ivec2(xc, yu))[read_buf])
		- (imageLoad(v_vals, ivec2(xd, yd))[read_buf] + imageLoad(v_vals, ivec2(xu, yd))[read_buf] 
			+ imageLoad(v_vals, ivec2(xd, yu))[read_buf] + imageLoad(v_vals, ivec2(xu, yu))[read_buf]); 
	
	float d_u = old_u * (old_v - 1) - 12 - A * kernel_u;
	float d_v = 16 - old_u * old_v - B * kernel_v;
	

	// Basic Euler method to update values.
	float new_u = max(old_u + time_step * d_u, 0.0f);
	float new_v = max(old_v + time_step * d_v, 0.0f);

	if (minmax[0] > new_u)
		minmax[0] = new_u;
	if (minmax[1] < new_u)
		minmax[1] = new_u;
	
	if (minmax[2] > new_v)
		minmax[2] = new_v;
	if (minmax[3] < new_v)
		minmax[3] = new_v;
	
	if(upd_buf == 0){ 
		imageStore(u_vals, glob_pos, vec4(new_u, old_u, 0.f, 0.f));
		imageStore(v_vals, glob_pos, vec4(new_v, old_v, 0.f, 0.f));
	} else { 
		imageStore(u_vals, glob_pos, vec4(old_u, new_u, 0.f, 0.f));
		imageStore(v_vals, glob_pos, vec4(old_v, new_v, 0.f, 0.f));
	}



	// imageStore(u_vals, glob_pos, vec4(minmax[0], minmax[1], 0.f, 0.f));
	// imageStore(u_vals, glob_pos, vec4(1.f,5.f, 0.f, 0.f));
}
