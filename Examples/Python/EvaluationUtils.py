import pathlib
import subprocess
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

def scree_plot(particles_glob, working_dir):
    """
    scree_plot takes a glob expression for a list of particles and generates
    a scree_plot showing the explainability of each mode. This is used to compute
    the compactness of the SSM.
    """

    print('Compactness\n'\
          '-----------')

    # Can't use list of args because it is incompatible with shell=True. We
    # need shell=True for glob expansion
    cmd = f'shapeworks readparticlesystem --names {particles_glob} -- '\
          f'           compactness --nmodes 1 --saveto {working_dir}/scree.txt'
    print(cmd)
    subprocess.check_call(cmd, shell=True)

    # Load scree plot data
    Y = np.loadtxt(f'{working_dir}/scree.txt')
    N = len(Y)
    X = np.arange(1, N+1)

    # Plot data
    plt.plot(X, Y, linewidth=4.0)
    fig = matplotlib.pyplot.gcf()
    fig.set_size_inches(10, 10)
    plt.title('Scree Plot')
    plt.xlabel('Mode')
    plt.ylabel('Scree')
    plt.xticks(X)
    plt.ylim(bottom=0, top=1.2)
    plt.xlim(left=1, right=N)
    plt.grid()
    plt.show()

    print()

def generalization(particles_glob, working_dir):
    """
    generalization takes a glob expression for a list of particles and computes the generalization
    of the SSM. The reconstructions are saved, and the 0th and 100th percentile are opened in
    ShapeWorksView2 for visualization
    """

    print('Generalization\n'\
          '--------------')

    # Can't use list of args because it is incompatible with shell=True. We
    # need shell=True for glob expansion
    cmd = f'shapeworks readparticlesystem --names {particles_glob} -- '\
          f'           generalization --nmodes 1 --saveto {working_dir}/'
    print(cmd)
    subprocess.check_call(cmd, shell=True)

    print("Best reconstruction")
    subprocess.check_call(['ShapeWorksView2', f'{working_dir}/0perc.xml'])

    print("Worst reconstruction")
    subprocess.check_call(['ShapeWorksView2', f'{working_dir}/100perc.xml'])

    print()