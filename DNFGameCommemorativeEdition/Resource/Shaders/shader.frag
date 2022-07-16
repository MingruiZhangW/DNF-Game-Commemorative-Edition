#version 330

uniform sampler2D theTexture;

// Outline
uniform vec4 outlineColor = vec4(0.65, 0.96, 0.19, 1.0);
uniform bool showOutline = false;

// Text
uniform bool drawText = false;
uniform vec3 textColour = vec3(0.56, 0.54, 0.5);

// Hit
uniform bool hit = false;
uniform vec3 hitColour = vec3(0.52, 0.04, 0.04);

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
	vec4 spriteColor = texture(theTexture, TextureCoor);

	if (drawText) {
		vec4 sampled = vec4(1.0, 1.0, 1.0, spriteColor.r);
		resultColor = vec4(textColour, 1.0) * sampled;
	} else if (showOutline) {
		float sizeX = 1.0 / float(textureSize(theTexture, 0).x);
		float sizeY = 1.0 / float(textureSize(theTexture, 0).y);

		bool rightHasColor = texture(theTexture, TextureCoor + vec2(sizeX, 0)).a < 1.0;
		bool leftHasColor = texture(theTexture, TextureCoor + vec2(-sizeX,0)).a < 1.0;
		bool bottomHasColor = texture(theTexture, TextureCoor + vec2(0, sizeY)).a < 1.0;
		bool topHasColor = texture(theTexture, TextureCoor + vec2(0, -sizeY)).a < 1.0;

		if (((rightHasColor && !leftHasColor) || (leftHasColor && !rightHasColor) ||
			(bottomHasColor && !topHasColor) || (bottomHasColor && !topHasColor))) {
			resultColor = outlineColor;
		} else {
			resultColor = spriteColor;
		}
	} else if (hit) {
		resultColor = spriteColor * vec4(hitColour, 1.0);
	} else {
		resultColor = spriteColor;
	}

	fragColor = resultColor;
}
