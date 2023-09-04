#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "BuddyAllocator.h"
#include "Kernel.h"

MEMORY_DESCRIPTOR** generateMemoryDescriptor()
{
	MEMORY_DESCRIPTOR** descriptor = malloc(sizeof(MEMORY_DESCRIPTOR*) * 20);
	for (int i = 0; i < 20; ++i)
	{
		int size = 4096 + (((float )rand() / RAND_MAX) * 32768);
		descriptor[i] = malloc(sizeof(MEMORY_DESCRIPTOR));
		descriptor[i]->Base = (UINT64) malloc(size);
		descriptor[i]->Length = size;
		descriptor[i]->Type = (((float) rand() / RAND_MAX) * 10) < 6.5 ? MEMMAP_USABLE : MEMMAP_RESERVED;
	}
	return descriptor;
}

int main()
{
	srand(time(0));
	MEMORY_DESCRIPTOR** descriptor = generateMemoryDescriptor();
	KERNEL_PARAMETERS* parameters = malloc(sizeof(KERNEL_PARAMETERS));
	parameters->MemoryMap = descriptor;
	parameters->MemoryMapSize = 20;
	parameters->CommandLine = NULL;
	parameters->CommandLineSize = 0;

	int memorySize = 0;
	for (int i = 0; i < 20; ++i)
	{
		memorySize += descriptor[i]->Length;
	}
	BuddyAllocatorInit(parameters);
	printf("Memory Size: %d\n", memorySize);
	printf("Buddy Allocator Size: %llu\n", CalculateBuddyAllocSize(memorySize));
	printf("Free Memory: %llu\n", memorySize - CalculateBuddyAllocSize(memorySize));
	printf("Tmp: %lu", sizeof(BuddyAllocatorNode));
	return 0;
}
