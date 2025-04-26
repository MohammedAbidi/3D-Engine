Engine "raytrace"
Display "sphereflake" "Screen" "rgbdouble"
#Display "r56" "PNM" "rgb"

ObjectBegin "Sphere0"
XformPush
Translate 0 0 1
Sphere 1.0 -1.0 1.0 360.0
XformPop
ObjectEnd # Sphere0

ObjectBegin "Sphere1"
XformPush
Translate 0 0 1
Rotate "Z" 45.0
Sphere 1.0 -1.0 1.0 360.0

#Top
XformPush
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere0"
XformPop

#Back
XformPush
Rotate "X" -90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere0"
XformPop

#Front
XformPush
Rotate "X" 90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere0"
XformPop

#Left
XformPush
Rotate "Y" -90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere0"
XformPop

#Right
XformPush
Rotate "Y" 90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere0"
XformPop

XformPop
ObjectEnd # Sphere1

ObjectBegin "Sphere2"
XformPush
Translate 0 0 1
Rotate "Z" 45.0
Sphere 1.0 -1.0 1.0 360.0

#Top
XformPush
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere1"
XformPop

#Back
XformPush
Rotate "X" -90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere1"
XformPop

#Front
XformPush
Rotate "X" 90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere1"
XformPop

#Left
XformPush
Rotate "Y" -90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere1"
XformPop

#Right
XformPush
Rotate "Y" 90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere1"
XformPop

XformPop
ObjectEnd # Sphere2

ObjectBegin "Sphere3"
XformPush
Translate 0 0 1
Rotate "Z" 45.0
Sphere 1.0 -1.0 1.0 360.0

#Top
XformPush
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere2"
XformPop

#Back
XformPush
Rotate "X" -90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere2"
XformPop

#Front
XformPush
Rotate "X" 90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere2"
XformPop

#Left
XformPush
Rotate "Y" -90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere2"
XformPop

#Right
XformPush
Rotate "Y" 90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere2"
XformPop

XformPop
ObjectEnd # Sphere3

ObjectBegin "Sphere4"
XformPush
Translate 0 0 1
Rotate "Z" 45.0
Sphere 1.0 -1.0 1.0 360.0

#Top
XformPush
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere3"
XformPop

#Back
XformPush
Rotate "X" -90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere3"
XformPop

#Front
XformPush
Rotate "X" 90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere3"
XformPop

#Left
XformPush
Rotate "Y" -90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere3"
XformPop

#Right
XformPush
Rotate "Y" 90
Translate 0 0 1
Scale 0.5 0.5 0.5
ObjectInstance "Sphere3"
XformPop

XformPop
ObjectEnd # Sphere4

CameraAt 0 0 20
CameraEye -25 -25 60
CameraUp 0 0 1
CameraFOV 45.0

#Format 1920 1440
Format 640 480
Background 0.4 0.5 0.9
OptionReal "Levels"  4
OptionBool "Bounding Tree"  on  # Default
WorldBegin

AmbientLight 0.4 0.5 0.9 1.0

FarLight 0 1 -1 1.0 1.0 1.0 1.0


Color 0.8 0.7 0.0
Ka 1.0
Kd 1.0

PolySet "P"
4 1
-100 -100 0
 100 -100 0
 100  100 0
-100  100 0
0 1 2 3 -1

Color 0.8 0.8 0.8
Specular 0.8 0.8 0.8 50
Surface "metal"
Ka 0.2
Kd 0.2
Ks 0.8

Scale 10.0 10.0 10.0
ObjectInstance "Sphere4"

WorldEnd
