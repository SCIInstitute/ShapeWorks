## ShapeWorks DeepSSM
DeepSSM is a convolutional neural network (CNN) model that can provide particles directly from unsegmented images after it has been trained. This documentation provides an overview of the DeepSSM process, for a full explanation see: [DeepSSM: A Deep Learning Framework for Statistical
Shape Modeling from Raw Images](publications/DeepSSM.pdf)

The input to the DeepSSM network are unsegmented 3D images of the anatomy of interest and the ouput are the particle-based shape models. DeepSSM requires training examples of image/PBM pairs which are generated via the traditional Shapeworks grooming and optimzation pipeline or otherewise. Once the network has been trained on these examples it can predict the PBM of unseen examples given only image, bypassing the need for labor intensive segmentation, grooming, and optization parameter tuning. 

The benefits of the DeepSSM pipeline include:
* Less Labor  - DeepSSM does not require segmentation, only a bounding box about where the anatomy of interest lies in the image.  
* End-to-end - Does not require separate grooming and optimization steps, it is an end-to-end process. This also reduces memory requirement as images don’t need to be saved after intermediate grooming steps.
* Faster Results - Once a DeepSSM network has been trained, it can be used to predict the shape model on a new image in seconds on a GPU.

The DeepSSM network is implemented in PyTorch and requires a GPU to run efficiently. 

### DeepSSM Steps 

1. Data Augmentation

The first step to creating a DeepSSM model is generating training data. Deep networks require thousands of training instances and so because medical imaging data is typically limited, data augmentaiton is neccesary. The data augmentation process is described here:  [Data Augmentation](DataAugmentation.md)

The data augmentation process involves reducing the PBM's to a low dimensional space via Principle Component Analysis (PCA), preserving a chosen percent of the variation. The PCA scores are saved and used as the labels for DeepSSM prediction. The PCA scores are deterministically mapped back to the PDM using the eigen values and vectors once the DeepSSM model makes a prediciton. 

2. Creation of Data Loaders

The next step is to reformat the data into PyTorch tensors. 80% of the data is randomly selected to be training data and the remaining 20% of the data is used as a validation set. The input images are whitened and and turned into tensors. They can also be optionally downsampled to half their original size to allow for faster training. The corresponding PCA scores are also normalized or whitened to avoid DeepSSM learning to favor the primary modes of variation then turned to tensors. Pytorch data loaders are then created with batch size specified by the user. 

3. Training
4. Testing
5. Evaluation
