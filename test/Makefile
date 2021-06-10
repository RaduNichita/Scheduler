CC= cl
CFLAGS=/nologo /W3 /EHsc /D_CRT_SECURE_NO_DEPRECATE /DDLL_EXPORTS
L=link
LIB_NAME=libscheduler.dll
INCLUDE_DIR=include
WRAPPERS=wrappers
DS_DIR=data_structures
UTILS_DIR=utils
OBJS = priority_queue.obj comparators.obj vector.obj so_scheduler.obj utils.obj so_thread_wrapper_win.obj

build: libscheduler.lib

libscheduler.lib: libscheduler.dll

libscheduler.dll: $(OBJS)
	$(L) /dll /out:$@ /implib:libscheduler.lib $**

comparators.obj: $(UTILS_DIR)/comparators.c
	$(CC) $(CFLAGS) /c /I$(INCLUDE_DIR) /Fo$@ /c $**

so_scheduler.obj: so_scheduler.c
	$(CC) $(CFLAGS) /c -I$(INCLUDE_DIR) /Fo$@ /c $**

priority_queue.obj: $(DS_DIR)/priority_queue.c
	$(CC) $(CFLAGS) /I$(INCLUDE_DIR) /Fo$@ /c $**

so_thread_wrapper_win.obj: $(WRAPPERS)/win/so_thread_wrapper_win.c
	$(CC) $(CFLAGS) /I$(INCLUDE_DIR) /Fo$@ /c $**

vector.obj: $(DS_DIR)/vector.c
	$(CC) $(CFLAGS) /c /I$(INCLUDE_DIR) /Fo$@ /c $**

utils.obj: $(UTILS_DIR)/utils.c
	$(CC) $(CFLAGS) /I$(INCLUDE_DIR) /Fo$@ /c $**

pack:
	zip -FSr 331CA_Tema4_NichitaRadu.zip so_scheduler.c README.md Makefile data_structures/ wrappers/ utils/ include/

clean:
	del /f $(LIB_NAME) *.obj *.dll *.exp *.lib

.PHONY: clean pack
