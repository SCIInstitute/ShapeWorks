# Free-Form Constraints


The free-form constraint (FFC) functionality allows the definition of arbitrary areas of interest on domains. These areas are defined using a member point and any number of boundaries loops that define exclusion areas. The area where the member point lives is where particles are allowed to live, and will not step into exclusion areads deliniated by the boundary loops.

For example, for a use case with d domains, one would add an xml parameter <num_ffcs_per_input> with d numbers denoting how many FFCs there will be per input. Then each of these will be defined as filenames under the tag <ffcs>. Each file will have the following format:

```
query
[member_point]
boundary_pts
[boundary_pt1]
[boundary_pt2]
...
[boundary_ptn]
boundary_pts
[boundary_pt1]
[boundary_pt2]
...
[boundary_ptn]
```
