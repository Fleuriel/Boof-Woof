// CustomMemoryAllocator.cpp
#include "MemoryAllocatorCustom.h"
#include <Jolt/Jolt.h>
#include <Jolt/Core/Memory.h>
#include <cstdlib> // For malloc, free
#include <cstdio>


namespace JPH {

    // Define the extern variables declared in Memory.h
    AllocateFunction Allocate = MyAllocate;
    ReallocateFunction Reallocate = MyReallocate;
    FreeFunction Free = MyFree;
    AlignedAllocateFunction AlignedAllocate = MyAlignedAllocate;
    AlignedFreeFunction AlignedFree = MyAlignedFree;

    // Implement the custom memory allocation functions
    void* MyAllocate(size_t inSize) {
        return std::malloc(inSize);
    }

    void* MyReallocate(void* inBlock, size_t inOldSize, size_t inNewSize) {
        // Note: inOldSize is not used in standard realloc
        return std::realloc(inBlock, inNewSize);
    }

    void MyFree(void* inBlock) {
        std::free(inBlock);
    }

    void* MyAlignedAllocate(size_t inSize, size_t inAlignment) {
        void* ptr = nullptr;
#if defined(_MSC_VER)
        ptr = _aligned_malloc(inSize, inAlignment);
        if (!ptr) {
            printf("Aligned allocation failed\n");
        }
#elif defined(__APPLE__) || defined(__linux__)
        if (posix_memalign(&ptr, inAlignment, inSize) != 0) {
            ptr = nullptr;
            printf("Aligned allocation failed\n");
        }
#else
        ptr = std::aligned_alloc(inAlignment, inSize);
        if (!ptr) {
            printf("Aligned allocation failed\n");
        }
#endif
        return ptr;
    }

    void MyAlignedFree(void* inBlock) {
#if defined(_MSC_VER)
        _aligned_free(inBlock);
#else
        std::free(inBlock);
#endif
    }

}

//namespace JPH {
//    // Custom allocate function
//    void* CustomAllocate(size_t inSize) {
//        return malloc(inSize); // Replace with your own allocator if needed
//    }
//
//    // Custom free function
//    void CustomFree(void* inBlock) {
//        free(inBlock); // Replace with your own deallocator if needed
//    }
//
//    // Register the custom allocator
//    void RegisterCustomAllocator() {
//        Allocate = CustomAllocate;
//        Free = CustomFree;
//    }
//}
