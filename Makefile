
gossip_vm: virtual_machine.o gossip.o garbage_collect.o
	gcc -o gossip_vm virtual_machine.o gossip.o garbage_collect.o

gossip.o: ./src/gossip.c ./include/virtual_machine.h
	gcc -c -I./include ./src/gossip.c

virtual_machine.o: ./src/virtual_machine.c ./include/virtual_machine.h
	gcc -c -I./include ./src/virtual_machine.c

garbage_collect.o: ./src/garbage_collect.c ./include/garbage_collect.h
	gcc -c -I./include ./src/garbage_collect.c

clean:
	rm *.o gossip_vm
