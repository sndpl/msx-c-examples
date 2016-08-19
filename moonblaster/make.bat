rem Assemble .MAC files

rem M80 =mbdrv
M80 =mbload

rem Compile and assemble C code

cf mbplay
cg -k mbplay
m80 =mbplay/z

rem Link
L80 b:\lib\ck,mbplay,mbdrv,mbload,b:\lib\clib/s,b:\lib\crun/s,b:\lib\cend,mbplay/n/y/e:xmain

rem Cleanup
rem del mbdrv.rel
rem del mbload.rel
rem del mbplay.rel
rem del mbplay.mac
