CC = g++
CCFLAGS = -g -Wall #-Og #-O3 -Ofast

rd_view: libcs631.a  Vector3.o Point4.o Matrix4.o  rd_direct.o pnm_display.o
	$(CC) -o rd_view $(CCFLAGS) $^ -lm -lX11

# Add whatever additional files and rules here, and also
# in the final linking rule above.
Vector3.o: Vector3.cc Vector3.h
	$(CC) $(CCFLAGS) -c Vector3.cc

Point4.o: Point4.cc Point4.h Vector3.h
	$(CC) $(CCFLAGS) -c Point4.cc

Matrix4.o: Matrix4.cc Matrix4.h Point4.h
	$(CC) $(CCFLAGS) -c Matrix4.cc

rd_direct.o: rd_direct.cc rd_direct.h Matrix4.h
	$(CC) $(CCFLAGS) -c rd_direct.cc

pnm_display.o: pnm_display.cc pnm_display.h
	$(CC) $(CCFLAGS) -c pnm_display.cc

clean:
	-rm -f *.o *.ppm rd_view


test: clean
	make
	clear
	./rd_view Input/lego.rd
	./rd_view Input/s46.rd

	./rd_view Input/s27.rd
	./rd_view Input/s28.rd
	./rd_view Input/s29.rd
	./rd_view Input/s30.rd
	./rd_view Input/s31.rd
	./rd_view Input/s32.rd
	./rd_view Input/s33.rd
	./rd_view Input/s34.rd
	./rd_view Input/s35.rd
	./rd_view Input/s36.rd
	./rd_view Input/s37.rd
	./rd_view Input/s38.rd
	./rd_view Input/s39.rd
	./rd_view Input/s40.rd

	./rd_view Input/s41.rd
	./rd_view Input/s42.rd
	./rd_view Input/s43.rd
	./rd_view Input/s44.rd
	./rd_view Input/s45.rd
#	./rd_view Input/s46.rd
	./rd_view Input/s47.rd
	./rd_view Input/s48.rd
	./rd_view Input/s49.rd
	./rd_view Input/s50.rd
