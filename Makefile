all: performance shipping

performance: performance.c
	gcc -pthread -o performance performance.c

shipping: shipping.c
	gcc -pthread -o shipping shipping.c

clean:
	rm -f performance shipping
