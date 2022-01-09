#version 460 core

uniform sampler2D sampler;
void main() {
	vec2 coord = vec2(gl_FragCoord.x/1280.,gl_FragCoord.y/720.0 );

	gl_FragColor = texture2D(sampler, coord);
}