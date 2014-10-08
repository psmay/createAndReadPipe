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

See [the Releases page for this project on
GitHub](https://github.com/psmay/createAndReadPipe/releases/) for recent
binaries.

License
-------

This program is hereby released into the public domain.
