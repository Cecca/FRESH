include_directories(../core)

add_executable(allpairs all_pairs.cpp ${CURVEDIST_LIB_HEADER_FILES})
target_link_libraries(allpairs curvedist
  ${Boost_LIBRARIES}
  ${BZIP2_LIBRARIES})

add_executable(ffrq-data data_manager.cpp ${CURVEDIST_LIB_HEADER_FILES})
target_link_libraries(ffrq-data curvedist
  ${Boost_LIBRARIES}
  ${BZIP2_LIBRARIES})
