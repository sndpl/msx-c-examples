cf dcopy1
cg dcopy1
m80 =dcopy1/z
del dcopy1.mac
cf dcopy2v2
fpc dcopy1 dcopy2v2 lib
del dcopy1.tco
cg -k dcopy2v2
m80 =dcopy2v2/z
del dcopy2v2.mac
l80 b:\lib\ck,dcopy1,dcopy2v2,b:\lib\clib/s,b:\lib\crun/s,b:\lib\cend,dcopyv2/n/y/e:xmain
