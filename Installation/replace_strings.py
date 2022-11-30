import sys
import fileinput

"""
Replaces strings in a file.
"""
def replace_strings(file, search_exp, replace_exp):
    for line in fileinput.input(file, backup='', inplace=1):
        if search_exp in line:
            line = line.replace(search_exp, replace_exp)
        sys.stdout.write(line)

if __name__ == "__main__":
    if len(sys.argv[1:]) != 3:
        print(f'params: <file> <search string> <replace string>')
        sys.exit(1)
    FILE = sys.argv[1]
    FIND = sys.argv[2]
    REPL = sys.argv[3]
    replace_strings(FILE, FIND, REPL)
