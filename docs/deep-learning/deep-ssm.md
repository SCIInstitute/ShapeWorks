## ShapeWorks DeepSSM
DeepSSM is a convolutional neural network (CNN) model that can provide particles directly from unsegmented images after it has been trained. This documentation provides an overview of the DeepSSM process, for a full explanation see: [DeepSSM: A Deep Learning Framework for Statistical
Shape Modeling from Raw Images](https://arxiv.org/abs/1810.00111).

The input to the DeepSSM network are unsegmented 3D images of the anatomy of interest and the output are the particle-based shape models. DeepSSM requires training examples of image/PBM pairs which are generated via the traditional Shapeworks grooming and optimization pipeline or otherwise. Once the network has been trained on these examples it can predict the PBM of unseen examples given only images, bypassing the need for labor intensive segmentation, grooming, and optimization parameter tuning. 

The benefits of the DeepSSM pipeline include:
* Less Labor  - DeepSSM does not require segmentation, only a bounding box about where the anatomy of interest lies in the image.  
* End-to-end - Does not require separate grooming and optimization steps, it is an end-to-end process. This also reduces memory requirement as images don’t need to be saved after intermediate grooming steps.
* Faster Results - Once a DeepSSM network has been trained, it can be used to predict the shape model on a new image in seconds on a GPU.

The DeepSSM network is implemented in PyTorch and requires a GPU to run efficiently. 

### DeepSSM Steps 

#### 1. Data Augmentation

The first step to creating a DeepSSM model is generating training data. Deep networks require thousands of training instances and so because medical imaging data is typically limited, data augmentation is necessary. The data augmentation process is described here:  [data-augmentation.md](DataAugmentation.md).

The data augmentation process involves reducing the PBM's to a low dimensional space via Principal Component Analysis (PCA), preserving a chosen percent of the variation. The PCA scores are saved and used as the labels for DeepSSM prediction. The PCA scores are deterministically mapped back to the PDM using the eigenvalues and vectors once the DeepSSM model makes a prediction. 

#### 2. Creation of Data Loaders

The next step is to reformat the data into PyTorch tensors. 80% of the data is randomly selected to be training data and the remaining 20% of the data is used as a validation set. The input images are whitened and turned into tensors. They can also be optionally downsampled to a smaller size to allow for faster training. The corresponding PCA scores are also normalized or whitened to avoid DeepSSM learning to favor the primary modes of variation then turned to tensors. Pytorch data loaders are then created with batch size specified by the user. 

#### 3. Training

PyTorch is used in constructing and training DeepSSM. The network architecture is defined to have five convolution layers followed by two fully connected layers as illustrated in the figure below. Parametric ReLU activation is used and the weights are initialized using Xavier initialization. The network is trained for the specified number of epochs using Adam optimization to minimize the L2 loss function with a learning rate of 0.0001. The average training and validation error are printed and logged each epoch to determine convergence.

![DeepSSM Architecture](../img/deep-learning/Architecture.pdf)

#### 4. Testing

The trained model is then used to predict the PCA score from the images in the test set. These PCA scores are then un-whitened and mapped back to the particle coordinates using the eigenvalues and eigenvectors from PCA. Thus a PDM is acquired for each test image.

#### 5. Evaluation

To evaluate the accuracy of DeepSSM output, we compare a mesh created from segmentation to a mesh created from the predicted PDM. To obtain the original mesh, we use the ShapeWorks MeshFromDistanceTransforms command to get a mesh from the distance transform created from the true segmentation. To obtain the predicted mesh, we use the ShapeWorks ReconstructSurface command with the mean and predicted particles to reconstruct a surface.

We then compare the original mesh to the predicted mesh via surface-to-surface distance. To find the distance from the original to the predicted, we consider each vertex in the original and find the shortest distance to the surface of the predicted. This process is not symmetric as it depends on the vertices of one mesh, so the distance from the predicted to the original will be slightly different. We compute the Hausdorff distance which takes the max of these vertex-wise distances to return a single value as a measure of accuracy. We also consider the vertex-wise distances as a scalar field on the mesh vertices and visualize them as a heat map on the surface. This provides us with a way of seeing where the predicted PDM was more or less accurate.

TODO: Example of heat map here
