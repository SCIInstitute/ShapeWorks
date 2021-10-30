#!/usr/bin/env python3
import time
import subprocess
import argparse
import sys
from subprocess import PIPE, run
from subprocess import Popen, PIPE, STDOUT
import io
import selectors
import subprocess
import sys

passed = []
failed = []


class Logger(object):
    def __init__(self):
        self.terminal = sys.stdout
        self.log = open("verify.log", "w")

    def write(self, message):
        self.terminal.write(message)
        self.log.write(message)

    def flush(self):
        # this flush method is needed for python 3 compatibility.
        # this handles the flush command by doing nothing.
        # you might want to specify some extra behavior here.
        pass

    def __getattr__(self, attr):
        return getattr(self.terminal, attr)


sys.stdout = Logger()


# adapted from:
# https://gist.github.com/nawatts/e2cdca610463200c12eac2a14efc0bfb#file-capture-and-print-subprocess-output-py
def run_command(subprocess_args):
    # Start subprocess
    # bufsize = 1 means output is line buffered
    # universal_newlines = True is required for line buffering
    process = subprocess.Popen(subprocess_args,
                               bufsize=1,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT,
                               universal_newlines=True)

    def handle_output(stream):
        # Because the process' output is line buffered, there's only ever one
        # line to read when this function is called
        line = stream.readline()
        sys.stdout.write(line)

    # Register callback for an "available for read" event from subprocess' stdout stream
    selector = selectors.DefaultSelector()
    selector.register(process.stdout, selectors.EVENT_READ, handle_output)

    # Loop until subprocess is terminated
    while process.poll() is None:
        # Wait for events and handle them with their registered callbacks
        events = selector.select()
        for key, mask in events:
            callback = key.data
            callback(key.fileobj)

    # Get process return code
    return_code = process.wait()
    selector.close()

    success = (return_code == 0)
    return success


def run_case(use_case):
    command = f"python -u RunUseCase.py {use_case}"
    print(f"\n----------------------------------------------------------------")
    print(f"* Running : {command}")
    print(f"----------------------------------------------------------------")
    start = time.time()

    success = run_command(command.split())

    end = time.time()
    duration = f"{end - start:6.0f} seconds"
    if success:
        print(f"{use_case} : {duration} : PASSED")
        passed.append([duration, use_case])
    else:
        print(f"{use_case} : {duration} : FAILED")
        failed.append([duration, use_case])


def main():
    # parse arguments
    parser = argparse.ArgumentParser(description='Verify Example ShapeWorks Pipelines')
    parser.add_argument("--tiny_test", help="Run only the tiny tests", action="store_true")
    args = parser.parse_args()

    start = time.time()
    run_case("ellipsoid --tiny_test")
    run_case("ellipsoid_mesh --tiny_test")
    run_case("ellipsoid_fd --tiny_test")
    run_case("ellipsoid_cut --tiny_test")
    run_case("lumps --tiny_test")
    run_case("left_atrium --tiny_test")
    run_case("femur --tiny_test")
    run_case("ellipsoid --tiny_test")
    run_case("ellipsoid_mesh --tiny_test")
    run_case("ellipsoid_fd --tiny_test")
    run_case("ellipsoid_cut --tiny_test")
    run_case("lumps --tiny_test")
    run_case("left_atrium --tiny_test")
    run_case("femur --tiny_test")
    run_case("femur --groom_images --tiny_test")
    run_case("femur_mesh --tiny_test")
    run_case("femur_cut --tiny_test")
    run_case("femur_cut --groom_images --tiny_test")
    run_case("deep_ssm --tiny_test")
    run_case("supershapes_1mode_contour --tiny_test")
    run_case("thin_cavity_bean --tiny_test")
    run_case("ellipsoid_multiple_domain --tiny_test")
    run_case("ellipsoid_multiple_domain_mesh --tiny_test")
    run_case("ellipsoid_pca --tiny_test")

    if not args.tiny_test:
        run_case("ellipsoid --verify")
        run_case("ellipsoid_cut --verify")
        run_case("ellipsoid_fd --verify")
        run_case("ellipsoid_mesh --verify")
        run_case("ellipsoid_multiple_domain --verify")
        run_case("ellipsoid_multiple_domain_mesh --verify")
        run_case("femur --verify")
        run_case("femur --groom_images --verify")
        run_case("femur_mesh --verify")
        run_case("femur_cut --verify")
        run_case("femur_cut --groom_images --verify")
        run_case("left_atrium --verify")
        run_case("left_atrium --groom_images --verify")
        run_case("lumps --verify")
        run_case("thin_cavity_bean --verify")
        run_case("supershapes_1mode_contour --verify")
    #        run_case("deep_ssm --verify")

    end = time.time()

    print("\n---------------------------------------------")
    print("Testing Results:")
    print("---------------------------------------------")
    print(f"The following use cases passed ({len(passed)})")
    for item in passed:
        print(f"[ PASSED ] : {item[0]} : {item[1]}")

    if len(failed) > 0:
        print("\n---------------------------------------------")
        print(f"The following use cases failed ({len(failed)})")
        for item in failed:
            print(f"[ FAILED ] : {item[0]} : {item[1]}")

    total = len(passed) + len(failed)
    pass_percent = len(passed) / total

    print("\n---------------------------------------------")
    print(f"\n{pass_percent:.2%} tests passed, {len(failed)} failed out of {total}\n")

    print(f"Total Test time: {end - start:.0f} seconds\n")


if __name__ == '__main__':
    main()
