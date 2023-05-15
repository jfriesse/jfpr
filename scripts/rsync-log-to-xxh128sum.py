#!/usr/bin/env python

# Formats output of rsync log executed with --log-file-format='%i %C %n%L' to xxh128sum compatible input
# Usage rsync-log-to-xxh128sum.py < rsync.log

import sys
import re

def process_log():
    files = {}

    for line in sys.stdin:
        line = line.rstrip("\n")
#        print(line)

        items = line.split()

        oper = items[3]
        if oper == "*deleting" or (len(oper) == 11 and (oper[0] == ">" or oper[0] == "h" or oper[0] == "c")):
            f_hash_and_name = re.sub("^[^ ]* [^ ]* [^ ]* [^ ]* ", "", line)
#            print(f_hash_and_name)

            if oper == "*deleting":
                fname = f_hash_and_name.lstrip()
#                print(fname)

                if fname not in files: # Ignore non-existing files (softlinks and directories)
                    continue

                del files[fname]
            elif oper[0] == ">":
                fhash = items[4]
                fname = re.sub("^[^ ]* ", "", f_hash_and_name)
#                print(fhash, fname)

                files[fname] = fhash
            elif oper[0] == "h":
                fname1 = re.sub(" => .*$", "", f_hash_and_name.lstrip())
                fname2 = re.sub("^.* => ", "", f_hash_and_name)
#                print(fname1, fname2)

                files[fname1] = files[fname2]
            elif oper[0] == "c":
                # For create oper check if new file doesn't delete old existing file
                fname = f_hash_and_name.lstrip()

                if oper[1] == "L":
                    fname = re.sub(" -> .*$", "", fname)

#                print(fname)

                if fname in files: # File overwrites existing file -> delete it from known files
                    del files[fname]
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
