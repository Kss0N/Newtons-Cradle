#include "AllocatorCallback.h"
#include <memory>


void* AllocatorCallback::allocate
(size_t size, const char* typeName, const char* filename, int line) {
	return _aligned_malloc(size, 16);
}
void AllocatorCallback::deallocate(void* ptr) {
	_aligned_free(ptr);
}
AllocatorCallback::~AllocatorCallback() {}