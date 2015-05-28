# FIM_PATH = path to the source of the fim library

file(GLOB FIM_INCL_PATH_1 "../fim*")
file(GLOB FIM_INCL_PATH_2 "../FIM*")
file(GLOB FIM_INCL_PATH_3 "../../fim*")
file(GLOB FIM_INCL_PATH_4 "../../FIM*")
file(GLOB FIM_INCL_PATH_5 "$ENV{HOME}/fim*")
file(GLOB FIM_INCL_PATH_6 "$ENV{HOME}/FIM*")
set(FIM_INCL_PATHS ${FIM_INCL_PATH_1} ${FIM_INCL_PATH_2} 
                   ${FIM_INCL_PATH_3} ${FIM_INCL_PATH_4} 
                   ${FIM_INCL_PATH_5} ${FIM_INCL_PATH_6})


find_path(FIM_PATH include/meshFIM.h 
          ${FIM_INCL_PATHS}
          /usr/include/fim
          /usr/local/include/fim    
          "$ENV{LIB_DIR}/include" 
          "$ENV{LIB_DIR}/include/fim"
          "$ENV{ProgramFiles}/fim" )       

if (FIM_PATH)
  set(FIM_FOUND TRUE)
  find_path(FIM_INCLUDE meshFIM.h ${FIM_PATH}/include)
  find_library(FIM_LIB fim ${FIM_PATH}/lib)
endif(FIM_PATH)

IF (FIM_FOUND)
    MESSAGE(STATUS "Found FIM: ${FIM_LIBRARY}")
ELSE (FIM_FOUND)
  IF (FIM_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find FIM.")
  ENDIF (FIM_FIND_REQUIRED)
ENDIF (FIM_FOUND)
