
#include <arch/apic.h>
#include <arch/cpuid.h>
#include <util/kprintf.h>

namespace starring::arch {


    void apic::init() {


        util::kputs("APIC initialization.");
        util::kputs("has x2APIC.");
        util::kputs(cpuid::has(CPUID_FEAT_ECX_x2APIC));
        
    }





}