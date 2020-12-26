
lovelace_vm: virtual_machine.o lovelace.o
	gcc -o lovelace_vm virtual_machine.o lovelace.o

lovelace.o: ./src/lovelace.c ./include/virtual_machine.h
	gcc -c -I./include ./src/lovelace.c

virtual_machine.o: ./src/virtual_machine.c ./include/virtual_machine.h
	gcc -c -I./include ./src/virtual_machine.c

clean:
	rm *.o lovelace_vm
