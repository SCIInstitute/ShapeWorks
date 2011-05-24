ADD_LIBRARY (Utilities STATIC
string_io.cxx
filenameFactory.cxx
)

IF(USE_VTK)
 ADD_LIBRARY (VtkUtilities STATIC
 string_io.cxx
 glyph_pipeline.cxx
 plane_widget_pipeline.cxx
 sphere_widget_pipeline.cxx
 isosurface_pipeline.cxx
 itkMeshTovtkPolyData.cxx
 )
ENDIF(USE_VTK)

