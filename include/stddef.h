#pragma once







template<class P, class M>
unsigned long offsetof(const M P::*member) {
	return (unsigned long) &( reinterpret_cast<P*>(0)->*member);
}

/*template<class P, class M>
P* container_of(M* ptr, const M P::*member) {
	return (P*)( (char*)ptr - offsetof(member));
}*/

template<class P, class M>
P* container_of(M* ptr, unsigned char *member) {
	return (P*)((unsigned char*)ptr - member);
}


