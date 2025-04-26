##########################
# Main Compiling/Linking #
##########################
CC = g++
CCFLAGS = -g -Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -Wsign-conversion -Wconversion # -fsanitize=address  # -Og -O3 -Ofast

all: rd_view transform dynamic static 3view

rd_view: libcs630.a  bezier.cc
	$(CC) $(CCFLAGS) -o rd_view $^ -lm -lX11

transform: transform.cc
	$(CC) $(CCFLAGS) -o transform $^

dynamic: dynamic.cc
	$(CC) $(CCFLAGS) -o dynamic $^

static: static.cc
	$(CC) $(CCFLAGS) -o static $^

3view: 3view.cc
	$(CC) $(CCFLAGS) -o 3view $^

clean:
	rm -f *.o rd_view transform dynamic static 3view


# Optional: Recompiles everything again (for debugging)
again: clean
	clear
	make


#########
# Tests #
#########
# Right-click after each image to go to the next one
test: clean
	make
	clear

	./transform Input/cube.dat    skew    3 7    | ./3view - | ./rd_view -
	./transform Input/rsphere.dat twist   65     | ./3view - | ./rd_view -
	cat         Input/square.dat                 | ./3view - | ./rd_view -
	./transform Input/square.dat  extrude 10     | ./3view - | ./rd_view -
	cat         Input/shapes.dat                 | ./3view - | ./rd_view -
	./transform Input/shapes.dat  extrude 5      | ./3view - | ./rd_view -
	./transform Input/sq_off.dat  sweep   3 45   | ./3view - | ./rd_view -
	./transform Input/shapes.dat  sweep   20 360 | ./3view - | ./rd_view -

	./rd_view Input/s55.rd
	./rd_view Input/s56.rd
	./rd_view Input/s58.rd

	./rd_view Input/sqsphere.rd
	./rd_view Input/sqtorus.rd
	./rd_view Input/four_sqs.rd

	./static  | ./rd_view -
	./dynamic | ./rd_view -
