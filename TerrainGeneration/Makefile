##########################
# Main Compiling/Linking #
##########################
CC = g++
CCFLAGS = -g -Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -Wsign-conversion -Wconversion -fsanitize=address  # -Og -O3 -Ofast

terrain: main.cc
	$(CC) $(CCFLAGS) -o terrain $^

clean:
	rm -f *.o terrain

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

#	-./terrain 11 2.2 68 10 | ./rd_view -
	-./terrain 6 2.2 68 10 > test_out.o
	-cat      test_out.o
	./rd_view test_out.o
