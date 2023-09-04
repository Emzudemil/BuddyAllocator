#include <stdio.h>
#include "BuddyAllocator.h"
#include "stdlib.h"

BuddyAllocator* KernelBuddyAllocator;

UINT64 CalculateBuddyAllocSize(IN UINT64 Size)
{
	UINT64 allocSize = 0;
	UINT64 layerSize = SMALL_BLOCK_SIZE;
	while (layerSize < Size)
	{
		layerSize *= 2;
	}
	allocSize = layerSize / SMALL_BLOCK_SIZE * sizeof(BuddyAllocatorNode) + sizeof(BuddyAllocator);
	return allocSize;
}

UINT64 CalculateAmountOfBlocks(IN UINT64 Size)
{
	UINT64 amount = 0;
	UINT64 layerSize = SMALL_BLOCK_SIZE;
	while (layerSize < Size)
	{
		layerSize += SMALL_BLOCK_SIZE;
		++amount;
	}
	return amount;
}

UINT64 CalculateLayerSizeRoundUp(IN UINT64 Size)
{
	UINT64 layerSize = SMALL_BLOCK_SIZE;
	while (layerSize <= Size)
	{
		layerSize *= 2;
	}
	return layerSize;
}

UINT64 CalculateLargestBlock(IN UINT64 MemorySize)
{
	UINT64 layerSize = SMALL_BLOCK_SIZE;
	while (layerSize < MemorySize)
	{
		layerSize *= 2;
	}
	return layerSize;
}

void BuddyAllocatorInit(IN KERNEL_PARAMETERS* parameters)
{
	UINT64 memorySize = 0;
	for (int i = 0; i < parameters->MemoryMapSize; ++i)
	{
		memorySize += parameters->MemoryMap[i]->Length;
	}
	for (int i = 0; i < parameters->MemoryMapSize; ++i)
	{
		if (parameters->MemoryMap[i]->Type == MEMMAP_USABLE && parameters->MemoryMap[i]->Length >= sizeof(BuddyAllocator))
		{
			KernelBuddyAllocator = (BuddyAllocator*) parameters->MemoryMap[i]->Base;
			parameters->MemoryMap[i]->Base += sizeof(BuddyAllocator);
			parameters->MemoryMap[i]->Length -= sizeof(BuddyAllocator);
			printf("Buddy Allocator initialized at %p\n", KernelBuddyAllocator);
			break;
		}
		if (i == parameters->MemoryMapSize - 1)
		{
			printf("No suitable memory region found for buddy allocator\n");
			return;
		}
	}
	KernelBuddyAllocator->SmallestBlock = SMALL_BLOCK_SIZE;
	KernelBuddyAllocator->LargestBlock = CalculateLargestBlock(memorySize);;
	KernelBuddyAllocator->LayerCount = 1;
	if (KernelBuddyAllocator->LargestBlock == memorySize)
	{
		KernelBuddyAllocator->LargestBlockAmount = 1;
	} else
	{
		KernelBuddyAllocator->LargestBlockAmount = 2;
	}
	UINT64 layerSize = SMALL_BLOCK_SIZE;
	while (layerSize < memorySize)
	{
		layerSize *= 2;
		++KernelBuddyAllocator->LayerCount;
	}
	for (int i = 0; i < parameters->MemoryMapSize; ++i)
	{
		if (parameters->MemoryMap[i]->Type == MEMMAP_USABLE
			&& parameters->MemoryMap[i]->Length >= sizeof(BuddyAllocatorNode**) * KernelBuddyAllocator->LayerCount)
		{
			KernelBuddyAllocator->Layers = (BuddyAllocatorNode***) parameters->MemoryMap[i]->Base;
			parameters->MemoryMap[i]->Base += sizeof(BuddyAllocatorNode**) * KernelBuddyAllocator->LayerCount;
			parameters->MemoryMap[i]->Length -= sizeof(BuddyAllocatorNode**) * KernelBuddyAllocator->LayerCount;
			printf("Layers initialized at %p\n", KernelBuddyAllocator->Layers);
			break;
		}
	}
	UINT32 Amount = KernelBuddyAllocator->LargestBlockAmount;
	UINT32 BlockSize = KernelBuddyAllocator->LargestBlock;
	for (int i = 0; i < KernelBuddyAllocator->LayerCount; ++i, Amount *= 2, BlockSize /= 2)
	{
		for (int j = 0; j < parameters->MemoryMapSize; ++j)
		{
			if (parameters->MemoryMap[j]->Type == MEMMAP_USABLE && parameters->MemoryMap[j]->Length >=  sizeof(BuddyAllocatorNode*) + Amount * sizeof(BuddyAllocatorNode))
			{
				KernelBuddyAllocator->Layers[i] = (BuddyAllocatorNode**) parameters->MemoryMap[j]->Base;
				parameters->MemoryMap[j]->Base += sizeof(BuddyAllocatorNode*);
				parameters->MemoryMap[j]->Length -= sizeof(BuddyAllocatorNode*);
				for (int k = 0; k < Amount; ++k)
				{
					KernelBuddyAllocator->Layers[i][k] = (BuddyAllocatorNode*) parameters->MemoryMap[j]->Base;
					parameters->MemoryMap[j]->Base += sizeof(BuddyAllocatorNode);
					parameters->MemoryMap[j]->Length -= sizeof(BuddyAllocatorNode);
					KernelBuddyAllocator->Layers[i][k]->Address = (void *) parameters->MemoryMap[j]->Base;
					KernelBuddyAllocator->Layers[i][k]->Size = BlockSize;
					KernelBuddyAllocator->Layers[i][k]->IsFree = TRUE;
					KernelBuddyAllocator->Layers[i][k]->Index = k;
				}
				printf("Layer %d initialized at %p with the size %d\n", i, KernelBuddyAllocator->Layers[i], BlockSize);
				break;
			}
		}
	}
}