#version 430

uniform int upd_buf;
layout (rg32f, binding = 0) uniform image2D u_vals;
layout (rg32f, binding = 1) uniform image2D v_vals;
layout (local_size_x = 1, local_size_y = 1) in;

void main() {	
	ivec2 glob_pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 loc_pos = ivec2(gl_LocalInvocationID.xy);

	vec2 u_values = imageLoad(u_vals, glob_pos).xy;

	float new_u = (mod(glob_pos.x, 16.f) + mod(loc_pos.y, 16.f));

	if(upd_buf == 0)
	{
		imageStore(u_vals, glob_pos, vec4(new_u, new_u, 0.f, 0.f));
		// imageStore(v_vals, storePos, vec4(new_v, v_values.y, 0.f, 0.f));
	} else {
		imageStore(u_vals, glob_pos, vec4(new_u, new_u, 0.f, 0.f));
		// imageStore(v_vals, storePos, vec4(v_values.x, new_v, 0.f, 0.f));
	}
}