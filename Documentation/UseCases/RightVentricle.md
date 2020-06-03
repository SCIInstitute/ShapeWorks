##  Right Ventricle Use Case

The goal of this use case is, study the group difference of right ventricle between control and patients.
 In this study, we have two sets of data, the control group with six subjects and patient group consists of 26 subjects.
  For each group, we have diastole and systole segmentation. The goal is to study the variation of the systole and diastole in two groups.
   For this purpose, we pre-process diastole and systole data with the groom utils of ShapeWorks and then optimize the particle system for them, independently.
    Then using the group difference analysis tool of ShapeWorks we can study the difference of variation in control and patient group from diastole to systole stage of heart.

### Grooming
For a description of the grooming tools and parameters, see: [Groom.md](https://github.com/SCIInstitute/ShapeWorks/blob/master/Documentation/Groom.md)
1. Isotropic Resampling - Segmentations are resampled to have uniform voxel spacing.
2. Apply Padding- Segmentations which lie on the image boundary will have a hole on that intersection. Padding is added to the segmentations to prevent this.
3. Center of Mass Alignment - Center of mass alignment is performed before aligning the samples to a reference. This factors out translations reducing the risk of misalignment and allows for a median sample to be selected as the reference.
4. Reference Selection - The reference is selected by first getting the mean distance transform of the segmentations, then selecting the sample closest to that mean.
5. Rigid Alignment - All of the segmentations are now aligned to the reference using rigid alignment.
6. Crop - The segmentations are cropped so that all of the samples are within the same bounding box.
7. Distance Transform - Finally, the distance transform is taken and the data is ready for ShapeWorks optimize.

![RV_groom](images/RV_groom.png)