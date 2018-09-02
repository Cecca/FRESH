find_package(Boost REQUIRED
  COMPONENTS unit_test_framework iostreams filesystem timer)

include_directories("../core")
include_directories("include")

add_executable(frechet frechet.cpp)
target_link_libraries(frechet
  curvedist
  ${Boost_LIBRARIES}
  ${BZIP2_LIBRARIES})

