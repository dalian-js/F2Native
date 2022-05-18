cmake_minimum_required(VERSION 3.4.1)

set(PROJECT_NAME "core_unit_test")

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.9")

# SET(GCC_COVERAGE_LINK_FLAGS  "-coverage -lgcov")

# link_directories("../third_party/googletest/build/lib")

include_directories(../core)
include_directories(../third_party)
include_directories(../third_party/json/single_include)

# set(GTEST ../third_party/googletest)
# include_directories("${GTEST}/googletest/include/")
# include_directories("${GTEST}/googlemock/include/")

# 添加 core 下所有源文件
aux_source_directory(../core/graphics DIR_SRC_GRAPHICS)
aux_source_directory(../core/utils DIR_SRC_UTILS)

aux_source_directory(../core/graphics/axis DIR_SRC_GRAPHICS_AXIS)
aux_source_directory(../core/graphics/canvas DIR_SRC_GRAPHICS_CANVAS)
aux_source_directory(../core/graphics/geom DIR_SRC_GRAPHICS_GEOM)
aux_source_directory(../core/graphics/geom/attr DIR_SRC_GRAPHICS_GEOM_ATTR)
aux_source_directory(../core/graphics/geom/shape DIR_SRC_GRAPHICS_GEOM_SHAPE)
aux_source_directory(../core/graphics/guide DIR_SRC_GRAPHICS_GUIDE)
aux_source_directory(../core/graphics/scale DIR_SRC_GRAPHICS_SCALE)
aux_source_directory(../core/graphics/scale/continuous DIR_SRC_GRAPHICS_SCALE_CONTINUOUS)
aux_source_directory(../core/graphics/shape DIR_SRC_GRAPHICS_SHAPE)
aux_source_directory(../core/graphics/util DIR_SRC_GRAPHICS_UTIL)
aux_source_directory(../core/graphics/adjust DIR_SRC_GRAPHICS_ADJUST)
aux_source_directory(../core/graphics/event DIR_SRC_GRAPHICS_EVENT)
aux_source_directory(../core/graphics/tooltip DIR_SRC_GRAPHICS_TOOLTIP)
aux_source_directory(../core/graphics/interaction DIR_SRC_GRAPHICS_INTERACTION)
aux_source_directory(../core/graphics/legend DIR_SRC_GRAPHICS_LEGEND)
aux_source_directory(../core/graphics/func DIR_SRC_GRAPHICS_FUNC)
aux_source_directory(../core/graphics/animate DIR_SRC_GRAPHICS_ANIMATE)

list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_AXIS})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_CANVAS})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_GEOM})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_GEOM_ATTR})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_GEOM_SHAPE})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_GUIDE})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_SCALE})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_SCALE_CONTINUOUS})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_SHAPE})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_UTIL})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_ADJUST})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_EVENT})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_TOOLTIP})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_INTERACTION})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_LEGEND})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_FUNC})
list(APPEND DIR_SRC_GRAPHICS ${DIR_SRC_GRAPHICS_ANIMATE})


list(APPEND DIR_SRC ${DIR_SRC_UTILS})
list(APPEND DIR_SRC ${DIR_SRC_GRAPHICS})
################

aux_source_directory(../third_party/WilkinsonExtended WilkinsonExtended)
list(REMOVE_ITEM WilkinsonExtended "../third_party/WilkinsonExtended/main.cpp")

include_directories(./src)
aux_source_directory(./src DIR_TESTS)

add_executable(${PROJECT_NAME} ${DIR_SRC} ${DIR_TESTS} ${WilkinsonExtended}
    ./main.cpp
)