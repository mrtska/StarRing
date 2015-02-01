/*
[Module sys.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2015/01/29
*/


#include <system.h>
#include <apic.h>
#include <utsname.h>
#include <uid.h>






long new_name(struct new_utsname *name) {

	strcpy(name->domainname, "StarRing");
	strcpy(name->machine, "x86_64");
	strcpy(name->release, "3.0.0");
	strcpy(name->sysname, "StarRing");
	strcpy(name->version, "3.0.0");
	strcpy(name->nodename, "StarRing");
	return 0;
}



unsigned int getuid(void) {

	return 0;
}

unsigned int getgid(void) {

	return 0;
}

unsigned int geteuid(void) {

	return 0;
}

unsigned int getegid(void) {

	return 0;
}
