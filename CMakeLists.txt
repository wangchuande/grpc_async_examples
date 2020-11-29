cmake_minimum_required(VERSION 3.16)
project(grpc_async_examples)

set(CMAKE_CXX_STANDARD 14)

find_package(Protobuf CONFIG REQUIRED)
message(STATUS "protobuf version: ${Protobuf_VERSION}")
find_program(_PROGRAM_PROTOC protoc)

find_package(gRPC CONFIG REQUIRED)
message(STATUS "grpc version: ${gRPC_VERSION}")
find_program(_PROGRAM_GRPC_CPP_PLUGIN grpc_cpp_plugin)

find_package(Threads REQUIRED)

include_directories(.)


execute_process(
        COMMAND ${_PROGRAM_PROTOC} -I=./ --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=${_PROGRAM_GRPC_CPP_PLUGIN} ./hellostreamingworld.proto
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)


file(GLOB LIB_GENERATED_SOURCES ${PROJECT_SOURCE_DIR}/*.pb.cc)

add_library(hellostreamingworld SHARED ${LIB_GENERATED_SOURCES})
target_link_libraries(hellostreamingworld gRPC::grpc++_unsecure protobuf::libprotobuf)

add_executable(greeter_streaming_client
        greeter_streaming_client.cc
        )
target_link_libraries(greeter_streaming_client
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)

add_executable(async_streaming_server
        async_streaming_server.cc
        )
target_link_libraries(async_streaming_server
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)

add_executable(async_streaming_server_alarm
        async_streaming_server_alarm.cc
        )
target_link_libraries(async_streaming_server_alarm
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)

add_executable(async_streaming_server_queue_to_back
        async_streaming_server_queue_to_back.cc
        )
target_link_libraries(async_streaming_server_queue_to_back
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)

add_executable(async_streaming_server_queue_to_front
        async_streaming_server_queue_to_front.cc
        )
target_link_libraries(async_streaming_server_queue_to_front
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)

add_executable(async_streaming_server_with_large_message
        async_streaming_server_with_large_message
        )
target_link_libraries(async_streaming_server_with_large_message
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)

add_executable(async_streaming_server_with_separate_thread_to_write
        async_streaming_server_with_separate_thread_to_write.cc
        )
target_link_libraries(async_streaming_server_with_separate_thread_to_write
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)

add_executable(async_streaming_server_with_separate_queue
        async_streaming_server_with_separate_queue.cc
        )
target_link_libraries(async_streaming_server_with_separate_queue
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)

add_executable(async_streaming_server_with_separate_queue_and_thread
        async_streaming_server_with_separate_queue_and_thread.cc
        )
target_link_libraries(async_streaming_server_with_separate_queue_and_thread
        hellostreamingworld
        gRPC::grpc++_unsecure
        protobuf::libprotobuf)
