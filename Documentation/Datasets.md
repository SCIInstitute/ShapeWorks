
# Datasets

## Requirements

Each dataset must have a README.txt and License.txt


## Uploading a Dataset

1. Create an account on the [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/#?dialog=register)
2. Ask Oleks to add you to the list of developers. (oleks@sci.utah.edu)
3. Prepare dataset folder with README.txt and License.txt
4. Use `DatasetUtils.uploadNewDataset(datasetName, datasetPath)` to upload.
5. Name the dataset all lowercase with underscores separating words similar to: 'ellipsoid', 'ellipsoid_fd', 'left_atrium', 'femur'

Example file structure:
- TestEllipsoids/
  - ellipsoid/
    - README.txt
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
    - README.txt
    - License.txt
    - Ellipsoids_Prepped/
      - bunch of \*.nrrd
    - Ellipsoids_UnPrepped/
      - bunch of \*.nrrd
