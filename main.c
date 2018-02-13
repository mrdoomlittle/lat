# include "lat.h"
# include <stdio.h>
# include <unistd.h>
# include <signal.h>
# include <time.h>
mdl_u8_t lock = 0;
void ctrl_c(int __sig) {
	 lock = -1;
}

int main(void) {
	signal(SIGINT, ctrl_c);
	struct lat ts;
	lat_prepare(&ts);

	double fi = 0.00;
	mdl_u64_t const n = 0xffff, start = 0;
	mdl_u64_t i = start;

	while(i != n && !lock) {
		lat_put(&ts, i, (void*)i);

		if (fi > 10) {
			printf("mem_usage: %lf mb\n", (double)mem_usage()*0.000001);
			fi = 0;
		} else
			fi+= (double)i*((double)n/100.0);
		i++;
	}

	double lookup_time = 0.0;
	struct timespec begin, end;

	i = start;
	while(i != n && !lock) {
		void *p;
		clock_gettime(CLOCK_MONOTONIC, &begin);
		p = lat_get(&ts, i);
		clock_gettime(CLOCK_MONOTONIC, &end);
		lookup_time+= (end.tv_nsec-begin.tv_nsec)+((end.tv_sec-begin.tv_sec)*1000000000.0);	

		if (i != (mdl_u64_t)p)
			fprintf(stderr, "mismatch.\n");
		i++;
	}

	fprintf(stdout, "\nlookup time: %lfsec\n", lookup_time/1000000000.0);
	fprintf(stdout, "records: %lu, mem_used: %lf mb\n", n, (double)mem_usage()*0.000001);

	lat_free(&ts);
	printf("all freed? %s, mem_usage: %u\n", !mem_usage()?"yes":"no", mem_usage());
}
