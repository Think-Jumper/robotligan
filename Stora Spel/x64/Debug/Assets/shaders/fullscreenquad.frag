#version 440 core

out vec4 out_color;

in vec2 v_uv;

uniform sampler2D texture_color;
uniform sampler2D texture_emission;
uniform sampler2D texture_ssao;

uniform bool use_ao;
uniform int is_invisible;

uniform vec2 resolution;

#define MAX_SHOCKWAVES 8

uniform int shockwave_count;
uniform vec3 shockwave_positions[MAX_SHOCKWAVES];
uniform float shockwave_time_ratios[MAX_SHOCKWAVES];
uniform float shockwave_radii[MAX_SHOCKWAVES];

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}
float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),
		u.y);
	return res*res;
}

vec3 calcColor(vec2 uv) {
	vec3 color = texture(texture_color, uv).rgb;

	vec4 emission = texture(texture_emission, uv, 1);
	float ao = texture(texture_ssao, uv).r;
	if(use_ao) color*=ao;
	color += 2.*emission.rgb;
	
	if (is_invisible == 1) {
		vec3 effect = vec3(noise(gl_FragCoord.xy/100.0));
		effect *= vec3(0.85,0.85,1);
		color = mix(color, effect, smoothstep(0.35,0.85, length(uv-0.5)));
	}
	return color;
}

void main() {
	
	vec2 uv = v_uv;

	for(int i = 0; i < shockwave_count; i++) {
		vec2 pos = shockwave_positions[i].xy * resolution;
		float thickness_ratio = 0.5;
		float radius = shockwave_radii[i];
		float shockwave = smoothstep(radius, radius-2.0, length(pos - gl_FragCoord.xy));
		shockwave *= smoothstep((radius-2.)*thickness_ratio, radius-2.0,length(pos - gl_FragCoord.xy));
		shockwave *= 1.-shockwave_time_ratios[i];
		vec2 disp_dir = normalize(gl_FragCoord.xy-pos);
		uv += 0.02*disp_dir*shockwave;
	}
	vec3 color = calcColor(uv);

	//color = texture(texture_ssao, v_uv).rrr;
	out_color = vec4(color, 1);
	//if(use_ao) out_color = vec4(vec3(ao),1);
}