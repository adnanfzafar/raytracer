#pragma once

typedef struct {
	//int type;
	float ka;	// ambient reflection coefficient
	float kd;	// diffuse coefficient
	float n;	// Phong specular reflection exponent
	float ks;	// material specular reflection coefficient
} material_s;