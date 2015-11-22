#xjilec00, ISA projekt 1, VUT FIT 2015, Makefile

MAKEFILE      = Makefile
CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -Wall -W -
CXXFLAGS      = -std=c++0x -Wall -W
LINK          = g++
LIBS          = -L/usr/local/lib -lssl -lcrypto -lxml2 -I/usr/include/libxml2 -lm 

OBJECTS_DIR   = ./

SOURCES       = main.cpp \
		xmlviewer.cpp \
		cmdArguments.cpp \
		gadgets.cpp \
		datadownloader.cpp 
OBJECTS       = main.o \
		xmlviewer.o \
		cmdArguments.o \
		gadgets.o \
		datadownloader.o
TARGET        = arfeed


first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)


####### Compile

main.o: main.cpp cmdArguments.hpp \
		gadgets.h \
		shared.h \
		datadownloader.hpp \
		xmlviewer.h
	$(CXX) -c $(CXXFLAGS) $(LIBS) -o main.o main.cpp

xmlviewer.o: xmlviewer.cpp xmlviewer.h \
		shared.h \
		cmdArguments.hpp \
		gadgets.h
	$(CXX) -c $(CXXFLAGS) $(LIBS) -o xmlviewer.o xmlviewer.cpp

cmdArguments.o: cmdArguments.cpp cmdArguments.hpp \
		gadgets.h \
		shared.h
	$(CXX) -c $(CXXFLAGS) -o cmdArguments.o cmdArguments.cpp

gadgets.o: gadgets.cpp gadgets.h \
		shared.h
	$(CXX) -c $(CXXFLAGS) -o gadgets.o gadgets.cpp

datadownloader.o: datadownloader.cpp datadownloader.hpp \
		shared.h \
		gadgets.h \
		cmdArguments.hpp \
		xmlviewer.h
	$(CXX) -c $(CXXFLAGS) $(LIBS) -o datadownloader.o datadownloader.cpp

clean:
	rm arfeed *.o

