## ShapeWorks Data Augmentation 

ShapeWorks includes a Pyhton package for running data augmentation which is useful when a larger amount of data than what is available is required (such as when training DeepSSM).
In data augmentation, realistic fabricated examples are generated based on the available real data in a way that preserves the population statistics.
Here we explain the basic steps to data augmentation.

### Data Augmentation Steps
1. **Embed** - First the true data is embedded into a lower dimensional subspace. This is done by creating a martix which contains all of the real examples, then reducing the dimension via Principle Compnent Analysis (PCA).
2. **Fit a Distribution** - Next a distribution which can be sampled from is fit to the examples embedded in the low-dimension subspace.
3. **Sample from the Distribution** - Samples are randomly drawn from this distribution and the closest real example to each sample is saved.
4. **Project Samples** - The embedded samples are then projected back to data space, providing new generated examples.
5. **Complete Sample Generation** - The closest real example to each sample is then used to generate any part of the sample that is still missing. 
For example, if particles are used in embedding and sampling, the closest real example can be used to generate a corresponding image for the samples particles to provide a complete generated example.

### Embedding

### Sampling

#### Mutlivariate Gaussian Distribution

#### Mixture of Multivariate Gaussians Distribution

#### Kernel Density Estimate Distribution

### Possible Pipelines

#### Particle Based Data Augmentation

#### Image and Particle Based Data Augmentation 




