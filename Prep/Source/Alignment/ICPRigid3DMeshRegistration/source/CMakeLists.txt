 
PROJECT(ICPRigid3DMeshRegistration)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ICPRigid3DMeshRegistration ICPRigid3DMeshRegistration.cxx)
TARGET_LINK_LIBRARIES(ICPRigid3DMeshRegistration ${VTK_LIBRARIES} tinyxml)
INSTALL(TARGETS ICPRigid3DMeshRegistration  RUNTIME DESTINATION bin)
