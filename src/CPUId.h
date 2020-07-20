#pragma once

#include <intrin.h>
/*
	Creates interfaces for checking hardware capabilities
*/

class CPUId {
public:
	bool SSE;
	bool SSE3;
	
	CPUId();
	void updateCapabilities();
};