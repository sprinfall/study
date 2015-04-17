# -*- coding: utf-8 -*-
"""
--------------------------------------------------------------------------------
is_ascii.py
2009-06-15, adam gu <sprinfall@gmail.com>

check if a given file is encoded in ASCII.
only support checking for the following file types:
    - .h, .hpp, .c, .cpp, .cc, .cxx

usage:
    $ is_ascii.py <file)
if it cannot work, try:
    $ python is_ascii.py <file>

exit code 0 means the file is ASCII encoded.

please use Python 3.0.
--------------------------------------------------------------------------------
"""

import sys
import codecs
import os

def check_ascii_line(line):
    for i in range(len(line)):
        if ord(line[i]) > 127: # ascii character in range [0,127]
            return False
    return True

def check_ascii_file(file):
    try:
        fi = codecs.open(file, 'r', 'ascii')
    except: # IOError, etc.
        return 2

    try:
        for line in fi:
            if not check_ascii_line(line):
                if __debug__:
                    print "failed in line:", line
                fi.close()
                return 1
    except:
        if __debug__:
            print "failed to readline."
        return 1

    fi.close()
    return 0

def main():
    args = sys.argv[1:]
    if len(args) < 1:
        print(__doc__)
        sys.exit(2)

    file = args[0]
    root, ext = os.path.splitext(file)
    # only check encoding for these kinds of files.
    if ext not in ['.h', '.hpp', '.c', '.cpp', '.cc', '.cxx']:
        sys.exit(2)

    code = check_ascii_file(file)
    if __debug__:
        print "exit code: ", code
    sys.exit(code)

if __name__ == '__main__':
    main()

