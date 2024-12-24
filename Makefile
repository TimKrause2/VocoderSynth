CPPFLAGS=-fPIC -O3

SOURCES=DCRemove.cpp FIRFilter.cpp ImpulseGen.cpp ImpulseGenVCtl.cpp \
ImpulseSynth.cpp LowPass.cpp LPC.cpp PoleFilter.cpp SincTable.cpp \
Vocoder.cpp VocoderSynth.cpp

OBJECTS=$(SOURCES:.cpp=.o)

all: VocoderSynth.so libxputty/Build/libxputty.a

VocoderSynth.so:$(OBJECTS)
	gcc -shared -o VocoderSynth.so $(OBJECTS) -lm

xputty:libxputty/Makefile
	git submodule update --init --recursive
	
libxputty/Build/libxputty.a:xputty
	make -C libxputty

DCRemove.o:DCRemove.cpp

FIRFilter.o:FIRFilter.cpp

ImpulseGen.o:ImpulseGen.cpp

ImpulseGenVCtl.o:ImpulseGenVCtl.cpp

ImpulseSynth.o:ImpulseSynth.cpp

LowPass.o:LowPass.cpp

LPC.o:LPC.cpp

PoleFilter.o:PoleFilter.cpp

SincTable.o:SincTable.cpp

Vocoder.o:Vocoder.cpp

VocoderSynth.o:VocoderSynth.cpp
