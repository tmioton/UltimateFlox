add_library(thread-pool INTERFACE thread-pool/SafeQueue.hpp thread-pool/ThreadPool.hpp)
set_target_properties(
    thread-pool PROPERTIES
    LINKER_LANGUAGE CXX
)
target_include_directories(thread-pool INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")