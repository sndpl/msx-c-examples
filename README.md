# MSX-C Examples / Experiments / Libraries

Use the http://www.lavandeira.net/2014/12/relearning-msx-5-setting-up-the-msx-c-environment-part-1/ blog posts to setup your MSX C environment.

## Disk Copy
Two versions of a disk copy program. Second version uses memory mapper and video RAM.

TODO: Check why second version doesn't work, it hangs during check for memory

## FMLib
Small library to play music with the FM chip.

Available commands:

| Function | Description |
| ------------ | ------------ |
| fmopen() | Open/Check FM-BIOS |
| fmclose() | Close FM-BIOS |
| fmwrite() | Write to OPLL register |
| fmotir() | Write to OPLL register 0..7 |
| fmstart() | Start background music |
| fmstop() | Stop background music |
| fmread() | Read data from ROM |
| fmtest() | Now Playing ? |

See also the testfm.c example.

TODO:
- Update the FMChip detection so it will also find the FM-PAC clones
- Clean the code

## Shooter [WIP]
My first attempt with MSX-C to create a shooter, needs some heavy refactoring :)
