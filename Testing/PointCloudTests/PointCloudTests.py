import os
import sys


def correct_put(fn, inp, out, rbfs):
    f = open(fn, 'r')
    xtra = []
    for line in f:
        s = line.rstrip('\n').split(' ')
        if(s[0] != 'input:' and s[0] != 'output:' and s[0] != 'input_rbfshape:'):
            xtra.append(s)

    f.close()

    f = open(fn, 'w')
    f.write('input: ' + inp + '\n')
    f.write('output: ' + out + '\n')
    if(rbfs != ''):
        f.write('input_rbfshape: ' + rbfs + '\n')
    for line in xtra:
        for i in range(len(line)):
            if i != len(line)-1:
                f.write(line[i] + ' ')
            else:
                f.write(line[i] + '\n')
    f.close()

def read_in(fn):
    f = open(fn, 'r')
    arr = []
    for line in f:
        s = line.rstrip('\n').split(' ')
        for i in range(len(s)):
            s[i] = float(s[i])
        arr.append(s)
    f.close()
    return arr

if __name__ == '__main__':

    '''
    print(f"Arguments count: {len(sys.argv)}")
    for i, arg in enumerate(sys.argv):
        print(f"Argument {i:>6}: {arg}")
    '''

    curr_dir = os.getcwd()
    cmd_path = sys.argv[1]
    if(not cmd_path[len(cmd_path)-1] == '/'):
        cmd_path = cmd_path + '/'

    if not os.path.exists('./output/'):
        os.mkdir('./output/')

    inp = curr_dir + '/input/'
    out = curr_dir + '/output/'

    print("****************************************************\n***********PointCloud Command Unit Tests************\n****************************************************")
    print("Here we test the RBFShapeBuild, RBFShapeEvaluate, \n and RBFShapeGradient functions with sample flat \n surface and sphere inputs in the input directory.")
    print("The outputs will reside in the output directory here.")
    print("****************************************************\n")

    print("****************************************************\n**************Testing for Flat Surface**************\n****************************************************\n")
    correct_put(inp + 'params_flat.txt', inp + 'sample_flat_surface.ptnrms', out + 'sample_flat_surface.rbfshape' ,  '')
    correct_put(inp + 'params_flat_evaluate.txt', inp + 'flat_surface_evals.txt', out + 'sample_flat_surface_evaluations.txt', out + 'sample_flat_surface.rbfshape')
    correct_put(inp + 'params_flat_gradient.txt', inp + 'flat_surface_evals.txt', out + 'sample_flat_surface_gradients.txt', out + 'sample_flat_surface.rbfshape')
    print('Building...')
    os.system(cmd_path + 'bin/RBFShapeBuild input/params_flat.txt')
    print('Evaluating...')
    os.system(cmd_path + 'bin/RBFShapeEvaluate input/params_flat_evaluate.txt')
    print('Evaluating gradiens...')
    os.system(cmd_path + 'bin/RBFShapeGradient input/params_flat_gradient.txt')

    print("\n\n\n*********************************************\n**************Testing for Sphere*************\n*********************************************\n")
    correct_put(inp + 'params_sphere.txt', inp + 'sample_sphere.ptnrms', out + 'sample_sphere.rbfshape' ,  '')
    correct_put(inp + 'params_sphere_evaluate.txt', inp + 'sphere_evals.txt', out + 'sample_sphere_evaluations.txt', out + 'sample_sphere.rbfshape')
    correct_put(inp + 'params_sphere_gradient.txt', inp + 'sphere_evals.txt', out + 'sample_sphere_gradients.txt', out + 'sample_sphere.rbfshape')
    print('Building...')
    os.system(cmd_path + 'bin/RBFShapeBuild input/params_sphere.txt')
    print('Evaluating...')
    os.system(cmd_path + 'bin/RBFShapeEvaluate input/params_sphere_evaluate.txt')
    print('Evaluating gradiens...')
    os.system(cmd_path + 'bin/RBFShapeGradient input/params_sphere_gradient.txt')

    print("\n\n\n*********************************************\n***************Comparing Values**************\n*********************************************\n")
    evals_flat = read_in('output/sample_flat_surface_evaluations.txt')

    for i in range(int(len(evals_flat)/3)):
        assert(abs(evals_flat[i*3][0] - 0) < 1e-4)
        assert(abs(evals_flat[i*3+1][0] - 10) < 1e-4)
        assert(abs(evals_flat[i*3+2][0] + 10) < 1e-4)

    evals_sphere = read_in('output/sample_sphere_evaluations.txt')

    for i in range(int(len(evals_sphere)/3)):
        assert(abs(evals_sphere[i*3][0] - 0) < 1e-4)
        assert(abs(evals_sphere[i*3+1][0] - 10) < 1e-4)
        assert(abs(evals_sphere[i*3+2][0] + 10) < 1e-4)

    grads_flat = read_in('output/sample_flat_surface_gradients.txt')

    for i in range(int(len(grads_flat))):
        assert(abs(grads_flat[i][0] - 1.) < 0.03)

    grads_sphere = read_in('output/sample_sphere_gradients.txt')

    for i in range(int(len(grads_sphere))):
        assert(abs(grads_sphere[i][0] - 1.) < 0.03)

    print("Done!... \n Tests passed!")
