Display "Four Superquadric Spheres" "Screen" "rgbobject"
Format 640 480
Background 0 0 0

OptionBool "Interpolate" off
OptionReal "Divisions" 60


CameraFOV 10
CameraEye 0 20 40

WorldBegin

# Lighting
FarLight  -1 -1 -1  1 1 1  1
Surface "plastic"
Ka .5
Kd .3
Ks .2

XformPush
Scale 5 1 5
Rotate "X" -90
Color 1 1 0 # Yellow
SqSphere 1 0 1 -1 1  360 # Cylinder
XformPop

XformPush
Translate -3 2 0
Rotate "X" -60
Rotate "Z" -30
Color 1 0 0 # Red
SqSphere 1 1 3  -1 1  360 # ?
XformPop

XformPush
Translate 0 2 0
Rotate "X" -60
Rotate "Z" -30
Color 0 1 0 # Green
SqSphere 1 .5 .5  -1 1  360 # Smooth Cube
XformPop

XformPush
Translate 3 2 0
Rotate "X" -60
Rotate "Z" -30
Color 0 0 1 # Blue
SqSphere 1 3 3  -1 1  360 # Spikey
XformPop


WorldEnd
