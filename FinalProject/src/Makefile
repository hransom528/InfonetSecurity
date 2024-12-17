CC = g++
CFLAGS = -g -Wall -lssl -lcrypto -lcryptopp
DEPS = MessagePreprocess.h
	
.PHONY : all
all: MessagePreprocess.exe NeedhamSchroederSymmetric.exe NeedhamSchroederPublicKey.exe NeedhamSchroederSymmetricCTR.exe NeedhamSchroederModeComparison.exe

#%.o: %.cpp $(DEPS)
#	$(CC) $< $(CFLAGS) -c -o $@
	  
MessagePreprocess.exe: MessagePreprocess.cpp
	$(CC) -o ../bin/MessagePreprocess.exe MessagePreprocess.cpp $(CFLAGS)

NeedhamSchroederSymmetric.exe: NeedhamSchroederSymmetric.cpp
	$(CC) -o ../bin/NeedhamSchroederSymmetric.exe NeedhamSchroederSymmetric.cpp $(CFLAGS)

NeedhamSchroederPublicKey.exe: NeedhamSchroederPublicKey.cpp
	$(CC) -o ../bin/NeedhamSchroederPublicKey.exe NeedhamSchroederPublicKey.cpp $(CFLAGS)

NeedhamSchroederSymmetricCTR.exe: NeedhamSchroederSymmetricCTR.cpp
	$(CC) -o ../bin/NeedhamSchroederSymmetricCTR.exe NeedhamSchroederSymmetricCTR.cpp $(CFLAGS)

NeedhamSchroederModeComparison.exe: NeedhamSchroederModeComparison.cpp
	$(CC) -o ../bin/NeedhamSchroederModeComparison.exe NeedhamSchroederModeComparison.cpp $(CFLAGS)

clean:
	rm ../bin/*.exe