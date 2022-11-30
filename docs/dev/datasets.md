
# How to Add New Datasets?

[ShapeWorks Data Portal](https://girder.shapeworks-cloud.org/)

## Dataset Requirements

Each dataset must have a **License.txt**, which describes the terms of use and citation requirements for the data.  
You can see an example in the Left Atrium dataset [License](https://girder.shapeworks-cloud.org/#item/5e5600298886bab730d72cf5).

Data should be organized into appropriately named directories: **images**, **segmentations**, **meshes**, **landmarks**, **distance_transforms**, **shape_models**.  

*Note that each dataset will likely only have a subset of these directories. For example, the ellipsoid dataset only has a segmentations directory.*  

### images/ 
contents: 3D images of the data  
file types: [itk supported image formats](https://insightsoftwareconsortium.github.io/itk-js/docs/image_formats.html)  

### segmentations/
contents: 3D segmentations of the data  
file types: [itk supported image formats](https://insightsoftwareconsortium.github.io/itk-js/docs/image_formats.html)  

### meshes/
contents: meshes  
file types: ply, vtk, stl  

### landmarks/
contents: sample specific files containing anatomical landmarks noted by the user  
file types: csv with 4 columns: *x*, *y*, *z*, *label*  

### distance_transforms/
contents: 3D distance transform volumes  
file types: [itk supported image formats](https://insightsoftwareconsortium.github.io/itk-js/docs/image_formats.html)  

### shape_models/
contents: example shape models and parameters used to generate them  
file types: xml  

One subfolder per model stores all of the particles files  
For example:  
- shape_models/
  - example_64.xml
  - example_64/
    - \*.particles

## Uploading a Dataset

!!! important
    Uploaded datasets are only restricted to ShapeWorks developers.

1. Create an account on the [ShapeWorks Data Portal](https://girder.shapeworks-cloud.org/#?dialog=register)
2. Ask Alan to add you to the list of developers. (amorris@sci.utah.edu)
3. Prepare dataset directory with License.txt
4. Use `DatasetUtils.uploadNewDataset(datasetName, datasetPath)` to upload.
5. Name the dataset all lowercase with underscores separating words. For example: 'ellipsoid', 'ellipsoid_fd', 'left_atrium', 'femur'

Example file structure:
- TestFolder/
  - dataset_name/
    - License.txt
    - images/
    - segmentations/
    - meshes/
    - landmarks/
    - distance_transforms/
    - shape_models/

Example python upload usage:
```
import DatasetUtils
DatasetUtils.uploadNewDataset('dataset_name', 'TestFolder/dataset_name/')
```

When calling `DatasetUtils.downloadDataset('dataset_name')`, you will get a zip file with the following structure:  
- dataset_name.zip
  - dataset_name/
    - License.txt
    - images/
    - segmentations/
    - meshes/
    - landmarks/
    - distance_transforms/
    - shape_models/

## Datasets API

### DatasetUtils.downloadDataset(datasetName, destinationPath='.', asZip = True, fileList = None)  
- Parameters:   
  - **datasetName** is one of the names returned by `DatasetUtils.getDatasetList()`  
  - **destinationPath** is where the zip file or folder will go once it is downloaded  
  - **asZip** toggles whether to download as zip or download individual files. (providing a fileList disables this functionality)   
  - **fileList** is a list of files to download. Example for femur: `['images/m03_1x_hip.nrrd', 'distance_transforms/m03_L_femur.ply']`   
- Returns: True on success and False on failure  

### DatasetUtils.uploadNewDataset(datasetName, datasetPath)
- Parameters:   
  - **datasetName** is the name the dataset will have on the data portal 
  - **datasetPath** is the path to the root folder of the dataset on the local file system  
- Returns: True on success and False on failure  

### DatasetUtils.getDatasetList()  
- Returns: a list of all existing datasets on the data portal  

### DatasetUtils.getFileList(datasetName)  
- Parameters:  
  - **datasetName** is one of the names returned by `DatasetUtils.getDatasetList()`  
- Returns: a list of all files in the specified dataset on the data portal  
