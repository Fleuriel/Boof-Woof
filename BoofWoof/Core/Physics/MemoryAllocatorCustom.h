// MemoryAllocatorCustom.h

#pragma once

#include <cstddef>

namespace JPH {

	// Custom memory allocation functions
	void* MyAllocate(size_t inSize);
	void* MyReallocate(void* inBlock, size_t inOldSize, size_t inNewSize);
	void MyFree(void* inBlock);
	void* MyAlignedAllocate(size_t inSize, size_t inAlignment);
	void MyAlignedFree(void* inBlock);

}