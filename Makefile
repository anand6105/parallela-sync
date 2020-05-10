EPIPHANY_HOME=/opt/adapteva/esdk
#host compiler path
LCC=/opt/linaro/bin/arm-linux-gnueabihf-gcc
#device compiler path
CC=e-gcc
#FreeRTOS dependencies
CFLAGS=-I.
INCLUDES= -g -I.
DEPS = synchronize.h
DEPSHOST = synchronize.h
#Epiphany SDK dependencies
ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host.armv7l/lib
EINCS=${ESDK}/tools/host.armv7l/include
ELDF=${ESDK}/bsps/current/internal.ldf
EHDF=${EPIPHANY_HDF}

#main target
all: host epiphany.elf
	@echo build status : successful
run: host epiphany.elf
	@echo build status : successful

example1: host epiphany.elf
	@echo build status : successful

#rule for every device target
%.elf: $(ELDF) %.o
	$(CC) -g -T$< -Wl,--gc-sections -o $@ $(filter-out $<,$^) -le-lib

#host target
host: host.c $(DEPSHOST) 
	$(LCC) $< -D__HOST__ -o $@ -I ${EINCS} -L ${ELIBS} -lpal -le-hal -le-loader -lpthread 

#clean target
clean:
	rm -f *.o *.srec *.elf host

.SECONDARY:
%.o: %.c $(DEPS)
	$(CC) -fdata-sections -ffunction-sections  -c -D__DEVICE__ -o $@ $< $(INCLUDES)

%.o: %.s $(DEPS)
	$(CC) -c -o $@ $< $(INCLUDES)

