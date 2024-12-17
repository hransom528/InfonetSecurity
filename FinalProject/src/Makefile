CC = g++
CFLAGS = -g -Wall -lssl -lcrypto -lcryptopp
DEPS = MessagePreprocess.h
	
.PHONY : all
all: MessagePreprocess.exe NeedhamSchroederSymmetric.exe NeedhamSchroederPublicKey.exe NeedhamSchroederSymmetricCTR.exe NeedhamSchroederModeComparison.exe

#%.o: %.cpp $(DEPS)
#	$(CC) $< $(CFLAGS) -c -o $@
	  
MessagePreprocess.exe: MessagePreprocess.cpp
	$(CC) -o MessagePreprocess.exe MessagePreprocess.cpp $(CFLAGS)

NeedhamSchroederSymmetric.exe: NeedhamSchroederSymmetric.cpp
	$(CC) -o NeedhamSchroederSymmetric.exe NeedhamSchroederSymmetric.cpp $(CFLAGS)

NeedhamSchroederPublicKey.exe: NeedhamSchroederPublicKey.cpp
	$(CC) -o NeedhamSchroederPublicKey.exe NeedhamSchroederPublicKey.cpp $(CFLAGS)

NeedhamSchroederSymmetricCTR.exe: NeedhamSchroederSymmetricCTR.cpp
	$(CC) -o NeedhamSchroederSymmetricCTR.exe NeedhamSchroederSymmetricCTR.cpp $(CFLAGS)

NeedhamSchroederModeComparison.exe: NeedhamSchroederModeComparison.cpp
	$(CC) -o NeedhamSchroederModeComparison.exe NeedhamSchroederModeComparison.cpp $(CFLAGS)

clean:
	rm *.exe