import java.util.List;
import java.util.Map;
import edu.utah.sci.shapeworks.ShapeWorksProject;
import java.io.*;

public class JavaExample {
    public static void main(String[] args) {

        // Create a new ShapeWorksProject object
        ShapeWorksProject project = new ShapeWorksProject();

        // Define the list of shapes to be processed
        List<String> filenames = List.of("meshes/ellipsoid_00.vtk", "meshes/ellipsoid_01.vtk",
                "meshes/ellipsoid_02.vtk", "meshes/ellipsoid_03.vtk");
        project.setShapes(filenames);

        // Define grooming parameters
        Map<String, String> groomingParameters = Map.of(
                "alignment_enabled", "true",
                "alignment_method", "Center"
        // Add more parameters as needed
        );
        project.setGroomingParameters(groomingParameters);

        // Define optimization parameters
        Map<String, String> optimizationParameters = Map.of(
                "number_of_particles", "128",
                "initial_relative_weighting", "0.05"
        // Add more parameters as needed
        );
        project.setOptimizationParameters(optimizationParameters);

        System.out.println("Saving project...");
        project.save("project.swproj");

        // demonstrate loading
        System.out.println("Loading project...");
        project.load("project.swproj");

        System.out.println("Running groom...");
        project.groom();
        System.out.println("Running optimize...");
        project.optimize();
        System.out.println("Opening ShapeWorksStudio...");
        project.analyze();
    }
}