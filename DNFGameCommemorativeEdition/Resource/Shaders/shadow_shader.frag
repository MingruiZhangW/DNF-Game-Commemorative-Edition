#version 330

uniform bool shadow = true;
uniform float alpha = 0.4;

uniform vec3 colour;
uniform sampler2D theTexture;

in vec2 TextureCoor;

out vec4 fragColor;

void main() {
	vec4 previous_color = texture(theTexture, TextureCoor);
	vec4 block_color;
	if(shadow == true){
		block_color = vec4(0.0,0.0,0.0,previous_color.a * alpha);
	}else{
		block_color = vec4(previous_color.r,previous_color.g,previous_color.b,previous_color.a * alpha);
	}

    fragColor = block_color;
}
