Engine "raytrace"
Display "Shadow Test"  "Screen" "rgbdouble"
Format 800 800

CameraEye 50 0 100
CameraAt 0 0 0
CameraUp 0 0 1
CameraFOV 5

WorldBegin

Surface "plastic"
Ka 0.1
Kd 0.5
Ks 1

FarLight   0   0 -1  1 1 1  .3
FarLight  .2 -.4 -1  1 1 1  .3
FarLight  .3  .3 -1  1 1 1  .3

PointLight 0 0 8  0 0 1  100


Sphere 3 -3 3 360
Translate 0 0 5
Sphere 1 -1 1 360
Translate 0 2 -3
Sphere 1 -1 1 360
Translate 0 -2 7
Color 0 0 1
Sphere .05 -.05 .05 360

WorldEnd
