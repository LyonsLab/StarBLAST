
CC=gcc
CFLAGS=-g -std=c11 -Wall -Werror
#CCTOOLS_INCLUDE=/usr/include/cctools-7.0.18/cctools/
#CCTOOLS_LIB=/usr/lib/cctools-7.0.18/
CCTOOLS_INCLUDE=/opt/cctools-7.0.19-x86_64-centos7/include/cctools
CCTOOLS_LIB=/opt/cctools-7.0.19-x86_64-centos7/lib
LIB=-lwork_queue -ldttools -lm -lz

CUTEST_INCLUDE=../cutest-1.5

.PHONY: all
all: blast_workqueue blast_workqueue-backend

.PHONY: release
release: CFLAGS=-O3 -std=c11 -Wall -Werror -Wextra
release: blast_workqueue blast_workqueue-backend

.PHONY: install
install: blast_workqueue blast_workqueue-backend
	mv blast_workqueue /usr/bin/
	mv blast_workqueue-backend /usr/bin/

.PHONY: simple
simple: blast_workqueue-simple

blast_workqueue-simple: blast_wq_simple.c
	$(CC) $^ $(CFLAGS) -o $@ -I$(CCTOOLS_INCLUDE) -L$(CCTOOLS_LIB) $(LIB) 

blast_workqueue: blast_workqueue.o socket_comm.o
	$(CC) $^ $(CFLAGS) -o $@ -I$(CCTOOLS_INCLUDE) -L$(CCTOOLS_LIB) $(LIB)

blast_workqueue-backend: blast_workqueue_backend.o socket_comm.o
	$(CC) $^ $(CFLAGS) -o $@ -I$(CCTOOLS_INCLUDE) -L$(CCTOOLS_LIB) $(LIB)

blast_workqueue.o: blast_workqueue.c
	$(CC) $^ $(CFLAGS) -c -o $@ -I$(CCTOOLS_INCLUDE) -L$(CCTOOLS_LIB) $(LIB)

blast_workqueue_backend.o: blast_workqueue_backend.c
	$(CC) $< $(CFLAGS) -c -o $@ -I$(CCTOOLS_INCLUDE) -L$(CCTOOLS_LIB) $(LIB)

socket_comm.o: socket_comm.c
	$(CC) $< $(CFLAGS) -c -o $@ -I$(CCTOOLS_INCLUDE) -L$(CCTOOLS_LIB) $(LIB)

.PHONY: test
test: CFLAGS=-g -std=c11 -Wall -Werror -Wextra -DUNIT_TEST -I$(CUTEST_INCLUDE)
test: test.o CuTest.o socket_comm.o blast_workqueue.o
	$(CC) $^ $(CFLAGS) -o $@ -I$(CCTOOLS_INCLUDE) -L$(CCTOOLS_LIB) $(LIB)

test.o: test.c
	$(CC) $< $(CFLAGS) -c -o $@
CuTest.o: $(CUTEST_INCLUDE)/CuTest.c
	$(CC) $< $(CFLAGS) -c -o $@

.PHONY: clean
clean:
	rm -f blast_workqueue-simple blast_workqueue blast_workqueue-backend test *.o

