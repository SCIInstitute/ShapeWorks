% ShapeWorks Matlab Example

% Add the required directory to MATLAB's dynamic class path
javaaddpath('/Users/amorris/sci/shapeworks/code/Java');

% Add the shapeworks binary directory to the system path
setenv('PATH', [getenv('PATH') ':/Users/amorris/sci/shapeworks/bin/bin']);


import java.util.ArrayList;
import java.util.HashMap;
import edu.utah.sci.shapeworks.ShapeWorksProject;

% Create a new ShapeWorksProject object
project = ShapeWorksProject();

% Create a set of ellipsoid meshes to be processed, save as STL files
for i=0:3
    % vary radius using i for one of the ellipsoids directions
    [x,y,z]=ellipsoid(0,0,0,50+i*10,50,50);
    fv = surf2patch(x, y, z, 'triangles');
    TR = triangulation(fv.faces, fv.vertices);
    stlwrite(TR, sprintf('ellipsoid_%02d.stl', i));
end


% Define the list of shapes to be processed
filenames = ArrayList();
filenames.add('ellipsoid_00.stl');
filenames.add('ellipsoid_01.stl');
filenames.add('ellipsoid_02.stl');
filenames.add('ellipsoid_03.stl');
project.setShapes(filenames);

% Define grooming parameters (using HashMap)
groomingParameters = HashMap();
groomingParameters.put('alignment_enabled', 'true');
groomingParameters.put('alignment_method', 'Center');
% Add more parameters as needed
project.setGroomingParameters(groomingParameters);

% Define optimization parameters (using HashMap)
optimizationParameters = HashMap();
optimizationParameters.put('number_of_particles', '128');
optimizationParameters.put('initial_relative_weighting', '0.05');
% Add more parameters as needed
project.setOptimizationParameters(optimizationParameters);

% Save the project
disp('Saving project...');
project.save('project.swproj');

% Demonstrate loading the project
disp('Loading project...');
project.load('project.swproj');

% Run the groom, optimize, and analyze steps
disp('Running groom...');
project.groom();
disp('Running optimize...');
project.optimize();
disp('Opening ShapeWorksStudio...');
project.analyze();
