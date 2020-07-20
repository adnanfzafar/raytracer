#include "CPUId.h"

CPUId::CPUId()
{
	updateCapabilities();
}

void CPUId::updateCapabilities()
{
	int reg[4];
	int numleaves = 0;

	SSE = false;
	SSE3 = false;

	// request feature page 0 (which returns the number of leafs in reg[0])
	__cpuid(&reg[0], 0x0);
	numleaves = reg[0];

	// as long as more than one leaf is supported, then our special extensions may exist
	if (numleaves >= 1)
	{
		// request leaf 1
		__cpuid(&reg[0], 0x1);

		// check for SSE
		SSE = reg[3] & (1 << 25);

		// check for SSE3
		SSE3 = reg[2] & (1 << 0);
	}
}