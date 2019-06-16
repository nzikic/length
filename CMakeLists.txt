cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project (length VERSION 1.0)


#
# length - header-only library
#


add_library (length INTERFACE)
target_include_directories (length INTERFACE include/)


# c++17 has to be enabled
set(CXX_STANDARD_REQUIRED ON)
target_compile_features(lru_cache INTERFACE cxx_std_17)


#installation
install (DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
