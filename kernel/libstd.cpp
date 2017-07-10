#include <starring.h>
#include <util/kprintf.h>

extern "C" void __stack_chk_fail() {

    starring::util::kputs("stack check failed.");
    STOP;
}


extern "C" void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function) {

    starring::util::kputs("assersion failed.");
    STOP;
}

extern "C" int dl_iterate_phdr(int (*callback) (void *info, unsigned int size, void *data), void *data) {
    
    starring::util::kputs("call dl_iterate_phdr.");
    STOP;
    return 0;
}

extern "C" void abort() {

    starring::util::kputs("abort called.");
    STOP;
}

extern "C" void free(void *ptr) {

    starring::util::kputs("free called.");
    STOP;
}

extern "C" void *malloc(unsigned int size) {

    starring::util::kputs("malloc called.");
    STOP;
    return nullptr;
}

extern "C" void *realloc(void *ptr, unsigned int size) {

    starring::util::kputs("realloc called.");
    STOP;
    return nullptr;
}


extern "C" void *calloc(unsigned int count, unsigned int size) {

    starring::util::kputs("calloc called.");
    STOP;
    return nullptr;
}
