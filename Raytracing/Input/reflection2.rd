Engine "raytrace"
Display "  Reflect 2"  "Screen" "rgbdouble"
Format 800 800

OptionReal "Levels" 1

CameraEye 50 30 30
CameraAt 0 0 0
CameraUp 0 0 1
CameraFOV 5


WorldBegin

Surface "plastic"
Ka 0.2
Kd 0.5
Ks 0.5


FarLight 0 0 -1  1 1 1  1

Translate 0 -2 1
Sphere 1 -1 1 360

Translate 0 2 -1
Sphere 1 -1 1 360

Translate 0 2 -1
Sphere 1 -1 1 360

Translate 0 -2 -1
Scale 5 5 1
Cube

WorldEnd