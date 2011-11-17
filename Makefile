################################
##############ZeOS #############
################################
########## Makefile ############
################################

# package dev86 is required
AS86	= as86 -0 -a
LD86	= ld86 -0

HOSTCFLAGS = -Wall -Wstrict-prototypes
HOSTCC 	= gcc -m32
CC      = gcc -m32
AS      = as --32
LD      = ld -m elf_i386
OBJCOPY = objcopy -O binary -R .note -R .comment -S

INCLUDEDIR = include

# Define here flags to compile the tests if needed
JP = 

CFLAGS = -O2  -g $(JP) -ffreestanding -Wall -I$(INCLUDEDIR) 
ASMFLAGS = -I$(INCLUDEDIR)
SYSLDFLAGS = -T system.lds
USRLDFLAGS = -T user.lds
LINKFLAGS = -g 

SYSOBJ = interrupt.o mm.o channel.o circbuff.o fat.o entry.o io.o kernutil.o sys.o sched.o sem.o devices.o utils.o hardware.o

#add to USROBJ the object files required to complete the user program
USROBJ = libc.o libjp.a kernutil.o

all:zeos.bin

zeos.bin: bootsect system build user
	$(OBJCOPY) system system.out
	$(OBJCOPY) user user.out
	./build bootsect system.out user.out > zeos.bin

build: build.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

bootsect: bootsect.o
	$(LD86) -s -o $@ $<

bootsect.o: bootsect.s
	$(AS86) -o $@ $<

bootsect.s: bootsect.S Makefile
	$(CPP) $(ASMFLAGS) -traditional $< -o $@

entry.s: entry.S $(INCLUDEDIR)/asm.h $(INCLUDEDIR)/segment.h
	$(CPP) $(ASMFLAGS) -o $@ $<

user.o: user.c $(INCLUDEDIR)/libc.h

interrupt.o: interrupt.c $(INCLUDEDIR)/interrupt.h $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/mm.h $(INCLUDEDIR)/types.h

io.o: io.c $(INCLUDEDIR)/io.h

kernutil.o: kernutil.c $(INCLUDEDIR)/kernutil.h

libc.o: libc.c $(INCLUDEDIR)/libc.h 

mm.o: mm.c $(INCLUDEDIR)/types.h $(INCLUDEDIR)/mm.h

sched.o: sched.c $(INCLUDEDIR)/sched.h

sem.o: sem.c $(INCLUDEDIR)/sem.h

sys.o: sys.c $(INCLUDEDIR)/devices.h $(INCLUDEDIR)/mm.h

channel.o: channel.c $(INCLUDEDIR)/channel.h

circbuff.o: circbuff.c $(INCLUDEDIR)/circbuff.h

fat.o: fat.c $(INCLUDEDIR)/fat.h

utils.o: utils.c $(INCLUDEDIR)/utils.h

system.o: system.c $(INCLUDEDIR)/hardware.h system.lds $(SYSOBJ) $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/types.h $(INCLUDEDIR)/interrupt.h $(INCLUDEDIR)/system.h $(INCLUDEDIR)/sched.h $(INCLUDEDIR)/mm.h $(INCLUDEDIR)/io.h $(INCLUDEDIR)/sem.h $(INCLUDEDIR)/mm_address.h 

system: system.o system.lds $(SYSOBJ)
	$(LD) $(LINKFLAGS) $(SYSLDFLAGS) -o $@ $< $(SYSOBJ) 

user: user.o user.lds $(USROBJ) 
	$(LD) $(LINKFLAGS) $(USRLDFLAGS) -o $@ $< $(USROBJ)

clean:
	rm -f *.o *.s bochsout.txt parport.out system.out system bootsect zeos.bin user user.out *~ build

disk: zeos.bin
	dd if=zeos.bin of=/dev/fd0

emul24: zeos.bin
	bochs -q -f bochsrc-2.4

emul: zeos.bin
	bochs -q -f bochsrc-2.3

debug: zeos.bin
	bochs -q -f bochsrc-debug

emuldbg: zeos.bin
	bochs_nogdb -q -f .bochsrc-2.4

sim:
	make clean
	make
	make emul

kill:
	killall -9 bochs-bin
