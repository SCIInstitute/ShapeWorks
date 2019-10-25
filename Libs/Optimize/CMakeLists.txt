add_executable(ShapeWorksRun ShapeWorksRun.cxx ShapeWorksRunApp.h ShapeWorksRunApp.txx)
set_target_properties(ShapeWorksRun PROPERTIES COMPILE_DEFINITIONS SW_USE_FEAMESH)
target_link_libraries(ShapeWorksRun ITKParticleSystem ${ITK_LIBRARIES} ${VTK_LIBRARIES} tinyxml fim_v4 vgl vgl_algo Utils)
install(TARGETS ShapeWorksRun RUNTIME DESTINATION bin)
