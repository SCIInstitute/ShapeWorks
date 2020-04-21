
# Datasets

## Requirements

Each dataset must have a **License.txt** which descibes the terms of use and citation requirements for the data.  
You can see an example in the Left Atrium dataset [License](http://cibc1.sci.utah.edu:8080/#item/5e5600298886bab730d72cf5).

Data should be organized into appropriately named directories: **images**, **segmentations**, **meshes**, **landmarks**, **distance_transforms**, **shape_models**.  

*Note that each dataset will likely only have a subset of these directories. For example, the ellipsoid dataset only has a segmentations directory.*  

### images/ 
contents: 3D images of the data  
file types: nrrd  

### segmentations/
contents: 3D segmentations of the data  
file types: nrrd  

### meshes/
contents: meshes  
file types: ply  

### landmarks/
contents:  
file types:  

### distance_transforms/
contents: 3D distance transform volumes  
file types: nrrd  

### shape_models/
contents: example shape models and parameters used to generate them  
file types: xml  

One subfolder per model stores all of the particles files  
For example:  
- shape_models/
  - example_64.xml
  - example_64/
    - \*.particles

## Uploading a Dataset (developers only)

1. Create an account on the [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/#?dialog=register)
2. Ask Oleks to add you to the list of developers. (oleks@sci.utah.edu)
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

## API

### DatasetUtils.downloadDataset(datasetName, destinationPath='.', asZip = True)  
- Parameters:   
  - **datasetName** is one of 'ellipsoid', 'ellipsoid_fd', 'femur', 'left_atrium'  
  - **destinationPath** is where the zip file or folder will go once it is downloaded  
  - **asZip** toggles whether to download as zip or download individual files   
- Returns: True on success and False on failure  

### DatasetUtils.uploadNewDataset(datasetName, datasetPath)
- Parameters:   
  - **datasetName** is the name the dataset will have on the data portal 
  - **datasetPath** is the path to the root folder of the dataset on the local file system  
- Returns: True on success and False on failure  

### DatasetUtils.getDatasetList()  
- Returns: a list of all existing datasets on the data portal  
