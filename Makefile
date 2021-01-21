
PMan: PMan.o pid_list.o
	gcc PMan.o pid_list.o -lreadline -o PMan

PMan.o: PMan.c
	gcc -c PMan.c

pid_list.o: pid_list.c pid_list.h
	gcc -c pid_list.c

clean:
	rm *.o PMan
