
FILE(GLOB_RECURSE demo_em_SRC ./*.cpp)

ADD_EXECUTABLE(demo_em ${demo_em_SRC})

target_compile_options(demo_em PUBLIC -DCARTESIAN_COORDINATE_SYTEM)
TARGET_LINK_LIBRARIES(demo_em m dl simpla_lib simpla_main)

 

