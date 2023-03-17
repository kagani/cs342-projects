all:
	gcc list.c topk.c proctopk.c -o proc_exec
	gcc list.c topk.c threadtopk.c -o thread_exec
proctopk:
	gcc list.c topk.c proctopk.c -o proc_exec
threadtopk:
	gcc list.c topk.c threadtopk.c -o thread_exec
clean:
	rm proc_exec thread_exec out.txt