#version 330

uniform vec3 colour;
uniform sampler2D theTexture;

uniform vec4 outlineColor = vec4(0.65, 0.96, 0.19, 1.0);
uniform bool showOutline = false;

in vec2 TextureCoor;

out vec4 fragColor;

/*
 * For outline:
 *		We need to know whether we are at the closest border of the texture
 *      (outside the texture, alpha will be 0 for full transparency). So, we check
 *      whether the textutre coord plus a small value -> 
 *			top not bottom, bottom not top, left not right, right not left
 *      has the full transparency. If any of them are true, then we know this is the
 *      closest border. We assign outline color.
 */

void main() {
	vec4 resultColor;
	
	float sizeX = 1.0/float(textureSize(theTexture, 0).x);
	float sizeY = 1.0/float(textureSize(theTexture, 0).y);
	
	vec4 spriteColor = texture(theTexture, TextureCoor);

	bool rightHasColor = texture(theTexture, TextureCoor + vec2(sizeX, 0)).a < 1.0;
	bool leftHasColor = texture(theTexture, TextureCoor + vec2(-sizeX,0)).a < 1.0;
	bool bottomHasColor = texture(theTexture, TextureCoor + vec2(0, sizeY)).a < 1.0;
	bool topHasColor = texture(theTexture, TextureCoor + vec2(0, -sizeY)).a < 1.0;

	if (((rightHasColor && !leftHasColor) || (leftHasColor && !rightHasColor) ||
	    (bottomHasColor && !topHasColor) || (bottomHasColor && !topHasColor)) && showOutline) {
		resultColor = outlineColor;
	} else {
		resultColor = spriteColor;
	}

	fragColor = resultColor;
}
