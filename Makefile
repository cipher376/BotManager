.PHONY: all clear

#override with different compiler
cxx=g++

home = manager/

objects = Helper.o main.o ThreadManager.o VirtualMachineManager.o

all = bot.exe

header = $(home)Helper.hpp $(home)ThreadManager.hpp $(home)VirtualMachineManager.hpp

bot.exe: $(objects)
	$(cxx) -o bot.exe $(objects) -pthread -lpthread -lrt

Helper.o: $(home)Helper.cpp $(home)Helper.hpp 
	$(cxx) -c $(home)Helper.cpp

main.o: $(home)main.cpp $(header)
	$(cxx)	-c $(home)main.cpp

ThreadManager.o: $(home)ThreadManager.cpp $(home)ThreadManager.hpp $(header)
	$(cxx) -c $(home)ThreadManager.cpp

VirtualMachineManager.o: $(home)VirtualMachineManager.cpp $(home)VirtualMachineManager.hpp $(header)
	$(cxx) -c $(home)VirtualMachineManager.cpp



clean:
	rm *.o
	rm bot.exe


