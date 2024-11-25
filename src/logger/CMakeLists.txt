
add_library(logger "")

target_sources(logger
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/logger.hpp
  ${CMAKE_CURRENT_LIST_DIR}/logger_helper.hpp
  ${CMAKE_CURRENT_LIST_DIR}/file_logger.hpp
  ${CMAKE_CURRENT_LIST_DIR}/term_logger.hpp

  ${CMAKE_CURRENT_LIST_DIR}/logger.cpp
  ${CMAKE_CURRENT_LIST_DIR}/logger_helper.cpp
  ${CMAKE_CURRENT_LIST_DIR}/file_logger.cpp
  ${CMAKE_CURRENT_LIST_DIR}/term_logger.cpp


)

target_include_directories(io
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )
