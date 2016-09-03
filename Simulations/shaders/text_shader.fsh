#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D font_texture;

void main(){

	//color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 tex = texture( font_texture, UV );
	if (tex.x == 0.0f && tex.y == 0.0f && tex.z == 0.0f)
		color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	else
		color = tex;
}