Engine "raytrace"
Display "Objects"  "Screen" "rgbdouble"
Format 800 300

#OptionReal "Levels" 1

CameraEye 20 10 10 # Front-ish (angled)
#CameraEye -20 -10 -10 # Behind-ish (inverse)
CameraAt 0 0 0
CameraUp 0 0 1
CameraFOV 10

WorldBegin

Surface "plastic"
Ka 0.2
Kd 0.5
Ks 0.3

FarLight  -1 -2 -3  1 1 1  1   # Angled Down
#FarLight  0  0 -1  1 1 1  1   # Straight Down


Sphere 1 -1 1 360


Translate -2 3 0
Cube


Translate 4 -6 0

###############
# Tetrahedron #
###############
PolySet "P"
4 # Vertices
4 # Faces

# Vertex points
 1.0  1.0  1.0
 1.0 -1.0 -1.0
-1.0  1.0 -1.0
-1.0 -1.0  1.0

# Face indices
3 2 1 -1
2 3 0 -1
1 0 3 -1
0 1 2 -1


WorldEnd