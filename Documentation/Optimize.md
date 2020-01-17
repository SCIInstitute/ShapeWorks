
##Optimization
ShapeWorks uses the processed data groomed to the sign distance transform for optimization step.
ShapeWorks uses the Particle Based Modeling (PBM) approach to constructs a correspondence 
point model of shape, which describes shape variation by choosing a discrete set of corresponding points 
on shape surfaces whose relative positions can be statistically analyzed. Consider a statistical sample of N surface 
representations embeded in a d-dimensional Cartesian space(typically, d= 2, d= 3). A model for shape variation is constructed
by choosing a set of M, d-dimensional points on each of the N surfaces. Each of the points is called a *correspondence*  point
and Collectively, the set of M points is known as the *configuration*.
The PBM algorithm models the correspondence positions as sets of dynamic particles that are constrained to lie on the surface of the sample set.
The optimization is based on the idea of treating correspondence position in configuration space as a random variable, while
simultaneously treating correspondence configuration as a random variable.

Correspondence positions are optimized by gradient descent on an energy function that balances the negative entropy of 
the distribution of particles in configuration space with the positive entropy of the distribution of the configurations
in shape space. The method is to consider ![equation](http://www.sciweavers.org/tex2img.php?eq=z_k%20%20%5Cin%20%20%5CRe%20%5E%7BdM%7D%2C%20k%3D%7B1%2C%202%2C%20...%2C%20N%7D&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)
 both as observations on a dM × 1 vector random
variable Z and as N samples of M observations on N , d × 1 vector random variables ![equation](http://www.sciweavers.org/tex2img.php?eq=X_k&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)
 . The optimization to establish correspondence minimizes the energy function
 
![equation](http://www.sciweavers.org/tex2img.php?eq=Q%20%3D%20H%28Z%29-%20%5Csum_%7Bk%3D1%7D%5EN%20H%28X_k%29%20&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)







where H is an estimation of differential entropy. Minimization of the first term in Q
produces a compact distribution of samples in shape space, while the second term seeks
uniformly-distributed correspondence positions on the shape surfaces for accurate shape
representation. Each term is given in commensurate units of entropy, avoiding the need
for a separate regularization strategy. Because correspondence points in this formulation
are not tied to a specific parameterization, the method operates directly on volumetric
data and extends easily to arbitrary shapes, even nonmanifold surfaces.

The optimization is consists f two part the initialization of correspondence and optimization. 
### Initialization
 One effective approach is to use an iterative, particle splitting strategy, in which the full set of particles is
initialized in a multiscale fashion as follows. First, the PBM system is initialized with a
single particle on each shape that finds the nearest zero of the implicit surface. This sin-
gle particle is then split to produce a new, nearby particle. The two-particle (per shape)
system is then optimized for correspondence until a steady state is reached. The splitting
process, followed by optimization, is then repeated until a specific number of particles
have been produced. Thus, the initialization proceeds simultaneously with the optimization in a multiscale fashion,
 generating progressively more detailed correspondence models with each split.
Typically, we set the numerical parameters for the PBM optimization automatically based on the machine precision.

### Optimization
ShapeWorks has two mode of optimization *single scale* and hierarchical *multi-scale* optimization.
##### Single Scale Optimization
This method takes a fixed number of particles (usually a power of 2 ) and performs the initialization step to independently
spread out particles uniformly on each shape. Then it performs the full optimization using the surface as well as
population based entropy terms of the ShapeWorks optimization and bringing the points on all the shapes into correspondence.
The command to use the single scale optimization is as following with proper parameters
```
SingleScaleShapeWorksRun --data_dir $1 --out_dir $2 --verbosity $3
--number of particles $4 --start_reg $5 --end_reg $6
--procrustes_interval $7
``` 
Here is the list of parameters and their descriptions.
* data_dir: Path to the directory containing the processed data in form of signed distance transform.
* out_dir:  The directory where you need to save the output produced by the ShapeWorks optimization.
* verbosity: (Optional) (default: '2') '0' : almost zero verbosity(error massage only), '1': minimal verbosity( notification of important steps,
 '2': additional details about parameters read from xml and files written, '3': full verbosity.
* start_reg: (Optional) (default:1000) Sets the starting regularization value (usually high value).
* end_reg: (Optional) (default:1e-4) Sets the ending regularization value (usually small value less than 1).
* number of particles: (default: 128) The desire number of particles to be placed in power of 2
* procrustes_interval: (default: 0) The interval between procrustes runs, 0 when procrustes is to be turned off

