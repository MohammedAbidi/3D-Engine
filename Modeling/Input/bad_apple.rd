Display "Deformed Apple" "Screen" "rgbsingle"
#Display "Deformed Apple" "PNM" "rgb"

# This is really just a Bezier Curve,
# but I flipped the coords to "fill" it
ObjectBegin "Leaf"
Patch "Bezier" "P"
3 1
0    0    2
0    0    3.34
1.77 0    3.77
0    0    2

0    0    2
1.77 0    3.77
0    0    3.34
0    0    2
ObjectEnd


ObjectBegin "Apple"
Patch "Bezier" "P"
3 12

 0     0     2
 1.66  0     3.66
 3.06  0     1.54
 0     0     0

 0     0     2
 1.44  0.83  3.66
 2.65  1.53  1.54
 0     0     0

 0     0     2
 0.83  1.44  3.66
 1.53  2.65  1.54
 0     0     0


 0     0     2
 0     1.66  3.66
 0     3.06  1.54
 0     0     0

 0     0     2
-0.83  1.44  3.66
-1.53  2.65  1.54
 0     0     0

 0     0     2
-1.44  0.83  3.66
-2.65  1.53  1.54
 0     0     0


 0     0     2
-1.66  0     3.66
-3.06  0     1.54
 0     0     0

 0     0     2
-1.44 -0.83  3.66
-2.65 -1.53  1.54
 0     0     0

 0     0     2
-0.83 -1.44  3.66
-1.53 -2.65  1.54
 0     0     0


 0     0     2
 0    -1.66  3.66
 0    -3.06  1.54
 0     0     0

 0     0     2
 0.83 -1.44  3.66
 1.53 -2.65  1.54
 0     0     0

 0     0     2
 1.44 -0.83  3.66
 2.65 -1.53  1.54
 0     0     0


 0     0     2
 1.66  0     3.66
 3.06  0     1.54
 0     0     0

ObjectEnd




CameraEye 0 -10 8
CameraUp  0 0 1
CameraAt  0 0 2
CameraFOV 20

Background .5 .75 1


WorldBegin

FarLight  -1 -2 -3  1 1 1  1

Surface "plastic"
Ka 0.2
Kd 0.5
Ks 0.3


Color 0.3 0.8 0.3
ObjectInstance "Leaf"

Color 0.8 0.3 0.3
ObjectInstance "Apple"


WorldEnd
