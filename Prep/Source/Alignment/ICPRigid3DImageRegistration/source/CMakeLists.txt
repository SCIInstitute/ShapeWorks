 
PROJECT(ICPRigid3DImageRegistration)
cmake_minimum_required(VERSION 2.8)
ADD_EXECUTABLE(ICPRigid3DImageRegistration ICPRigid3DImageRegistration.cxx)
TARGET_LINK_LIBRARIES(ICPRigid3DImageRegistration ${VTK_LIBRARIES} ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS ICPRigid3DImageRegistration  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(MatrixRigid3DImageRegistration MatrixRigid3DImageRegistration.cxx)
TARGET_LINK_LIBRARIES(MatrixRigid3DImageRegistration ${VTK_LIBRARIES} ${ITK_LIBRARIES} OptionParser)
INSTALL(TARGETS MatrixRigid3DImageRegistration  RUNTIME DESTINATION bin)
