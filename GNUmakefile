CC= gcc
CFLAGS= -fPIC -Wall -Wextra
LDFLAGS=-shared
LIB=libscheduler.so
DS_DIR=data_structures
INCLUDE_DIR=include
WRAPPERS=wrappers
UTILS_DIR=utils
OBJS=priority_queue.o comparators.o vector.o so_scheduler.o utils.o so_thread_wrapper_lin.o


all: libscheduler.so

libscheduler.so: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(LIB)

comparators.o: $(UTILS_DIR)/comparators.c $(INCLUDE_DIR)/comparators.h
	$(CC) $(CFLAGS) -c -I$(INCLUDE_DIR) $<

so_scheduler.o: so_scheduler.c $(INCLUDE_DIR)/so_scheduler.h $(INCLUDE_DIR)/so_thread.h
	$(CC) $(CFLAGS) -c -I$(INCLUDE_DIR) $<

priority_queue.o:  $(DS_DIR)/priority_queue.c $(INCLUDE_DIR)/priority_queue.h $(INCLUDE_DIR)/vector.h
	$(CC) $(CFLAGS) -c -I$(INCLUDE_DIR) $<

so_thread_wrapper_lin.o: $(WRAPPERS)/lin/so_thread_wrapper_lin.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $<

vector.o:  $(DS_DIR)/vector.c $(INCLUDE_DIR)/vector.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $<

utils.o: $(UTILS_DIR)/utils.c $(INCLUDE_DIR)/utils.h
	$(CC) $(CFLAGS) -c -I$(INCLUDE_DIR) $<

pack:
	zip -FSr 331CA_Tema4_NichitaRadu.zip so_scheduler.c README.md Makefile GNUmakefile data_structures/ wrappers/ utils/ include/

clean:
	rm *.o *.so

.PHONY: clean
