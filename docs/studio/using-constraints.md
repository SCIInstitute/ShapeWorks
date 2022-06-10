
# Using Constraints in Studio

ShapeWorks Studio supports grooming, optimization and analysis of shapes with constraints, i.e. explicit inclusion and exclusion areas of the surfaces. These constraints can be defined in Studio as cutting-plane or free-form constraints. In other words, when you define a constraint, particles will be limited to the allowed area and will not cross into the disallowed region.

<!-- Image of sliced femurs -->


## Defining Cutting-Plane Constraints

Follow the steps below to define cutting planes on an existing project:

1. Make sure you are on the Data tab.
2. Click the Constraints drop-down menu.  
![Plane 1](../img/studio/plane1.png){: width="300" }
3. ctrl+click 3 points on a shape surface to define a plane.  
![Plane 2](../img/studio/plane2.png){: width="300" }
![Plane 3](../img/studio/plane3.png){: width="300" }
  1. Slide plane along the normal with shift+click.  
  ![Plane 4](../img/studio/plane4.png){: width="300" }
  2. Right click plane point to flip, delete or **copy plane to other shapes**.  
  ![Plane 5](../img/studio/plane5.png){: width="300" }
4. Now the newly defined cutting-plane constraint is active for the desired domain on the desired shape. This will show on the constraints panel. Now particles will not spread to the grayed out area.  
![Plane 6](../img/studio/plane6.png){: width="300" }

## Defining Free-Form Constraints

Follow the steps below to define free-form constraints on an existing project:

1. Make sure you are on the Data tab.
2. Click the Constraints drop-down menu.  
![Plane 1](../img/studio/plane1.png){: width="300" }
3. Click the "Painting Enabled" toggle to checked.  
![FFC 1](../img/studio/ffc1.png)
  1. Select brush size using slider if different from default.  
  ![FFC 2](../img/studio/ffc2.png){: width="300" }
  2. Select whether to paint included or excluded area.  
  ![FFC 3](../img/studio/ffc3.png){: width="300" }
4. Paint on the shape you would like to define the free-form constraint on.  
![FFC 4](../img/studio/ffc4.png){: width="300" }
5. Now the newly-defined free form constraint is active for the desired domain on the desired shape. This will show on the constraints panel. Now particles will not spread to the grayed out area.  
![FFC 5](../img/studio/ffc5.png){: width="300" }

## Removing a Constraint

To remove a constraint:

1. Make sure you are on the Data tab with the constraints drop-down menu open.
2. The planes and free-form constraints previously defined will be listed. For cutting planes, you can also right click a plane point for the same effect.  
![Delete 1](../img/studio/delete1.png){: width="300" }
3. To delete, select the one to delete and click delete.  
![Delete 2](../img/studio/delete2.png){: width="300" }
