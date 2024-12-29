CPPFLAGS=-fPIC -O3

SOURCES=DCRemove.cpp FIRFilter.cpp ImpulseGen.cpp ImpulseGenVCtl.cpp \
ImpulseSynth.cpp LowPass.cpp LPC.cpp PoleFilter.cpp SincTable.cpp \
Vocoder.cpp VocoderSynth.cpp

OBJECTS=$(SOURCES:.cpp=.o)

all: VocoderSynth.so VocoderSynthUI.so

VocoderSynth.so:$(OBJECTS)
	g++ -shared -o VocoderSynth.so $(OBJECTS) -lm

LIBXPUTTY=libxputty/libxputty


VocoderSynthUI.so:VocoderSynthUI.cpp $(LIBXPUTTY)/libxputty.a
	g++ -shared -fPIC -o VocoderSynthUI.so \
	`pkg-config --cflags lv2 cairo x11` \
	-I$(LIBXPUTTY)/include \
	VocoderSynthUI.cpp \
	`pkg-config --libs lv2 cairo x11` \
	-L$(LIBXPUTTY) -lxputty

libxputty/Makefile:
	git submodule update --init --recursive
	
$(LIBXPUTTY)/libxputty.a:libxputty/Makefile
	make -C libxputty

install:VocoderSynth.lv2 all
	cp manifest.ttl VocoderSynth.lv2
	cp VocoderSynth.ttl VocoderSynth.lv2
	cp VocoderSynth.so VocoderSynth.lv2
	cp VocoderSynthUI.so VocoderSynth.lv2
	cp --recursive VocoderSynth.lv2 ~/.lv2

VocoderSynth.lv2:
	mkdir VocoderSynth.lv2







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
