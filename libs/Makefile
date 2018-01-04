
#Name of this library:
LIBNAME=projectlib
LIBRARY=$(LIBNAME).lib


#Which files need to be used for this library:
SOURCES=stm8s_gpio.c
SOURCES+=stm8s_awu.c
SOURCES+=stm8s_tim1.c
SOURCES+=stm8s_clk.c
SOURCES+=stm8s_adc1.c
#SOURCES+= delay.c
SOURCES+= HT1621.c
SOURCES+= awu.c

HEADERS= HT1621.h

RELS=$(SOURCES:.c=.rel)


SDCC=sdcc
SDLD=sdld
SDAR=sdar

PROCESSOR=stm8s103f3
DEBUGPROBE=stlinkv2

# Define tool path. Adjust to your setup:
LIBROOT=/home/onat/elektronik/ARM/stm8/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver

INCLUDES=$(LIBROOT)/inc/
SOURCEDIR=$(LIBROOT)/src/

CFLAGSLIB= -I$(INCLUDES)
CFLAGSLIB+= -I.
CFLAGSLIB+= -I..
CFLAGSLIB+= -c

SDARFLAGS= -rc

#The macros are to keep the header files (which want to see a specific compiler and processor) happy.
MACROS= -D__CSMC__ -DSTM8S103
PROCTYPE= -lstm8 -mstm8

vpath %.c
vpath %.c $(SOURCEDIR)


#.phony all # all is not a file name.

all: $(LIBRARY)

$(LIBRARY) : $(SOURCES) # $(HEADERS)


%.lib : $(RELS)  
	$(SDAR) $(SDARFLAGS) $@ $(RELS)

%.rel: %.c 
	$(SDCC) $(MACROS) $(PROCTYPE) $(CFLAGSLIB) $(LDFLAGS) $<

clean:
	rm -f $(RELS) $(RELS:.rel=.cdb) $(RELS:.rel=.map) $(RELS:.rel=.lst) $(RELS:.rel=.asm) $(RELS:.rel=.lk) $(RELS:.rel=.rst) $(RELS:.rel=.sym)


