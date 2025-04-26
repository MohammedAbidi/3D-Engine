##########################
# Main Compiling/Linking #
##########################
CC = g++
CCFLAGS = -g -Wall -Wextra -pedantic -fsanitize=address -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference  # -Wconversion -Wsign-conversion  # -Og -O3 -Ofast

make: rd_view

rd_view: libcs697.a  rd_ray.o pnm_display.o
	$(CC) $(CCFLAGS) -o rd_view $^ -lm -lX11

rd_ray.o: rd_ray.cc rd_ray.h
	$(CC) $(CCFLAGS) -c rd_ray.cc

pnm_display.o: pnm_display.cc pnm_display.h
	$(CC) $(CCFLAGS) -c pnm_display.cc

clean:
	rm -f *.o rd_view

# Recompiles everything again (for debugging)
again: clean
	clear
	make rd_view


#########
# Tests #
#########
# Cant really automate tests, so these are more like lazy shortcuts for me to "eyeball"
# Intended order of implementation
# 1. Sphere
# 2. Shading
# 3. Shadows
# 4. Axis Aligned Bounding Box (rd_cube)
# 5. PolySet (rd_cube = PolySet of 6 squares)
# 6. Reflections
# 7. Acceleration Structure
# 8. Smooth Shadows (WIP)
simple: rd_view
	clear
	./rd_view Input/simple_sphere.rd

shadow: rd_view
	clear
	./rd_view Input/shadow.rd

cube: rd_view
	clear
	./rd_view Input/rectangular_prism.rd

poly: rd_view
	clear
	./rd_view Input/polyset.rd

polys: rd_view
	clear
	./rd_view Input/polysets.rd

reflect: rd_view
	clear
	./rd_view Input/reflection.rd

reflect2: rd_view
	clear
	./rd_view Input/reflection2.rd

reflect3: rd_view
	clear
	./rd_view Input/reflection3.rd

objects: rd_view
	clear
	./rd_view Input/objects.rd

sphereflake: rd_view
	clear
	./rd_view Input/r56.rd

world: rd_view
	clear
	./rd_view Input/r55.rd

old: rd_view
	clear
	./rd_view Input/s45r.rd # shadow acne?
	./rd_view Input/s46r.rd
	./rd_view Input/s47r.rd # lots of faces
	./rd_view Input/s48r.rd
	./rd_view Input/s49r.rd


test: rd_view
	make simple
	make shadow
	make cube
	make poly
	make polys
	make reflect
	make reflect2
	make reflect3
	make objects
	make sphereflake
#	make world  # WIP (if you try to run this now it may never finish hahaha)
	make old
