
# Datasets

## Requirements

Each dataset must have a **License.txt** which descibes the terms of use and citation requirements for the data.  
You can see an example in the Left Atrium dataset [License](http://cibc1.sci.utah.edu:8080/api/v1/file/5e5600298886bab730d72cf6/download).

## Uploading a Dataset (developers only)

1. Create an account on the [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/#?dialog=register)
2. Ask Oleks to add you to the list of developers. (oleks@sci.utah.edu)
3. Prepare dataset folder with License.txt
4. Use `DatasetUtils.uploadNewDataset(datasetName, datasetPath)` to upload.
5. Name the dataset all lowercase with underscores separating words similar to: 'ellipsoid', 'ellipsoid_fd', 'left_atrium', 'femur'

Example file structure:
- TestEllipsoids/
  - ellipsoid/
    - License.txt
    - Ellipsoids_Prepped/
      - bunch of \*.nrrd
    - Ellipsoids_UnPrepped/
      - bunch of \*.nrrd

Example python upload usage:
```
import DatasetUtils
DatasetUtils.uploadNewDataset('ellipsoid', 'TestEllipsoids/ellipsoid/')
```

When calling `DatasetUtils.downloadDataset('ellipsoid')`, you will get a zip file with the following structure:  
- ellipsoid.zip
  - ellipsoid/
    - License.txt
    - Ellipsoids_Prepped/
      - bunch of \*.nrrd
    - Ellipsoids_UnPrepped/
      - bunch of \*.nrrd

## API

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
