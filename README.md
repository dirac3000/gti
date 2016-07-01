rpeo
====

Just a silly `repo` launcher, ~~inspired by~~ copied from `gti` (that was
inspired by `sl`).

Build and Install
-----------------

From source:

    $ make
    $ make install # as root, probably

The default install PREFIX is `/usr/bin`.

You can change the speed of the car at compile time via a cpp-define.
For example:

    $ make CFLAGS=-DROBOT_SPEED=100 # default is 500

It works on GNU/Linux and it should work on Windows, I haven't tested it though.

Usage
-----

Try typing `repo` really fast, on an unfamiliar keyboard.

Demo
----

![gif](http://i.imgur.com/KeLYDEk.gif)

Author and License
------------------

Copyright 2016 by Alvaro Moran <alvaro.moran@gmail.com>

Note that 99% of the code has been actually taken from gti, so I feel obliged to
leave the original copyright notice here.

Copyright 2012 by Richard Wossal <richard@r-wos.org>

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear in
supporting documentation.  No representations are made about the
suitability of this software for any purpose.  It is provided "as
is" without express or implied warranty.

