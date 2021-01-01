#version 330
in vec4 v_color;
out vec4 o_color;

uniform vec2 iResolution;
uniform float iTime;
uniform vec2 iMouse;
uniform sampler3D voxels;

void main() {
	o_color = v_color * abs(sin(gl_FragCoord.x / iResolution.x
				+ iTime / 10000.0
				+ iMouse.x / iResolution.x));
	o_color *= texture(voxels, vec3(0, 0, 0));
}
