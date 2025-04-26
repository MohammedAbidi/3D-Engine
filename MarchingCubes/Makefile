##########################
# Main Compiling/Linking #
##########################
CC = g++
CCFLAGS = -g -Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -Wsign-conversion -Wconversion -fsanitize=address  # -Og -O3 -Ofast
EXES = cubes cview

make: $(EXES)

cubes: main.cc
	$(CC) $(CCFLAGS) -o cubes $^

cview: cview.cc
	$(CC) $(CCFLAGS) -o cview $^

clean:
	rm -f *.o $(EXES)

# Optional: Recompiles everything again (for debugging)
again: clean
	clear
	make


########
# Test #
########
test: clean
	make
	clear

#	./cubes -n Input/mc1.dat 1    > mc.o
#	./cubes -n Input/mc1.dat 1.25 > mc.o
#	./cubes -n Input/mc2.dat 2    > mc.o
#	./cubes -n Input/mc2.dat 3.5  > mc.o
	./cubes -n Input/mc3.dat 4.5 > mc.o
#	./cubes -n Input/mc3.dat 5.17 > mc.o
#	./cubes -n Input/mc3.dat 5.5  > mc.o
#	./cubes -n Input/mc3.dat 7.5  > mc.o

#	cat mc.o
	./cview -d 20 20 20 mc.o | ./rd_view -
