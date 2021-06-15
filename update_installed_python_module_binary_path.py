import sys
from os import path
import fileinput

"""
Updates installed setup_path.py that tells shapeworks module where to find compiled API cython library.
"""

def replaceAll(file, searchExp, replaceExp):
    print(f"{file} {searchExp} {replaceExp}")
    for line in fileinput.input(file, inplace=1):
        if searchExp in line:
            line = line.replace(searchExp,replaceExp)
        sys.stdout.write(line)

if __name__ == "__main__":
    if len(sys.argv[1:]) != 2:
        print(f'params: <shapeworks module setup_path.py> <shapeworks bin path>')
        sys.exit(1)
    MODULE_INSTALL_SCRIPT = sys.argv[1]
    BINARY_INSTALL_PATH = sys.argv[2]
    replaceAll(path.normpath(path.join(MODULE_INSTALL_SCRIPT)),
               str("\"\""),
               str(f'\"{BINARY_INSTALL_PATH}\"'))
