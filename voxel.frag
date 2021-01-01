#version 330
in vec4 v_color;
out vec4 o_color;

uniform vec2 iResolution;
uniform float iTime;
uniform vec2 iMouse;
uniform vec3 iCameraDirection;
uniform sampler3D voxels;

#define MAX_DISTANCE 600.0

float getVoxel(vec3 c){
	//if(min(vec3(0), c) != vec3(0)) return 0.0;
	//if(max(vec3(1), c) != vec3(1)) return 0.0;
	return texture(voxels, c).x;
}

mat2 rot(float a) {
	float c = cos(a);
	float s = sin(a);
	return mat2(c, -s, s, c);
}

vec3 rotCam(vec2 uv, vec3 p, vec3 f, float z) {;
	vec3 r = normalize(cross(vec3(0, 1, 0), f));
	vec3 u = cross(f, r);
	vec3 c = p + f * z;
	vec3 i = c + uv.x * r + uv.y * u;
	vec3 d = normalize(i - p);
	return d;
}

float voxelCast(vec3 ro, vec3 rd, float maxDist){
	float t = 0.0;
	vec3 ic = floor(ro);
	vec3 stepC = sign(rd);
	vec3 tDelta = abs(vec3(1) / rd);
	vec3 dist = (ic - ro) * stepC + max(vec3(0), stepC);
	vec3 tMax = tDelta * dist;
	
	while (getVoxel(ic) == 0.0 && t < maxDist) {
		float tMin = min(min(tMax.x, tMax.y), tMax.z);
		t = tMin;
		vec3 s = sign(vec3(tMin) - tMax) + vec3(1);
		ic += stepC * s;
		tMax += tDelta * s;
	}
	return t;
}

void main() {
	vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
	vec2 m = iMouse.xy / iResolution.xy;

	vec3 ro = vec3(2, 2, 2);
	vec3 rd = iCameraDirection;
	rd = rotCam(uv, ro, rd, 1.0);

	vec3 col = vec3(0.7, 0.7, 0.3) * (1.0 - voxelCast(ro, rd, MAX_DISTANCE) / MAX_DISTANCE);

	o_color = vec4(col, 1.0);
}
