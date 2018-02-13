# include "lat.h"
# include <stdio.h>
# include <malloc.h>
mdl_u64_t used = 0;
void* lat_mal(mdl_uint_t __size) {
	used+=__size;
	void *p = malloc(__size+sizeof(mdl_uint_t));
	*(mdl_uint_t*)p = __size;	
	return (void*)((mdl_u8_t*)p+sizeof(mdl_uint_t));
}

void lat_mfr(void *__p) {
	void *p = (void*)((mdl_u8_t*)__p-sizeof(mdl_uint_t));
	used-=*(mdl_uint_t*)p;
	free(p);
}

podp alloc_pod() {
	podp ret = lat_mal(sizeof(struct pod));
	podp *p = (ret->p = (podp*)lat_mal(0x100*sizeof(podp)));
	while(p != ret->p+0x100)
		*(p++) = NULL;
	ret->list = NULL;
	return ret;
}

mdl_uint_t mem_usage() {return used;}
void lat_prepare(latp __lat) {
	__lat->p = alloc_pod();
}

//# define DEBUG
void lat_put(latp __lat, mdl_u64_t __key, void *__p) {
	podp *p = __lat->p->p;
	if (!*(p = (p+((__key&0xff)^(__key>>8&0xff))))) {
		*p = alloc_pod();
# ifdef DEBUG
		fprintf(stdout, "pod 0 allocated.\n");
# endif
	}

	if (!*(p = (*p)->p+((__key>>8&0xff)^(__key>>16&0xff)))) {
		*p = alloc_pod();
# ifdef DEBUG
		fprintf(stdout, "pod 1 allocated.\n");
# endif
	}

	if (!*(p = (*p)->p+((__key>>16&0xff)^(__key>>24&0xff)))) {
		*p = alloc_pod();
# ifdef DEBUG
		fprintf(stdout, "pod 2 allocated.\n");
# endif
	}

	if (!*(p = (*p)->p+((__key>>24&0xff)^(__key>>32&0xff)))) {
		*p = alloc_pod();
# ifdef DEBUG
		fprintf(stdout, "pod 3 allocated.\n");
# endif
	}

	if (!*(p = (*p)->p+((__key>>32&0xff)^(__key>>40&0xff)))) {
		*p = alloc_pod();
# ifdef DEBUG
		fprintf(stdout, "pod 4 allocated.\n");
# endif
	}

	if (!*(p = (*p)->p+((__key>>40&0xff)^(__key>>48&0xff)))) {
		*p = alloc_pod();
# ifdef DEBUG
		fprintf(stdout, "pod 5 allocated.\n");
# endif
	}

	if (!*(p = (*p)->p+((__key>>48&0xff)^(__key>>56&0xff)))) {
		*p = alloc_pod();
# ifdef DEBUG
		fprintf(stdout, "pod 6 allocated.\n");
# endif
	}

	if (!*(p = (*p)->p+(__key>>56&0xff))) {
		*p = alloc_pod();
# ifdef DEBUG
		fprintf(stdout, "pod 7 allocated.\n");
# endif
	}

	recordp rec = lat_mal(sizeof(struct record));
	rec->next = (*p)->list;
	(*p)->list = rec;
	rec->key = __key;
	rec->p = __p;
}


void* lat_get(latp __lat, mdl_u64_t __key) {
	podp *p = __lat->p->p;
	if (!*(p = (p+((__key&0xff)^(__key>>8&0xff))))) {
		fprintf(stderr, "pod 0 failure.\n");
		return NULL;
	}
    
	if (!*(p = (*p)->p+((__key>>8&0xff)^(__key>>16&0xff)))) {
		fprintf(stderr, "pod 1 failure.\n");
		return NULL;
	}
    
	if (!*(p = (*p)->p+((__key>>16&0xff)^(__key>>24&0xff)))) {
		fprintf(stderr, "pod 2 failure.\n");
		return NULL;
	}
    
	if (!*(p = (*p)->p+((__key>>24&0xff)^(__key>>32&0xff)))) {
		fprintf(stderr, "pod 3 failure.\n");
		return NULL;
	}
    
	if (!*(p = (*p)->p+((__key>>32&0xff)^(__key>>40&0xff)))) {
		fprintf(stderr, "pod 4 failure.\n");
		return NULL;
	}
    
	if (!*(p = (*p)->p+((__key>>40&0xff)^(__key>>48&0xff)))) {
		fprintf(stderr, "pod 5 failure.\n");
		return NULL;
	}
    
	if (!*(p = (*p)->p+((__key>>48&0xff)^(__key>>56&0xff)))) {
		fprintf(stderr, "pod 6 failure.\n");
		return NULL;
	}
    
	if (!*(p = (*p)->p+(__key>>56&0xff))) {
		fprintf(stderr, "pod 7 failure.\n");
		return NULL;
	}

	recordp rec = (*p)->list;
	while(rec != NULL) {
		if (rec->key == __key) return rec->p;
		rec = rec->next;
	}
	
	fprintf(stderr, "not found.\n");
	return NULL;
}

void free_pod(podp __pod) {
	podp *p = __pod->p;
	while(p != __pod->p+0x100) {
		if (*p != NULL) {
			free_pod(*p);
			lat_mfr(*p);
		}
		p++;
	}

	recordp rec = __pod->list, bk;
	while(rec != NULL) {
		bk = rec;	
		rec = rec->next;
		lat_mfr(bk);
	}

	lat_mfr(__pod->p);
}

void lat_free(latp __lat) {
	free_pod(__lat->p);
	lat_mfr(__lat->p);
}
