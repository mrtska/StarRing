
#include <system.h>


void *__dso_handle;



extern "C" void __cxa_atexit() {


}

extern "C" void __cxa_pure_virtual() {


}


extern "C" void abort() {


}

extern "C" void *malloc(unsigned long size) {

	return nullptr;
}

extern "C" void free(void *obj) {


}

extern "C" int dl_iterate_phdr() {

	return 0;
}
