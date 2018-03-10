FmiWrapper.dll: FmiWrapper.o SystemFunctions.o
	gcc -shared -o FmiWrapper.dll FmiWrapper.o SystemFunctions.o

FmiWrapper.o: FmiWrapper.c
	gcc -Wall -c FmiWrapper.c

SystemFunctions.o: SystemFunctions.c
	gcc -Wall -c SystemFunctions.c