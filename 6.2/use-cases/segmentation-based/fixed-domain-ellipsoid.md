# Fixed Domains Ellipsoid: Shape Model on New Shapes

## What is the Use Case? 
The `ellipsoid_fd` use case is designed to demonstrate the functionality of the fixed domain of ShapeWorks. Fixed domains is used in cases where we need to place correspondences on new shapes using a pre-existing shape model. In this example, we use a dataset which contains a previously generated shape model on ellipsoids (see [Ellispoid Use Case](ellipsoid.md)) and prepped segmentations of five new ellipsoids.

![Fixed Domains](../../img/use-cases/fixed-domains.png)

## Grooming Steps
This use case assumes that the new samples are already aligned with the existing shape model. The only grooming step is computing the signed distance transform for each new segmentation.

Below are the pre-existing (first fifteen) and new (last five) groomed distance transforms.
![Fixed domain ellipsoid distance transforms](../../img/use-cases/ellipsoids_fd_input.png)

## Relevant Arguments
[--tiny_test](../use-cases.md#-tiny_test)

## Optimization Parameters
The python code for the use case calls the `optimize` command of ShapeWorks, which requires that the optimization parameters are specified in a python dictionary. Please refer to [Parameter Dictionary in Python](../../workflow/optimize.md#parameter-dictionary-in-python) for more details. 
Below are the default optimization parameters for this use case. Note the `number_fixed_domains` and `fixed_domain_model_dir` parameters.

```python
{
        "number_of_particles": 128,
        "use_normals": 0,
        "normal_weight": 15.0,
        "checkpointing_interval": 200,
        "keep_checkpoints": 0,
        "iterations_per_split": 100,
        "optimization_iterations": 2000,
        "starting_regularization": 100,
        "ending_regularization": 0.1,
        "recompute_regularization_interval": 2,
        "domains_per_shape": 1,
        "domain_type": 'image',
        "relative_weighting": 15,
        "initial_relative_weighting": 0.05,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0,
        "number_fixed_domains": len(file_list_dts),
        "fixed_domain_model_dir": shape_model_dir,
        "mean_shape_path": mean_shape_path
}
```

In `ellipsoid_fd.py`, the following is defined.

- `fileListDT` is the list of distance transforms for the existing shape model
- `shapemodelDir` is the directory path to the new shape model
- `meanShapePath` is the path to the mean (average) shape particles to be used for initializing correspondences on the new shape

## Analyzing Shape Model

Here we can see the optimized particles for the new samples (numbers 15-19). The particles for the original samples (0-14) have remained the same.
![Ellipsoid FD Optimization](../../img/use-cases/ellipsoid_fd.png)

The primary mode of variation is along the x-axis as it was before adding the new shapes.
![Ellipsoid FD Mode 1](https://sci.utah.edu/~shapeworks/doc-resources/gifs/ellipsoid_fd_mode1.gif)

