A small utility to quickly generate large, random files.

It generates two large sets of random numbers, and then
generates data files by XORing the numbers togeather with
different offsets used for different blocks.

For best performance these blocks should fit in cache. 

It checks that the final random numbers do not appear in the
rest of the block, as a check against a rand() implementation
with too short a period.

Currently it is configured using the #define values at
the start of the file.
