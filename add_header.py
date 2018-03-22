#!/usr/bin/env python3

# add headers to .h, .cpp, .c and .ino files automatically.
# the script will first list the files missing headers, then
# prompt for confirmation. The modification is made inplace.
#
# usage:
#   add-headers.py <header file> <root dir>
#
# The script will first read the header template in <header file>,
# then scan for source files recursively from <root dir>.

import sys, os
import os.path as path
import fileinput

src_extensions = ['.h', '.cpp', '.c', '.ino']

def is_src_file(f):
    results = [f.endswith(ext) for ext in src_extensions]
    return True in results

def is_header_missing(f):
    with open(f) as reader:
        lines = reader.read().lstrip().splitlines()
        if len(lines) > 0: return not lines[0].startswith("/**")
        return True

def get_src_files(dirname):
    src_files = []
    for cur, _dirs, files in os.walk(dirname):
        [src_files.append(path.join(cur,f)) for f in files if is_src_file(f)]

    return [f for f in src_files if is_header_missing(f)]

def add_headers(files, header):
    for line in fileinput.input(files, inplace=True):
        if fileinput.isfirstline():
            for h in header.splitlines():
				 print(h) 
        print(line)



if __name__ == "__main__":

    if len(sys.argv) < 3:
        print("usage: %s <header file> <root dir>" % sys.argv[0])
        exit()

    args = [path.abspath(arg) for arg in sys.argv]
    root_path = path.abspath(args[2])

    header = open(args[1]).read()
    files = get_src_files(root_path)

    print("Files with missing headers:")
    for f in files:
		print("  - %s" % f)

    print()
    print("Header: ")
    print(header)

    #confirm = input("proceed ? [y/N] ")
    #if confirm is not "y": exit(0)

    add_headers(files, header)