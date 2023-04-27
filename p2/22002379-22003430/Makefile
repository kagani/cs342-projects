all: mps mps_cv
mps:
	gcc burstitem.c readyqueue.c sched.c mps.c -o mps -lm
mps_cv:
	gcc burstitem.c readyqueue.c sched_cv.c mps.c -o mps_cv -lm
clean:
	rm -f mps mps_cv out.txt