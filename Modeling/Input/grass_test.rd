Display "Grass PolySet Idea" "Screen" "rgbdouble"

CameraFOV 70
CameraEye 0 -16 0
CameraAt  0 0 0
CameraUp  0 0 1
Background .4 .6 .8

WorldBegin
Translate -8 0 -8


Color 0 .5 0 # Green
PolySet "P"  # Grass fill (tip to base)
9 # Vertices
4 # Edges

16 .001 16             # Tip
 9 .001 12  12 .001 12
 4 .001 8   9  .001 8
 1 .001 4   7  .001 4
 0 .001 0   6  .001 0  # Base

0 1 2 0 -1
1 3 4 2 -1
3 5 6 4 -1
5 7 8 6 -1


Color 0 1 0 # Pure Green
LineSet "P" # Grass outline
9 # Vertices
4 # Edges

16 0 16          # Tip
 9 0 12  12 0 12
 4 0 8   9  0 8
 1 0 4   7  0 4
 0 0 0   6  0 0  # Base

0 1 2 0 -1
1 3 4 2 -1
3 5 6 4 -1
5 7 8 6 -1


WorldEnd
