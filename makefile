#############################################################################
# Makefile for MDC
#############################################################################

####### Compiler, tools and options

CC	=	g++
CFLAGS	=	-O2 -D_GNU_SOURCE
INCPATH	=	
LINK	=	g++
LFLAGS	=	-ldl -t
LIBS	=	

####### Directories

OBJ  = obj
SOURCE = source

####### Files

OBJECTS =	$(OBJ)/algorithm.o \
		$(OBJ)/clustering.o \
		$(OBJ)/cooccurrence.o \
		$(OBJ)/jensen_shannon.o \
		$(OBJ)/main.o \
		$(OBJ)/neighborhood.o \
		$(OBJ)/node.o \
		$(OBJ)/read_ini.o \
		$(OBJ)/read_input.o
TARGET	=	bin/mdc

####### Implicit rules

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all:	$(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET) $(INCPATH) $(OBJECTS) $(LIBS)

clean:
	-rm -f $(OBJECTS) $(TARGET)

####### Compile

$(OBJ)/algorithm.o: $(SOURCE)/algorithm.cpp
	$(CC) -c $(SOURCE)/algorithm.cpp $(CFLAGS) $(INCPATH) -o $@

$(OBJ)/clustering.o: $(SOURCE)/clustering.cpp
	$(CC) -c $(SOURCE)/clustering.cpp $(CFLAGS) $(INCPATH) -o $@

$(OBJ)/cooccurrence.o: $(SOURCE)/cooccurrence.cpp
	$(CC) -c $(SOURCE)/cooccurrence.cpp $(CFLAGS) $(INCPATH) -o $@

$(OBJ)/jensen_shannon.o: $(SOURCE)/jensen_shannon.cpp
	$(CC) -c $(SOURCE)/jensen_shannon.cpp $(CFLAGS) $(INCPATH) -o $@

$(OBJ)/main.o: $(SOURCE)/main.cpp
	$(CC) -c $(SOURCE)/main.cpp $(CFLAGS) $(INCPATH) -o $@

$(OBJ)/neighborhood.o: $(SOURCE)/neighborhood.cpp
	$(CC) -c $(SOURCE)/neighborhood.cpp $(CFLAGS) $(INCPATH) -o $@

$(OBJ)/node.o: $(SOURCE)/node.cpp
	$(CC) -c $(SOURCE)/node.cpp $(CFLAGS) $(INCPATH) -o $@

$(OBJ)/read_ini.o: $(SOURCE)/read_ini.cpp
	$(CC) -c $(SOURCE)/read_ini.cpp $(CFLAGS) $(INCPATH) -o $@

$(OBJ)/read_input.o: $(SOURCE)/read_input.cpp
	$(CC) -c $(SOURCE)/read_input.cpp $(CFLAGS) $(INCPATH) -o $@

