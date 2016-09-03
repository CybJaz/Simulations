#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 position_screenspace;
layout(location = 1) in vec2 vertex_uv;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){

	// Output position of the vertex, in clip space
	// map [0..800][0..600] to [-1..1][-1..1]
	// vec2 vertexPosition_homoneneousspace = position_screenspace - vec2(100,100); // [0..800][0..600] -> [-400..400][-300..300]
	// vertexPosition_homoneneousspace /= vec2(100,100);
	// gl_Position =  vec4(vertexPosition_homoneneousspace,0,1);
	
	gl_Position = vec4(position_screenspace, 0, 1);

	// UV of the vertex. No special space for this one.
	UV = vertex_uv;
}

