cf mapmap
cg mapmap
del mapmap.tco
m80 =mapmap.mac/z/m
del mapmap.mac
m80 =maplib.asm/z/m
l80 b:\lib\ck,mapmap,maplib,b:\lib\clib/s,b:\lib\crun/s,b:\lib\cend,mapmap/n/y/e:xmain
