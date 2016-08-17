#pragma once



#include <string.h>




template<class P, class M>
size_t offsetof(const M P::*member) {
	return (size_t) &( reinterpret_cast<P*>(0)->*member);
}

/*template<class P, class M>
P* container_of(M* ptr, const M P::*member) {
	return (P*)( (char*)ptr - offsetof(member));
}*/

template<class P, class M>
P* container_of(M* ptr, unsigned char *member) {
	return (P*)((unsigned char*)ptr - member);
}


