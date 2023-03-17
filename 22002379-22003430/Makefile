all: proctopk threadtopk
proctopk:
	gcc list.c topk.c proctopk.c -o proc_exec
threadtopk:
	gcc list.c topk.c threadtopk.c -o thread_exec
clean:
	rm -f proc_exec thread_exec out.txt