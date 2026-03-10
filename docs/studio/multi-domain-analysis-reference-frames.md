# Multi-Domain Analysis: Reference Frames and MCA

## Overview

When analyzing multi-domain (multi-anatomy) shape models, ShapeWorks provides two independent controls that affect how variation is viewed:

1. **Alignment Reference Domain** — controls the coordinate frame particles are transformed into
2. **Mode of Variation** (Multi-Level Component Analysis) — decomposes variation into shape and pose subspaces for PCA visualization

These are separate mechanisms that operate at different stages of the analysis pipeline.

## Alignment Reference Domain

The alignment dropdown (visible only for multi-domain projects) controls how particles are transformed from local to world coordinates.

| Setting | Effect |
|---------|--------|
| **Global Alignment** | Applies the global grooming alignment transform (centering, ICP, or landmark) and Procrustes (if enabled) across all domains together. This is the default. |
| **Local Alignment** | Each domain is aligned independently using the world particles from optimization (which include per-domain grooming alignment and Procrustes, if enabled). Removes all inter-domain pose information. Best for isolating per-domain shape variation. |
| **Specific Domain** (e.g. "femur") | Applies that domain's grooming alignment transform and Procrustes (if enabled), aligning all domains relative to the chosen reference. Shows how other anatomies move/change relative to that domain. |

Changing the alignment recomputes the particle positions used for all downstream analysis: PCA modes, group comparisons, and group p-values.

## Mode of Variation (Multi-Level Component Analysis)

When multiple domains are present, the PCA tab shows a "Mode of Variation" selector with three options. These use Multi-Level Component Analysis (MCA), which decomposes multi-domain particle data into two orthogonal subspaces before running PCA.

| Mode | What it shows |
|------|---------------|
| **Shape and Pose** | Standard PCA on all particles. Shape and pose variation are entangled. |
| **Shape Only** | PCA on shape-deviation data. Per-domain centroids are subtracted per subject, removing translational pose differences. Shows morphological variation only. |
| **Relative Pose Only** | PCA on per-domain centroid data. Shows how domains move relative to each other, with per-domain shape variation removed. |

### How MCA works

For each subject:

1. Compute the centroid (mean position) of each domain's particles
2. **Shape Only**: subtract each domain's centroid from its particles, isolating per-domain morphological variation
3. **Relative Pose Only**: keep only the per-domain centroids (relative to the grand mean), capturing translational relationships between domains

Standard PCA is then run on each decomposed matrix independently.

## Interaction of Alignment and MCA

MCA always operates on the particles produced by the current alignment setting. The combination of alignment and MCA mode determines what variation you see:

| Alignment | Shape and Pose | Shape Only | Relative Pose Only |
|---|---|---|---|
| **Global** | Standard PCA, meaningful | Meaningful -- isolates shape | Meaningful -- isolates translational pose |
| **Specific Domain** | Meaningful | Shape + rotational articulation (see below) | Translational pose only (see below) |
| **Local** | Already pose-free | Redundant -- Procrustes already removes pose | Not meaningful -- centroids are in unrelated frames |

### Specific Domain Alignment + MCA

When aligning to a specific domain (e.g. pelvis), all particles are in that domain's coordinate frame. The reference domain is stable while other domains (e.g. femur) move relative to it through both translation and rotation.

**Shape Only** in this configuration shows real morphological changes *plus* rotational articulation between domains. MCA only subtracts per-domain centroids (translation), so rotation remains and appears as "shape" variation.

**Relative Pose Only** shows only translational shifts between domains. If a domain rotates but its centroid stays roughly in place, the rotation is invisible to this mode.

This is the fundamental limitation of MCA's centroid-based decomposition -- rotation is not captured by the pose component and contaminates the shape component.

### Local Alignment + MCA is redundant

With Local Alignment, each domain already has its own alignment (rotation + translation + scale). The domains end up in unrelated coordinate frames. MCA on top of this adds no useful information:

- **Relative Pose Only**: Computes centroids in unrelated frames -- the "relative pose" is arbitrary
- **Shape Only**: Subtracts centroids, which is redundant since Procrustes already aligned translations

### MCA vs Local Alignment for shape isolation

Both MCA's "Shape Only" mode and Local Alignment aim to isolate per-domain shape variation, but they differ in thoroughness:

- **Local Alignment** applies full per-domain alignment (rotation + translation + scale), providing thorough isolation of per-domain shape from inter-domain pose
- **MCA Shape Only** only subtracts per-domain translation (centroid). Rotation and scale differences between domains remain

Local Alignment is generally the more complete approach for isolating shape variation.

## Interaction with Group P-Values

**The alignment setting correctly affects group p-values.** Changing alignment triggers full recomputation of statistics and invalidation of cached p-values.

**MCA mode does not affect group p-values.** The p-value computation always uses the combined particle data regardless of which MCA mode is selected. The MCA decomposed matrices are only used for PCA mode visualization. This is correct because:

- **Relative Pose Only** modes reduce to one rigid translation per domain -- all particles in a domain share the same displacement vector, so there is no per-particle variation to compute p-values on
- **Shape Only** modes only remove per-domain translation, which is a less thorough version of what **Local Alignment** already provides

To get per-particle p-values that reflect shape-only differences, use **Local Alignment** from the alignment dropdown instead of MCA.

## Pre-Aligned Data (No Grooming Alignment)

When data has been pre-aligned outside of ShapeWorks and grooming alignment is skipped, ShapeWorks automatically detects this and preserves the original spatial relationships between domains during analysis. Without this, Procrustes alignment would center all domains to the origin, causing them to overlap incorrectly.

This correction applies automatically -- no user action is needed. When grooming alignment (centering, ICP, or landmark) is performed within ShapeWorks, this behavior is disabled as it is not needed.
