cmake_minimum_required(VERSION 3.7.0)
project(tetris)

option(TETRIS_WITH_DEVELOPMENT_DEPENDANCIES " need conan package manager" ON)

########### dependencies from cmake #########
if(TETRIS_WITH_DEVELOPMENT_DEPENDANCIES)
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()

########### modern C++ flags######################
set (CMAKE_CXX_STANDARD             17)
set (CMAKE_CXX_STANDARD_REQUIRED    ON)
set (CMAKE_CXX_EXTENSIONS           OFF)



########### Tetris Library ###################

add_library(Tetris 
    src/Tetris/Tetriminos.cpp
    src/Tetris/Tetris.cpp
    src/Tetris/NintendoClassicScore.cpp)
add_library(Tetris::Tetris ALIAS Tetris)
target_include_directories(Tetris PUBLIC src)


########### Tetris Application ###################

add_executable(tetris src/main.cpp)
target_link_libraries(tetris PUBLIC Tetris::Tetris )
target_include_directories(tetris PUBLIC rlutil)


######## test ########
if(TETRIS_WITH_DEVELOPMENT_DEPENDANCIES)
    find_package(Catch2 REQUIRED)

    add_executable(test_tetris  
                    test/test_game_logic.cpp 
                    test/test_tetriminos.cpp 
                    test/test_score.cpp
                    test/main_catch.cpp
                    test/test_user_input.cpp
                    test/test_timer.cpp
                    test/Testables.h)
    target_compile_definitions(test_tetris PUBLIC UNITEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test")
    target_link_libraries(test_tetris Tetris::Tetris  Catch2::Catch2 )


    ######### test coverage with kcov and ctest ######################

    find_program(KCOV kcov)
    message("kcov: ${KCOV}")
    if( KCOV )
        enable_testing()

        add_test(
            NAME tetris_coverage 
            COMMAND ${KCOV} --exclude-pattern=/usr/ --include-path=${CMAKE_CURRENT_LIST_DIR}/src  cov $<TARGET_FILE:test_tetris>  
            )
    endif()
endif()
###################################################################
##################### rlutil   ####################################
add_executable(test_rlutil rlutil/test_rlutil.cpp)
