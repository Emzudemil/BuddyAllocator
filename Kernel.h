#pragma once

#include "Base.h"

#define MEMMAP_USABLE                 0
#define MEMMAP_RESERVED               1
#define MEMMAP_ACPI_RECLAIMABLE       2
#define MEMMAP_ACPI_NVS               3
#define MEMMAP_BAD_MEMORY             4
#define MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define MEMMAP_KERNEL_AND_MODULES     6
#define MEMMAP_FRAMEBUFFER            7

typedef struct {
	UINT64 Base;
	UINT64 Length;
	UINT64 Type;
} MEMORY_DESCRIPTOR;

typedef struct {
	// Program Parameters
	UINTN               CommandLineSize;
	CHAR8**             CommandLine;

	// Memory Map
	UINTN               MemoryMapSize;
	MEMORY_DESCRIPTOR** MemoryMap;
} KERNEL_PARAMETERS;