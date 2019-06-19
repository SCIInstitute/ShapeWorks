 
PROJECT(ICPRigid3DImageRegistration)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ICPRigid3DImageRegistration ICPRigid3DImageRegistration.cxx ${source_files})
TARGET_LINK_LIBRARIES(ICPRigid3DImageRegistration ${VTK_LIBRARIES} ${ITK_LIBRARIES})
INSTALL(TARGETS ICPRigid3DImageRegistration  RUNTIME DESTINATION .)
