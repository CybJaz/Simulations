#version 430 
layout(location = 0) in vec2 position;

out vec2 texCoord;

uniform sampler2D srcTex;
uniform uint state;

void main() {
	texCoord = position * 0.5f + 0.5f;
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}