/* Created by Michał Lipiński
	Compute shader for Turing patterns.
*/

#version 430
uniform int upd_buf;

layout (rg32f, binding = 0) uniform image2D u_vals;
layout (rg32f, binding = 1) uniform image2D v_vals;
layout (local_size_x = 1, local_size_y = 1) in;

uint modui(int x, int n) { if(x < 0) return n+x; else if(x>=n) return x-n; else return x;}

void main() {
	float A = 1.6, B = 6;
	// float A = 3.5, B = 16;
	int n = 512;
	float time_step = 0.01f;

	ivec2 glob_pos = ivec2(gl_GlobalInvocationID.xy);

	int xc = glob_pos.x, yc = glob_pos.y;

	uint read_buf = 1 - upd_buf;

	vec2 u_val_curr = imageLoad(u_vals, glob_pos).xy;
	vec2 v_val_curr = imageLoad(v_vals, glob_pos).xy;

	float old_u = u_val_curr[read_buf];
	float old_v = v_val_curr[read_buf];

	uint xd = modui(xc-1, n), xu = modui(xc+1, n), yd = modui(yc-1, n), yu = modui(yc+1, n);

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
	
	float new_u = max(old_u + time_step * d_u, 0.0f);
	float new_v = max(old_v + time_step * d_v, 0.0f);

	if(upd_buf == 0){ 
		imageStore(u_vals, glob_pos, vec4(new_u, old_u, 0.f, 0.f));
		imageStore(v_vals, glob_pos, vec4(new_v, old_v, 0.f, 0.f));
	} else { 
		imageStore(u_vals, glob_pos, vec4(old_u, new_u, 0.f, 0.f));
		imageStore(v_vals, glob_pos, vec4(old_v, new_v, 0.f, 0.f));
	} 
}
