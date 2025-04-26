Engine "raytrace"
Display "Axis Aligned Rectangular Prism (Cube)"  "Screen" "rgbdouble"
Format 480 640

CameraEye 50 30 50
CameraAt 0 0 0
CameraUp 0 0 1
CameraFOV 10

WorldBegin

Surface "plastic"
Ka 0.2
Kd 0.5
Ks 1

FarLight   -1 -2 -3   0 0 1  1
#PointLight 0  0  3   0 0 1  5

Sphere 4 -4 4 360
Scale 2 3 4
Cube

WorldEnd
