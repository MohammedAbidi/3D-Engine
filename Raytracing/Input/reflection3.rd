Engine "raytrace"
Display "Reflect 3"  "Screen" "rgbdouble"
Format 800 800

OptionReal "Levels" 2

CameraEye 50 30 30
CameraAt 0 0 0
CameraUp 0 0 1
CameraFOV 6


WorldBegin

Surface "plastic"
Ka 0.2
Kd 0.5
Ks 0.5

#FarLight -1 -2 -3  1 1 1  1
FarLight 0 0 -1  1 1 1  1


Translate 0 -2 1
XformPush
Scale 2 2 1
Color 1 0 0
Sphere 1 -1 1 360
XformPop

Translate 0 2 -1
XformPush
Scale 2 2 2
Color 0 1 0
Sphere 1 -1 1 360
XformPop

Translate 0 2 -1
Color 0 0 1
Sphere 1 -1 1 360

Translate 0 -2 -1
Scale 5 5 1
Color 1 1 1
Cube

WorldEnd