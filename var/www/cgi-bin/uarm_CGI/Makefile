#CROSS_COMPILE=arm-xilinx-linux-gnueabi-
all:
	$(CROSS_COMPILE)gcc -c error.c
	$(CROSS_COMPILE)gcc -c uarm_move.c zrarm.c iic.c uarm.c error.o -lm
	$(CROSS_COMPILE)gcc -o ./uarm_move.cgi ./uarm_move.o ./zrarm.o iic.o uarm.o error.o -lm
install:
	cp -a ./*.cgi $(INSTALL_PATH) 
clean:
	rm ./*.cgi ./*.o
