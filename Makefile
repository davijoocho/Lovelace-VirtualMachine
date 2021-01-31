
gossip_vm: virtual_machine.o gossip.o
	gcc -o gossip_vm virtual_machine.o gossip.o

gossip.o: ./src/gossip.c ./include/virtual_machine.h
	gcc -c -I./include ./src/gossip.c

virtual_machine.o: ./src/virtual_machine.c ./include/virtual_machine.h
	gcc -c -I./include ./src/virtual_machine.c

clean:
	rm *.o lovelace_vm
