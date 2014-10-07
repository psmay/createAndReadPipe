createAndReadPipe
=================

A simple Windows console app that creates a named pipe for reading and
then reads it directly to `stdout` with relatively little buffering. In
other words, it makes a named pipe behave a little more like an unnamed
pipe.

Usage
-----

    createAndReadPipe PIPENAME

This creates an inbound pipe with the path `\\.\pipe\PIPENAME` and
accepts one connection on that pipe. When that connection closes, this
program exits.

Build
-----

This was built using MinGW and MSYS. The resulting executable only
depends on the Windows API.

Binaries
--------

A `build` directory has been intentionally left in this tree for those
who just need the binary right away.

License
-------

This program is hereby released into the public domain.
