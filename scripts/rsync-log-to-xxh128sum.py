#!/usr/bin/env python

# Formats output of rsync log executed with --log-file-format='%i %C %n%L' to xxh128sum compatible input
# Usage rsync-log-to-xxh128sum.py < rsync.log

import sys

def process_log():
    files = {}

    for line in sys.stdin:
#        print(line.rstrip())
        items = line.split()

        oper = items[3]
        if oper == "*deleting" or (len(oper) == 11 and (oper[0] == ">" or oper[0] == "h")):
            if oper == "*deleting":
                if items[4][-1] == "/": #Ignore directory delete
                    continue

                del files[items[4]]
            elif oper[0] == ">":
                files[items[5]] = items[4]
            elif oper[0] == "h":
                files[items[4]] = files[items[6]]
            else:
                sys.exit("Unhandled oper")

    return files

def print_xxh128sum(files):
    for fname, hval in files.items():
        print("%s  %s" % (hval, fname))

def main():
    print_xxh128sum(process_log())

if __name__ == "__main__":
    main()
