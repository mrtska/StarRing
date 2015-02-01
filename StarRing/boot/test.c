/*
 * test.c
 *
 *  Created on: 2015/01/12
 *      Author: StarRing
 */

#include <system.h>
#include <page/page.h>

void a(void) {

	write_cr3(VIRTUAL_ADDRESS_TO_PHYSICAL_ADDRESS(kernel_pml4));
}
