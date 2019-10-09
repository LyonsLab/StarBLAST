

CC=gcc
CCTOOLS_INCLUDE=/usr/include/cctools-7.0.18/cctools
CCTOOLS_LIB=/usr/lib/cctools-7.0.18

blast_workqueue-simple: blast_wq_simple.c
	$(CC) $< -g -o $@ -I$(CCTOOLS_INCLUDE) -L$(CCTOOLS_LIB) -lwork_queue -ldttools -lm -lz

.PHONY: clean
clean:
	rm blast_workqueue-simple

