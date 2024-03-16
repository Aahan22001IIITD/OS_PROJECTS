all:
	gcc -o p1 p1.c
	gcc -o p2 p2.c
	gcc -o p3 p3.c
	gcc -o p4 p4.c
	gcc -o p5 p5.c
	gcc -o p6 p6.c
	gcc -o os os.c -lrt -lpthread
run:
	gcc -o p1 p1.c
	gcc -o p2 p2.c
	gcc -o p3 p3.c
	gcc -o p4 p4.c
	gcc -o p5 p5.c
	gcc -o p6 p6.c
	gcc -o os os.c -lrt -lpthread
	./os 3 3
clean:
	rm -f p1 p2 p3 p4 p5 p6 os

	