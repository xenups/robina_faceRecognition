cmake_minimum_required(VERSION 2.8.3)
project(robina_faceRecognition)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  actionlib
  actionlib_msgs
  pantilt_control
  sensor_msgs
  geometry_msgs
  std_msgs
  cv_bridge
  image_transport
  message_generation
)

find_package(OpenCV)

#find_package(OpenMP REQUIRED)
#add_definitions(${OpenMP_CXX_FLAGS})

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend and a run_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependencies might have been
##     pulled in transitively but can be declared for certainty nonetheless:
##     * add a build_depend tag for "message_generation"
##     * add a run_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
 add_message_files(
  FILES
  Angles_msg.msg
  Face_Angle.msg
  Face_Angles.msg
  faceRecognition_msg.msg
)

## Generate services in the 'srv' folder
add_service_files(
   FILES
   CVloadTest.srv        
   deleteFaces.srv 
   recognizeFaceContiniously.srv
   recognizeFaceOnce.srv
   facesAngles.srv      
   stopContiniouslyfaceDetection.srv
   imageResponse.srv    
   StoreTemplateface.srv
   loadTemplateface.srv
   waitToDetectFace.srv
)

## Generate actions in the 'action' folder
add_action_files(
   FILES
ac_faceRecogn.action       
ac_faceRecognitionContiniously.action
ac_faceRecognition.action
)

 generate_messages(
   DEPENDENCIES  std_msgs sensor_msgs actionlib_msgs
 )

###################################
## catkin specific configuration ##
###################################

catkin_package(
 INCLUDE_DIRS include
#  LIBRARIES HumanDetection
#  CATKIN_DEPENDS hri_msgs opencv2 pcl roscpp
#  DEPENDS system_lib
)



###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
)
include_directories(${OpenCV_INCLUDE_DIRS})

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)




## Declare a cpp library
# add_library(HumanDetection
#   src/${PROJECT_NAME}/HumanDetection.cpp
# )

## Declare a cpp executable

file(GLOB MY_SOURCES src/*.cpp include/robina_faceRecognition/*.h include/robina_faceRecognition/*.hpp)

add_executable(robina_faceRecognition ${MY_SOURCES} )




#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


target_link_libraries(robina_faceRecognition   ${catkin_LIBRARIES})
target_link_libraries(robina_faceRecognition ${OpenCV_LIBRARIES})
target_link_libraries(robina_faceRecognition fsdk)

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_HumanDetection.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
