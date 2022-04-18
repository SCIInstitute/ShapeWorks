# Peanut: Shape Model of Multiple Domains with Shared Boundaries

## What is the Use Case? 
The `peanut_shared_boundary` use case demonstrates using ShapeWorks tools to perform shape modelings for anatomies with multiple structures (domains), e.g., joints, with shared boundaries to capture inter-domains correlations and interactions. It also demonstrates the shared mesh boundary and contour extraction tools and runs multi-domain (meshes and contours) optimization on the peanut dataset.

The peanut dataset,consists of two aligned spheres, but one of the spheres is subtracted from the other. There exists a shared surface between the two spheres. The radii of the two spheres vary inversely, ie: as one gets bigger the other gets smaller.

## Grooming Steps

1. [**Remeshing**](../../workflow/groom.md#remesh): Meshes are remeshed to ensure uniform vertices.
2. [**Extract Shared Boundary**](../../workflow/groom.md#extract-shared-boundary): In this step, we ingest the two original shapes and output three new shapes, two of which correspond to the original shapes and one for the shared boundary.
3. [**Smoothing**](../../workflow/groom.md#smoothing):Applied laplacian smoothing.
4. [**Extract Contour**](../../workflow/groom.md#extract-contour): The boundary loop of the shared surface is obtained.

## Relevant Arguments
[--use_subsample](../use-cases.md#-use_subsample)
[--num_subsample](../use-cases.md#-use_subsample)
[--tiny_test](../use-cases.md#-tiny_test)