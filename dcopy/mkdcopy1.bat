cf dcopy1
cg dcopy1
m80 =dcopy1/z
del dcopy1.mac
cf dcopy2v1
fpc dcopy1 dcopy2v1 lib
del dcopy1.tco
cg -k dcopy2v1
m80 =dcopy2v1/z
del dcopy2v1.mac
l80 b:\lib\ck,dcopy1,dcopy2v1,b:\lib\clib/s,b:\lib\crun/s,b:\lib\cend,dcopyv1/n/y/e:xmain
