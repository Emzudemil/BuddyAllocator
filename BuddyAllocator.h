#pragma once

#include "Kernel.h"

#define SMALL_BLOCK_SIZE 4096

typedef struct BuddyAllocatorNode {
	void* Address;
	UINT32 Size;
	BOOLEAN IsFree;
	UINT16 Index;
} BuddyAllocatorNode;

typedef struct {
	 UINT32 SmallestBlock;
	 UINT32 LargestBlock;
	 UINT8 LargestBlockAmount;
	 UINT16 LayerCount;
	 BuddyAllocatorNode*** Layers;
} BuddyAllocator;

extern BuddyAllocator* KernelBuddyAllocator;

void BuddyAllocatorInit(IN KERNEL_PARAMETERS* parameters);

void DeleteAllocator();

void Allocate(IN UINT64 size, OUT void** address);

void Free(IN void* address);

/**
 * @brief Calculates the size of the memory needed to allocate a buddy allocator
 * @param Size The total memory size
 * @return The size of the memory needed to allocate a buddy allocator
 */
UINT64 CalculateBuddyAllocSize(IN UINT64 Size);

UINT64 CalculateAmountOfBlocks(IN UINT64 Size);