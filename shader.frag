#version 330
in vec4 v_color;
out vec4 o_color;

uniform vec2 iResolution;
uniform float iTime;
uniform vec2 iMouse;
uniform vec3 iCameraDirection;

#define MAX_ITER 400.0
#define fragColor o_color
#define fragCoord gl_FragCoord
void main() {
	vec2 uv = (fragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
	vec2 m = vec2(atan(iCameraDirection.z, iCameraDirection.x), iCameraDirection.y);
	
	float zoom = pow(10.0, -m.x * 5.0);
	vec2 c = zoom * uv;
	c += vec2(-0.69955, 0.37999);

	float iter = 0.0;
	vec2 z = vec2(0);
	
	for (float i = 0.0; i < MAX_ITER; i++) {
		z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.y * z.x) + c;
		if (length(z) > 2.0) break;
		iter++;
	}
	
	float f = iter / MAX_ITER;
	
	vec3 col = mix(vec3(0.0, 0.0, 0.0), vec3(0.5, 1.0, 0.1), f);
	fragColor = vec4(col, 1.0);
}
