# Documentation ToDo List


## To add/edit for docs

- [ ] `index.md#shapeworks-in-action`: Add the box bump examples and reference our benchmark study. 
- [ ] `index.md#shapeworks-in-action`: Add video for illustration
- [ ] `index.md#with-shapeworks-you-can`: show case ShapeWorks in different studies (e.g., ortho and cardilogy) and cite relevant papers
- [ ] `index.md` and repo readme: Showcase our SSM benchmark study
- [ ] `users/papers.md`: Add links to the papers and update the list of papers with recent ones
- [x] `about/release-notes.md`: Fix links in release notes
- [ ] `about/team.md`: May be some pics
- [x] `dev/autodoc.md`: Which dir to use for autodoc for commands? 
- [ ] Spell and grammar check all markdown files 
- [ ] `use-cases/ellipsoid.md`: Add a figure showing some samples of the dataset that highlight the mode of variation
- [ ] Be consistent: multi-scale vs multiscale in the md files
- [ ] Be consistent: we vs you (e.g., in use cases)
- [ ] Add illustrating images to the groom steps in groom and specific use cases
- [ ] Update md files for groom/optimize and use cases to reflect the new workflow (icp, bounding box, crop) that does not apply explicitly resample images/segmentations beyond the isoresample step and use transforamtions as input to the optimization
- [ ] For groom and use cases, update the reference selection documentation to reflect any recent updates in this process (e.g., meshes, pairwise distance matrix ... etc)
- [ ] `workflow/XX.md`: Review and edit the workflow md files. Add/update the decription of single vs multiscale.
- [ ] `use-cases/ellipsoid.md`: Add video/snapshots for the optimized model and groomed data
- [ ] `use-cases/XX.md`: Update optimization parameters based on Examples/Python
- [ ] `use-cases/XX.md`: Update use cases to reflect multi-scale being integrated into the optimizer


## To investigate for docs
- [ ] How can we use repo_url in markdown files to link to repo files? Currently, we use hard code it whenever we link to a page: https://github.com/SCIInstitute/ShapeWorks/tree/master
- [ ] How to use google_analytics?

## For ShapeWorks repo

- [ ] Change the autodoc dir for commands in github action and release, see 'dev/autodoc.md'
- [ ] Update `DocumentationUtilsPackage` to generate md for mkdocs (ShapeworksCommand.md)
- [ ] Add `mkdocs build` for autodoc (deploy) to github actions and release
- [x] When a use case launches Studio, does it load groomed data? It does, based on the launch function that takes in distance transforms, local, and world point files.

## Misc
- [ ] Add link to documentation http://sciinstitute.github.io/ShapeWorks/ to shapeworks.sci.utah.edu
- [ ] Prepped vs groomed? Be consistent in documentation, tags for RunUseCase, and output folders from running use cases
