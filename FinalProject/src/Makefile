CC = g++
CFLAGS = -g -Wall -lssl -lcrypto
DEPS = MessagePreprocess.h
	
.PHONY : all
all: MessagePreprocess.exe NeedhamSchroederSymmetric.exe NeedhamSchroederPublicKey.exe

#%.o: %.cpp $(DEPS)
#	$(CC) $< $(CFLAGS) -c -o $@
	  
MessagePreprocess.exe: MessagePreprocess.cpp
	$(CC) -o MessagePreprocess.exe MessagePreprocess.cpp $(CFLAGS)

NeedhamSchroederSymmetric.exe: NeedhamSchroederSymmetric.cpp
	$(CC) -o NeedhamSchroederSymmetric.exe NeedhamSchroederSymmetric.cpp $(CFLAGS)

NeedhamSchroederPublicKey.exe: NeedhamSchroederPublicKey.cpp
	$(CC) -o NeedhamSchroederPublicKey.exe NeedhamSchroederPublicKey.cpp $(CFLAGS)

clean:
	rm *.exe