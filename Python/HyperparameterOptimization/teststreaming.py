import signal
import subprocess
import os

if __name__ == '__main__':
    execpath = '/home/sci/karthik/projects/ShapeWorks/build/shapeworks-build/bin/ShapeWorksRun'
    xmlpath = '/home/sci/karthik/projects/ShapeWorks/Examples/Python/TestEllipsoids/PointFiles/hyperparm_128.xml'
    print(execpath, xmlpath)
    process = subprocess.Popen(' '.join([execpath, xmlpath]), stdout=subprocess.PIPE,
                                preexec_fn=os.setsid, shell=True)

    while True:
        output = process.stdout.readline()
        if not output and not process.poll():
            if not process.poll():
                break
            else:
                continue
        line = output.decode('utf8').strip()
        if 'Energy' not in line:
            continue
        energy = float(line.split(' ')[-1])
        print(energy)

    os.killpg(os.getpgid(process.pid), signal.SIGTERM)