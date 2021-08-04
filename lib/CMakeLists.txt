# 设置变量
SET(LIBHELLO_SRC ./jni.h ./dllmain.cpp ./loader.h ./pch.h ./framework.h ./classes.h ./pch.cpp) 
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib_out) # 第一个参数为你需要构建的dll的名字，第二个为类型 
ADD_LIBRARY(eloader_dll SHARED ${LIBHELLO_SRC})
INSTALL(TARGETS eloader_dll) # 为dll设置linker 
# 指定dll的生成目录，这里是：./lib_out 
SET_TARGET_PROPERTIES(eloader_dll PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} OUTPUT_NAME "eloader_dll" PREFIX "")