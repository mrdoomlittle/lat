# include "lat.h"
# include <stdio.h>
# include <unistd.h>
# include <signal.h>
# include <time.h>
# include <string.h>
# include <stdlib.h>
mdl_u8_t lock = 0;
void ctrl_c(int __sig) {
	 lock = -1;
}

int main(int __argc, char const *__argv[]) {
	if (__argc < 2) return -1;

	signal(SIGINT, ctrl_c);
	struct lat ts;
	lat_prepare(&ts);

	double period = 0.00;
	double fi = 0.00;
	mdl_u64_t n = strtol(__argv[1], NULL, 10), start = 0;
	mdl_u64_t i = start;

	while(i != n && !lock) {
		lat_put(&ts, i, (void*)i);

		if (period > 0.01) {
			fi+=period;
			printf("mem_usage: %lf mb, finished: %lf\n", (double)mem_usage()*0.000001, fi);
			period = 0;
		}

		period+= (100.0/(double)n);
		usleep(10);
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

	lookup_time /= (double)n;
	fprintf(stdout, "\nlookup time: %lfsec\n", lookup_time/1000000000.0);
	fprintf(stdout, "records: %lu, mem_used: %lf mb\n", n, (double)mem_usage()*0.000001);

	lat_free(&ts);
	printf("all freed? %s, mem_usage: %u\n", !mem_usage()?"yes":"no", mem_usage());
}
