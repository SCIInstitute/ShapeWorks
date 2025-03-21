# Documentation ToDo List


## To add/edit for docs

- [x] `index.md#shapeworks-in-action`: Add the box bump examples and reference our benchmark study. 
- [ ] `index.md#shapeworks-in-action`: Add video for illustration
- [x] `index.md#with-shapeworks-you-can`: show case ShapeWorks in different studies (e.g., ortho and cardilogy) and cite relevant papers
- [x] `index.md` and repo readme: Showcase our SSM benchmark study
- [ ] `users/papers.md`: Add links to the papers and update the list of papers with recent ones
- [x] `about/release-notes.md`: Fix links in release notes
- [x] `about/team.md`: May be some pics
- [x] `dev/autodoc.md`: Which dir to use for autodoc for commands? 
- [x] Spell and grammar check all markdown files 
- [ ] `use-cases/ellipsoid.md`: Add a figure showing some samples of the dataset that highlight the mode of variation
- [x] Be consistent: multi-scale vs multiscale in the md files
- [ ] Be consistent: we vs you (e.g., in use cases)
- [ ] Add illustrating images to the groom steps in groom and specific use cases
- [ ] Update md files for groom/optimize and use cases to reflect the new workflow (icp, bounding box, crop) that does not apply explicitly resample images/segmentations beyond the isoresample step and use transforamtions as input to the optimization
- [ ] For groom and use cases, update the reference selection documentation to reflect any recent updates in this process (e.g., meshes, pairwise distance matrix ... etc)
- [x] `workflow/XX.md`: Review and edit the workflow md files. Add/update the decription of single vs multiscale.
- [ ] `use-cases/ellipsoid.md`: Add video/snapshots for the optimized model and groomed data
- [x] `use-cases/XX.md`: Update optimization parameters based on Examples/Python
- [x] `use-cases/XX.md`: Update use cases to reflect multi-scale being integrated into the optimizer
- [x] Add documentation for femur_mesh and lumps use cases
- [ ] `use-cases/left-atrium.md`:Check the left atrium use case for saving txt files to carry over images to reflect recent changes (consolidation)
- [ ] Consolidate docs/pdfs
- [x] Add to use cases how to run with prepped data
- [ ] Add mean and PCA visuals for ellipsoid and fixed domain ellipsoid use cases
- [x] Review and edit ellipsoid use cases
- [x] Review and edit femur use case
- [x] Review and edit right ventricle use case
- [x] Once meshes work, update the femur use case to indicate that this is optional and add femur_mesh use case
- [x] Instructions on how to add videos to the documentation (thumbnails that link to youtube)
- [x] Instructions to add a new use case
- [ ] Revise the right ventricle use case after release it. Might need more results to illustrate group differences.
- [ ] Instructions for Doxygen and building doxygen.
- [ ] `dev/commands.md`:How to add shapeworks commands?
- [ ] `dev/gh-actions.md`: getting started with github actions
- [ ] `dev/python-apis.md`: How to Add Python APIs?
- [ ] `dev/tests.md`: How to Add and Run Unit Tests?
- [x] Remove all hard-coded links outside docs (relative paths) except for (1) web links (e.g. segmentation softwares, markdown edits ... etc) and (2) shapeworks releases, and shapeworks data portal.

## To investigate for docs
- [x] How can we use repo_url in markdown files to link to repo files? -- not needed any more, we won't include any links outside docs except for weblinks, shapeworks releases and data portal.
- [ ] How to use google_analytics?

## For ShapeWorks repo

- [x] Change the autodoc dir for commands in github action and release, see 'dev/autodoc.md'
- [x] Update `DocumentationUtilsPackage` to generate md for mkdocs (ShapeworksCommand.md)
- [x] Add `mkdocs build` for autodoc (deploy) to github actions and release
- [x] When a use case launches Studio, does it load groomed data? It does, based on the launch function that takes in distance transforms, local, and world point files.
- [ ] Review and edit comments in the <use-case>.py
- [ ] A fixed domain use case that include grooming the new samples
- [ ] Revisit the fixed domain use case, why do we need the path for the mean shape? why not use the mean of the fixed domains?
- [ ] Release and update the right ventricle use case


## Misc
- [ ] Add link to documentation http://sciinstitute.github.io/ShapeWorks/ to shapeworks.sci.utah.edu
- [ ] Prepped vs groomed? Be consistent in documentation, tags for RunUseCase, and output folders from running use cases


