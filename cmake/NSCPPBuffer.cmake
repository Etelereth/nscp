# Try to find protocol buffers (protobuf)
#
# Use as FIND_PACKAGE(ProtocolBuffers)
#
#  PROTOBUF_FOUND - system has the protocol buffers library
#  PROTOBUF_INCLUDE_DIR - the zip include directory
#  PROTOBUF_LIBRARY - Link this to use the zip library
#  PROTOBUF_PROTOC_EXECUTABLE - executable protobuf compiler
#
# And the following command
#
#  WRAP_PROTO(VAR input1 input2 input3..)
#
# Which will run protoc on the input files and set VAR to the names of the created .cc files,
# ready to be added to ADD_EXECUTABLE/ADD_LIBRARY. E.g,
#
#  WRAP_PROTO(PROTO_SRC myproto.proto external.proto)
#  ADD_EXECUTABLE(server ${server_SRC} {PROTO_SRC})
#
# Author: Esben Mose Hansen <[EMAIL PROTECTED]>, (C) Ange Optimization ApS 2008
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

IF (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)
  # in cache already
  SET(PROTOBUF_FOUND TRUE)
  MESSAGE(STATUS, "CACHED Protocol buffers was found!")
ELSE (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)

  FIND_PATH(PROTOBUF_INCLUDE_DIR stubs/common.h
    /usr/include/google/protobuf
  )

  FIND_LIBRARY(PROTOBUF_LIBRARY NAMES protobuf libprotobuf
    PATHS
    ${GNUWIN32_DIR}/lib
	${PROTOBUF_LIBRARYDIR}
  )

  FIND_PROGRAM(PROTOBUF_PROTOC_EXECUTABLE protoc)
  IF(NOT PROTOBUF_PROTOC_EXECUTABLE)
	FIND_PROGRAM(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS ${PROTOBUF_BINARYDIR})
  ENDIF(NOT PROTOBUF_PROTOC_EXECUTABLE)

  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(protobuf DEFAULT_MSG PROTOBUF_INCLUDE_DIR PROTOBUF_LIBRARY PROTOBUF_PROTOC_EXECUTABLE)

  # ensure that they are cached
  SET(PROTOBUF_INCLUDE_DIR ${PROTOBUF_INCLUDE_DIR} CACHE INTERNAL "The protocol buffers include path")
  SET(PROTOBUF_LIBRARY ${PROTOBUF_LIBRARY} CACHE INTERNAL "The libraries needed to use protocol buffers library")
  SET(PROTOBUF_PROTOC_EXECUTABLE ${PROTOBUF_PROTOC_EXECUTABLE} CACHE INTERNAL "The protocol buffers compiler")

  MESSAGE(STATUS, "PROTOBUF_INCLUDE_DIR: ${PROTOBUF_INCLUDE_DIR}, PROTOBUF_LIBRARY: ${PROTOBUF_LIBRARY}, PROTOBUF_PROTOC_EXECUTABLE: ${PROTOBUF_PROTOC_EXECUTABLE}")

ENDIF (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)

IF (PROTOBUF_FOUND)
  MESSAGE(STATUS, "Good: Protocol buffers was found (${PROTOBUF_INCLUDE_DIR})")
  # Define the WRAP_PROTO function
  SET(PROTOBUF_FOUND TRUE PARENT_SCOPE)
  FUNCTION(WRAP_PROTO VAR)
    IF (NOT ARGN)
      MESSAGE(SEND_ERROR "Error: WRAP PROTO called without any proto files")
      RETURN()
    ENDIF(NOT ARGN)

    SET(INCL)
    SET(${VAR})
    FOREACH(FIL ${ARGN})
      GET_FILENAME_COMPONENT(ABS_FIL ${FIL} ABSOLUTE)
      GET_FILENAME_COMPONENT(FIL_WE ${FIL} NAME_WE)
      LIST(APPEND ${VAR} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")
      LIST(APPEND INCL "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h")
	  
	  SET(PB_TARGET_INCLUDE ${INCL})
	  configure_file(${ABS_FIL}.h.in ${ABS_FIL}.h)

      ADD_CUSTOM_COMMAND(
        OUTPUT ${${VAR}} ${INCL}
        COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
        ARGS --cpp_out  ${CMAKE_CURRENT_BINARY_DIR} --proto_path ${CMAKE_CURRENT_SOURCE_DIR} ${ABS_FIL}
        DEPENDS ${ABS_FIL}
        COMMENT "Running protocol buffer compiler on ${FIL}" VERBATIM )

      SET_SOURCE_FILES_PROPERTIES(${${VAR}} ${INCL} PROPERTIES GENERATED TRUE)
    ENDFOREACH(FIL)

    SET(${VAR} ${${VAR}} PARENT_SCOPE)

  ENDFUNCTION(WRAP_PROTO)
ELSE(PROTOBUF_FOUND)
  MESSAGE(STATUS, "Error: Protocol buffers was not found!")
ENDIF(PROTOBUF_FOUND)