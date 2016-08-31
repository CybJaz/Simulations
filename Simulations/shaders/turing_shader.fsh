#version 430

uniform sampler2D srcTex;

in vec2 texCoord;

uniform uint state;
uniform vec2 limits;

void main() 
{
	const float max_value = 6.0f;

	vec2 oth_coord = vec2(1,1);
	float sth = texture(srcTex, vec2(0.2,0.4)).x;
	float col;
	
	if (state == 0)
		col = texture(srcTex, texCoord).x;
	else
		col = texture(srcTex, texCoord).y;
	
	if(col < 0.0f)
		gl_FragColor = vec4(0.0f, .6f, 0.4f, 1.0f);
	else 
		gl_FragColor = vec4(abs(3*col/(limits[1]*2)), abs(col/limits[1]) / 2.f, 0.0f, 1.0f);
}
	