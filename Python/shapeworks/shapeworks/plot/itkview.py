# importing itkwidgets to visualize single segmentations
import itkwidgets as itkw

# itkwidgets.view returns a Viewer object. And, the IPython Jupyter kernel 
# displays the last return value of a cell by default. So we have to use the display function
# to be able to call itkwidgets within a function and if statements
from IPython.display import display


def itkfoo():
    print("itkfoo")
    
