Display "Spinning Top" "Screen" "rgbobject"
#Display "Spinning Top" "PNM" "rgb"
Format 640 480
Background 0 0 0

OptionBool "Interpolate" off
OptionReal "Divisions" 24

CameraFOV 13
CameraUp 0 0 1
CameraAt 0 0 -.25
CameraEye 6 8 5  # Pythagorean triple (2x 2x 1x)


FrameBegin 1
WorldBegin

# Lighting
FarLight  -1 -1 -1  0 1 0  .5  # Green (front)
FarLight  -1  1 -1  1 0 0   1  # Red
FarLight   1  1 -1  0 1 0  .5  # Green (back)
FarLight   1 -1 -1  0 0 1   1  # Blue
Ka .5
Kd .5
Ks .5

# Shadow
Surface "matte"
Color .4 .4 .4
Disk -1 1 360

# Spinning Top
Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 0 10 5
FrameBegin 2
WorldBegin

FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Surface "matte"
Color .4 .4 .4
Disk -1 1 360

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -6 8 5
FrameBegin 3
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Surface "matte"
Color .4 .4 .4
Disk -1 1 360

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -8 6 5
FrameBegin 4
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Surface "matte"
Color .4 .4 .4
Disk -1 1 360

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -10 0 5
FrameBegin 5
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Surface "matte"
Color .4 .4 .4
Disk -1 1 360

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -8 -6 5
FrameBegin 6
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Surface "matte"
Color .4 .4 .4
Disk -1 1 360

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -6 -8 5
FrameBegin 7
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Surface "matte"
Color .4 .4 .4
Disk -1 1 360

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 0 -10 5
FrameBegin 8
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.08715574274765817 0 0  # sin(5)

Surface "matte"
Color .4 .4 .4
Disk -1 0.9961946980917455 360  # cos(5)

Translate 0 0 -0.003805301908254455  # 1-cos(5)
Rotate "Y" -5

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 6 -8 5
FrameBegin 9
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.17364817766693033 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.984807753012208 360

Translate 0 0 -0.01519224698779198
Rotate "Y" -10

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 8 -6 5
FrameBegin 10
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.25881904510252074 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.9659258262890683 360

Translate 0 0 -0.03407417371093169
Rotate "Y" -15

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 10 0 5
FrameBegin 11
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.3420201433256687 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.9396926207859084 360

Translate 0 0 -0.06030737921409157
Rotate "Y" -20

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 8 6 5
FrameBegin 12
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.42261826174069944 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.9063077870366499 360

Translate 0 0 -0.09369221296335006
Rotate "Y" -25

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 6 8 5
FrameBegin 13
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.49999999999999994 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.8660254037844387 360

Translate 0 0 -0.1339745962155613
Rotate "Y" -30

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 0 10 5
FrameBegin 14
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.573576436351046 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.8191520442889918 360

Translate 0 0 -0.1808479557110082
Rotate "Y" -35

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -6 8 5
FrameBegin 15
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.6427876096865393 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.766044443118978 360

Translate 0 0 -0.233955556881022
Rotate "Y" -40

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -8 6 5
FrameBegin 16
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -9.659258262890683 2.5881904510252074 5
FrameBegin 17
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -9.983081582716064 0.5814482890466817 5
FrameBegin 18
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -9.910609820056912 -1.3340963213319712 5
FrameBegin 19
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -9.510565162886817 -3.090169943948662 5
FrameBegin 20
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -8.856639100282592 -4.643268659828501 5
FrameBegin 21
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -8.021231927307252 -5.97158591735452 5
FrameBegin 22
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -7.071067811495235 -7.071067812235715 5
FrameBegin 23
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -6.064510853470044 -7.951207965343633 5
FrameBegin 24
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -5.050298415064 -8.631018822757952 5
FrameBegin 25
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -4.067366429897005 -9.13545457680935 5
FrameBegin 26
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -3.1454475604668652 -9.49242643607803 5
FrameBegin 27
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -2.3061587061789983 -9.730448706093405 5
FrameBegin 28
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -1.5643446489542792 -9.876883406180724 5
FrameBegin 29
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -0.929498607637247 -9.956707906652701 5
FrameBegin 30
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye -0.4071309325641933 -9.991708782973483 5
FrameBegin 31
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 2.0943901023875736e-09 -10.0 5
FrameBegin 32
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 0.2908471897572369 -9.995769500754323 5
FrameBegin 33
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 0.465253124546134 -9.989171113265607 5
FrameBegin 34
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd


CameraEye 0.5233595652529968 -9.986295347397762 5
FrameBegin 35
WorldBegin
FarLight  -1 -1 -1  0 1 0  .5
FarLight  -1  1 -1  1 0 0   1
FarLight   1  1 -1  0 1 0  .5
FarLight   1 -1 -1  0 0 1   1
Ka .5
Kd .5
Ks .5

Translate -0.7071067811865476 0 0

Surface "matte"
Color .4 .4 .4
Disk -1 0.7071067811865476 360

Translate 0 0 -0.2928932188134524
Rotate "Y" -45

Surface "plastic"
Color .8 .8 .8
SqSphere 1 3 1 -1 1 360

WorldEnd
FrameEnd

