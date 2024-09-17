package edu.utah.sci.shapeworks;

import java.io.*;
import java.util.*;

public class ShapeWorksProject {
    private List<String> shapes;
    private Map<String, String> groomingParameters;
    private Map<String, String> optimizationParameters;

    public void setShapes(List<String> filenames) {
        this.shapes = filenames;
    }

    public void setGroomingParameters(Map<String, String> groomingParameters) {
        this.groomingParameters = groomingParameters;
    }

    public void setOptimizationParameters(Map<String, String> optimizationParameters) {
        this.optimizationParameters = optimizationParameters;
    }

    public void save(String filename) {
        try (PrintWriter writer = new PrintWriter(new FileOutputStream(filename))) {
            writer.println("{");
            writer.println("\"data\": [");
            for (int i = 0; i < shapes.size(); i++) {
                writer.println(
                        " {\"shape_" + (i + 1) + "\": \"" + shapes.get(i) + "\"}" + (i < shapes.size() - 1 ? "," : ""));
            }
            writer.println("],");
            writer.println("\"groom\": {");
            writer.println(" \"1\": {");
            int i = 0;
            for (Map.Entry<String, String> entry : groomingParameters.entrySet()) {
                writer.println("   \"" + entry.getKey() + "\": \"" + entry.getValue() + "\""
                        + ((i < groomingParameters.size() - 1) ? "," : ""));
                i++;
            }
            writer.println(" }");
            writer.println("},");
            writer.println("\"optimize\": {");
            i = 0;
            for (Map.Entry<String, String> entry : optimizationParameters.entrySet()) {
                writer.println(" \"" + entry.getKey() + "\": \"" + entry.getValue() + "\""
                        + ((i < optimizationParameters.size() - 1) ? "," : ""));
                i++;
            }
            writer.println("},");
            writer.println("\"project\": {");
            writer.println(" \"version\": \"2\"");
            writer.println("}");
            writer.println("}");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void load(String filename) {
        try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
            StringBuilder jsonBuilder = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                jsonBuilder.append(line.trim());
            }
            String json = jsonBuilder.toString();

            // Basic JSON parsing (Note: This is a simplistic parser; real-world scenarios
            // should use a complete library)
            this.shapes = new ArrayList<>();
            this.groomingParameters = new HashMap<>();
            this.optimizationParameters = new HashMap<>();

            // Parse data
            int dataStart = json.indexOf("\"data\": [") + 8;
            int dataEnd = json.indexOf("]", dataStart);
            String dataContent = json.substring(dataStart, dataEnd);
            String[] dataEntries = dataContent.split("},\\s*\\{");
            for (String entry : dataEntries) {
                shapes.add(entry.split(":")[1].replace("\"", "").replace("}", "").replace("{", "").trim());
            }

            // Parse groom
            int groomStart = json.indexOf("\"groom\": {") + 10;
            int groomEnd = json.indexOf("}", groomStart) + 1;
            String groomContent = json.substring(groomStart, groomEnd);
            String[] groomEntries = groomContent.split(",");
            for (String entry : groomEntries) {
                String[] keyValue = entry.split(":");
                if (keyValue.length == 2) {
                    groomingParameters.put(keyValue[0].replace("\"", "").trim(), keyValue[1].replace("\"", "").trim());
                }
            }
            // Parse optimize
            int optimizeStart = json.indexOf("\"optimize\": {") + 12;
            int optimizeEnd = json.indexOf("}", optimizeStart);
            String optimizeContent = json.substring(optimizeStart, optimizeEnd);
            String[] optimizeEntries = optimizeContent.split(",");
            for (String entry : optimizeEntries) {
                String[] keyValue = entry.split(":");
                if (keyValue.length == 2) {
                    optimizationParameters.put(keyValue[0].replace("\"", "").trim(),
                            keyValue[1].replace("\"", "").trim());
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void groom() {
        runSystemCommand("shapeworks groom --name project.swproj");
    }

    public void optimize() {
        runSystemCommand("shapeworks optimize --name project.swproj");
    }

    public void analyze() {
        runSystemCommand("shapeworksstudio project.swproj");
    }

    private void runSystemCommand(String command) {
        try {
            Process process = Runtime.getRuntime().exec(command);
            process.waitFor();
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
}