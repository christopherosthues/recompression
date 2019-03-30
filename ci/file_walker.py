#!/usr/bin/python

import os
import os.path

with open("../file_walker.cmake", 'w') as file_walker:
    file_walker.write("set(SOURCES\n")
    for root, sub_dirs, files in os.walk("../src"):
        for file in files:
            if file.endswith(".cpp"):
                file_walker.write("    " + root[3:] + "/" + file + "\n")
    file_walker.write(")\n")
    file_walker.write("set(HEADERS\n")
    for root, sub_dirs, files in os.walk("../include"):
        for file in files:
            if file.endswith(".hpp"):
                r = root[1:] if root == "." else root[2:]
                file_walker.write("    " + root[3:] + "/" + file + "\n")
    file_walker.write(")\n")
