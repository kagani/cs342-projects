all: proctopk threadtopk
proctopk:
	gcc list.c topk.c proctopk.c -o proctopk
threadtopk:
	gcc list.c topk.c threadtopk.c -o threadtopk
clean:
	rm -f proctopk threadtopk out.txt