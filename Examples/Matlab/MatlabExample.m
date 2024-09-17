% ShapeWorks Matlab Example


% Add the required directory to MATLAB's dynamic class path
javaaddpath('/path/to/shapeworks/Java');

% Add the shapeworks binary directory to the system path
setenv('PATH', [getenv('PATH') ':/path/to/shapeworks/bin']);

import java.util.ArrayList;
import java.util.HashMap;
import edu.utah.sci.shapeworks.ShapeWorksProject;

% Create a new ShapeWorksProject object
project = ShapeWorksProject();

% Define the list of shapes to be processed (using ArrayList)
filenames = ArrayList();
filenames.add('meshes/ellipsoid_00.vtk');
filenames.add('meshes/ellipsoid_01.vtk');
filenames.add('meshes/ellipsoid_02.vtk');
filenames.add('meshes/ellipsoid_03.vtk');
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
