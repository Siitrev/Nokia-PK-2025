project(UeApplication)
cmake_minimum_required(VERSION 3.12)

aux_source_directory(. SRC_LIST)
aux_source_directory(Ports SRC_LIST)
aux_source_directory(States SRC_LIST)
aux_source_directory(SmsRepository SRC_LIST)

add_library(${PROJECT_NAME} ${SRC_LIST}
    SmsRepository/SmsEntity.cpp
    States/TalkingState.h States/TalkingState.cpp

)
target_link_libraries(${PROJECT_NAME} Common)
