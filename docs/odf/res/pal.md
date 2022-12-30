PAL File Documentation
=======================

This file is used to provide a color scheme for different indexed image files like BM and FME files.

File structure
--------------
This file contains 256 different color, each represented by an RGB triplet.
The red, green and blue colors are represented by one byte each,
but due to a VGA limitation, these bytes would only contain numbers between 0 and 63
This file is always 768 (256*3) bytes long.

Example:
--------

`0x0A` `0x0F` `0x15` in this sequence would be a color combining
10/63 red, 15/63 green and 21/63 blue
