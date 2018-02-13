# ifndef __lat__h
# define __lat__h
# include <mdlint.h>
typedef struct record {
	struct record *next;
	mdl_u64_t key;
	void *p;
} *recordp;

typedef struct pod {
	recordp list;	
	struct pod **p;
} *podp;

typedef struct lat {
	podp p;	
} *latp;

mdl_uint_t mem_usage();
void lat_prepare(latp);
void lat_put(latp, mdl_u64_t, void*);
void* lat_get(latp, mdl_u64_t);
void lat_free(latp);
# endif /*__lat__h*/
