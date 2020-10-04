# Modifying Datasets

When modifying use case datasets you must adhere to the use dataset constrol system in order to prevent everyone's use cases from breaking.  
If you want to change the dataset in a way that breaks the use case script, you must upload a new version of the dataset.  

Example:  
I want to change the `groomed` folder in the ellipsoid use case to `groomed_stuff`.  
The current python script looks for a `groomed` folder so this change will break it.  
Because of this, I upload a new version of ellipsoid with my change, leaving the old version untouched.
Then I can proceed to change the script as needed.
![Folder Structure Before](../img/workflow/modify_use_case_before.png)
![Folder Structure After](../img/workflow/modify_use_case_after.png)
