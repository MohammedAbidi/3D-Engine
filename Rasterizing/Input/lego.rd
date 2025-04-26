# Lego Brick with real lighting

Display "Real lighting Lego Brick"  "Screen" "rgbsingle"
Format 640 480


##################
# 2x2 Lego Brick #
##################
ObjectBegin "Lego"

XformPush
Scale 0.0625 0.0625 0.0625

# Base
XformPush
Scale 16 16 9.6
Cube
XformPop

# Studs
XformPush
Translate 8 8 9.6
Scale 2 2 2
Disk 1.7 2.5 360
Cylinder 2.5 0 1.7 360

Translate -8 0 0
Disk 1.7 2.5 360
Cylinder 2.5 0 1.7 360

Translate 0 -8 0
Disk 1.7 2.5 360
Cylinder 2.5 0 1.7 360

Translate  8 0 0
Disk 1.7 2.5 360
Cylinder 2.5 0 1.7 360
XformPop

XformPop
ObjectEnd # Lego



##############
# Stud Label #
##############
ObjectBegin "Label" # Lego Label
# L
Line -.75 1.75 1.7   .75 1.75 1.7 # Side
Line -.75 1.25 1.7  -.75 1.75 1.7 # Bottom

# E
Line -.75 .75 1.7   .75 .75 1.7 # Side
Line  .75 .25 1.7   .75 .75 1.7 # Top
Line    0 .25 1.7     0 .75 1.7 # Middle
Line -.75 .25 1.7  -.75 .75 1.7 # Bottom

# G
Line -.75 -.25 1.7   .75 -.25 1.7 # Left
Line -.75 -.75 1.7     0 -.75 1.7 # Right
Line  .75 -.25 1.7   .75 -.75 1.7 # Top
Line    0 -.50 1.7     0 -.75 1.7 # Middle
Line -.75 -.25 1.7  -.75 -.75 1.7 # Bottom

# O
Line -.75 -1.25 1.7   .75 -1.25 1.7 # Left
Line -.75 -1.75 1.7   .75 -1.75 1.7 # Right
Line  .75 -1.25 1.7   .75 -1.75 1.7 # Top
Line -.75 -1.25 1.7  -.75 -1.75 1.7 # Bottom

ObjectEnd # Label



########
# Draw #
########
CameraEye  -3 -3 3
CameraAt   0 0 0
CameraUp   0 0 1
CameraFOV  70
#Background 0 .4 .6

FrameBegin 0
WorldBegin

# Lighting
AmbientLight      1 1 1  .6
FarLight  0 1 -1  1 1 1  1

Surface "plastic"
Ka 1
Kd 0.5
Ks 0.5


# Red Brick
XformPush
Translate 8 0 0
Rotate "Z" 90
Rotate "X" -30
Color 1 0 0
ObjectInstance "Lego"
XformPop


# Color brick collection
XformPush
Translate 0 8 0
Rotate "X" 30

Translate 2 0 0
Color 0 0 1
ObjectInstance "Lego"
Translate 0 2 0
Color 0 1 0
ObjectInstance "Lego"
Translate -2 0 0
Color 1 1 0
ObjectInstance "Lego"
Translate 0 -2 0
Color 1 0 0
ObjectInstance "Lego"
XformPop


# Main Red Brick
Translate -1 -1 1
Scale 0.0625 0.0625 0.0625

XformPush # Sides
Scale 16 16 16
PointLight  0 0 0   1 0 0  10 
ObjectInstance "Lego"
XformPop # Sides

#XformPush # Floor
#Color .5 .5 .5
#Translate 0 0 -28
#Scale 100 100 1
#Cube
#XformPop # Floor


# Stud Labels
Color .6 0 0
Translate 8 8 9.6
Scale 2 2 2

# Up
ObjectInstance "Label"

# Left
Translate -8 0 0
ObjectInstance "Label"

# Down
Translate 0 -8 0
ObjectInstance "Label"

# Right
Translate  8 0 0
ObjectInstance "Label"


WorldEnd
FrameEnd
