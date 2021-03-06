#==============================================================================
# GMDS Project
#==============================================================================
cmake_minimum_required(VERSION 3.10.0)
#==============================================================================
# Operating System specificities (must be put at first)
#==============================================================================
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
endif()
#==============================================================================
project(gmds VERSION 1.0 LANGUAGES CXX)
#==============================================================================
# Compilation options mainly for testing purpose
#==============================================================================
option(WITH_DOC "Build documentation"                                                   OFF)
option(WITH_TEST "Build test"                                                           OFF)
option(WITH_CODE_COVERAGE "Perform code coverage with avail. test suites"               OFF)
option(WITH_CODE_COVERAGE_HTML_REPORT "Activate html reporting during code covering"    OFF)
#==============================================================================
if (WITH_CODE_COVERAGE_HTML_REPORT)
    set(WITH_CODE_COVERAGE ON)
endif()
if (WITH_CODE_COVERAGE)
    set(WITH_TEST ON)
endif()
#==============================================================================
# Location of cmake modules
#==============================================================================
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
#==============================================================================
#Tester avec External project, qui build en configure et projet à part (pas de redeploiement)
# Fait un projet dont on depend et pas inclus
if(WITH_TEST)
    enable_testing()
    add_subdirectory(external/googletest)
    include(GoogleTest)
    set(TEST_SAMPLES_DIR "${CMAKE_SOURCE_DIR}/test_samples")

    configure_file( 
            unit_test_config.h.in
            unit_test_config.h
            @ONLY)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif(WITH_TEST)
#==============================================================================
# Commmand to add a new component in GMDS
#==============================================================================
#     COMP_NAME  component name
#     DIR_NAME   the subdirectory where the component code is located
#     LIB_NAME   the final lib name as generated at the end
#     MESSAGE    the message associated to the flag for activating this component
#     ACTIVATE   ON or OFF to indicate that the component must be used or not
#     COVER      ON or OFF, ON meaning that the component must be covered
macro(GMDS_ADD_COMPONENT COMP_NAME DIR_NAME LIB_NAME  MESSAGE ACTIVATE COVER)
    option(ENABLE_${COMP_NAME} ${MESSAGE} ${ACTIVATE} )
    list(APPEND GMDS_ALL_COMPONENTS  ${COMP_NAME})
    if(${COVER})
            list(APPEND GMDS_COVER_COMPONENTS ${COMP_NAME})
    endif()
    if (ENABLE_${COMP_NAME})
        list(APPEND GMDS_ACTIVE_COMPONENTS  ${COMP_NAME})
        list(APPEND GMDS_ACTIVE_LIBS  LIB_GMDS_${COMP_NAME})
        list(APPEND GMDS_ACTIVE_DIRS ${DIR_NAME})
        message(STATUS "Component ${LIB_NAME} is activated")
    else()
        set(LIB_GMDS_${COMP_NAME} )
        message(STATUS "Component ${LIB_NAME} is not activated")
    endif ()

endmacro()

#==============================================================================
# OPTIONAL COMPONENTS
#==============================================================================
GMDS_ADD_COMPONENT(
        FRAME                               # cmake variable
        frame                               # src subdirectory name
        GMDSFrame                           # name of the generated library
        "Frame field generation component"  # description
        ON                                  # is activated
        ON                                 # must be covered
)

GMDS_ADD_COMPONENT(
        FRAME_3D
        frame3d
        GMDSFrame3d
        "3D Frame field generation component"
        ON
        ON                                 # must be covered
)

GMDS_ADD_COMPONENT(
        SINGGRAPHBUILD
        singGraphBuild
        GMDSSingGraphBuild
        "Singularity graph building component"
        ON
        ON                                 # must be covered
)

GMDS_ADD_COMPONENT(
        DUAL_BLOCKING
        dualBlocking
        GMDSDualBlocking
        "Dual blocking component"
        OFF
        OFF                                 # must be covered
)

GMDS_ADD_COMPONENT(
        MEDUSA
        medusa
        GMDSMedusa
        "Visualization component"
        OFF
        OFF                                 # must be covered
)

GMDS_ADD_COMPONENT(
        KMDS
        kmds
        GMDSkmds
        "kokkos-based mesh structure"
        OFF
        OFF                                 # must be covered
)

GMDS_ADD_COMPONENT(
        ELG3D
        Elg3D
        GMDSelg3d
        "overlay-grid algorithm"
        OFF
        OFF                                 # must be covered
)

GMDS_ADD_COMPONENT(
        HYBRIDMESHADAPT
        hybridMeshAdapt
        GMDSHybridMeshAdapt
        "Hybrid remeshing component"
        ON
        ON                                 # must be covered
)

GMDS_ADD_COMPONENT(
        PADDING
        padding
        GMDSPadding
        "Smart Padding/Pillowing algorithm"
        ON
        ON                                # must be covered
)

GMDS_ADD_COMPONENT(
        POLYBLOCK
        polyblock
        GMDSPolyblock
        "Polycube_like algorithms for hexahedral blocking"
        ON
        ON                                # must be covered
)
#==============================================================================
#mettre les find package uniquement ou necessaire
# faire pareil en dessous avec External project
find_package(Eigen3)
if(NOT EIGEN3_FOUND)
    include_directories(${CMAKE_SOURCE_DIR}/external/Eigen)
    message(STATUS "Eigen3 not found, we use a local copy located in the external directory")
else()
    message(STATUS "Eigen3 found in ${EIGEN3_INCLUDE_DIR}")
    # should be replaced by target_link_libraries (example Eigen3::Eigen)
    include_directories(${EIGEN3_INCLUDE_DIR})
endif(NOT EIGEN3_FOUND)

set (GMDS_INCLUDE_DIRS APPEND)


#==============================================================================
# TESTING AND CODE COVERAGE
#
# Up to now, some components are not reliable enough to be added in the full
# code coverage process. It must be improved in the next few months
#==============================================================================
if (WITH_CODE_COVERAGE)
    message(STATUS "============================================")
    message(STATUS "============= COVERAGE MODE  ===============")
    message(STATUS "============================================")
    message(STATUS "CXX Compiler          = ${CMAKE_CXX_COMPILER}")
    message(STATUS "CXX Compiler Version  = ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "CMAKE_CXX_FLAGS       = ${CMAKE_CXX_FLAGS}")
    message(STATUS "CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG}")
    message(STATUS "============================================")
    message(STATUS "============ COVERED COMPONENTS ============")
    message(STATUS "============================================")
    foreach(c IN LISTS GMDS_COVER_COMPONENTS)
        set(ENABLE_${c} ON)
        message(STATUS "COMPONENT  : ${c}")
    endforeach()
    message(STATUS "============================================")
endif(WITH_CODE_COVERAGE)
#==============================================================================
#voir a ajouter une target coverage avec ces options liées à la target
# va m'éviter les variables locales
# target vide si pas besoin ou pas
# forcer mode non optimise plutôt
# Attention ne pas livrer avec code coverage!!
# Faire un build type Coverage, comme Debug ou Release
if (WITH_CODE_COVERAGE)
    include(CodeCoverage)
    set(CMAKE_CXX_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
    set(COVERAGE_LCOV_EXCLUDES
            '/usr/*'
            '${CMAKE_SOURCE_DIR}/doc/*'
            '${CMAKE_SOURCE_DIR}/test_samples/*'
            '${CMAKE_SOURCE_DIR}/external/*'
            '${CMAKE_SOURCE_DIR}/*/tst/*'
            )

    #==============================================================================
    # Data directory if required for some tests
    set(DATA_DIR ${CMAKE_BINARY_DIR}/test_samples)
    # Directory that contains test suites

    if(WITH_CODE_COVERAGE_HTML_REPORT)
        SETUP_TARGET_FOR_COVERAGE_LCOV(
                NAME code_cover_gmds
                EXECUTABLE ctest
                ${DATA_DIR} .
        )
    else(WITH_CODE_COVERAGE_HTML_REPORT)
        SETUP_TARGET_FOR_COVERAGE_LCOV_SHORT(
                NAME code_cover_gmds
                EXECUTABLE ctest
                ${DATA_DIR}
        )
    endif(WITH_CODE_COVERAGE_HTML_REPORT)
    #==============================================================================
    #voir configure_file qui garde le lien de mise à jour, dépendances conservées
    file(COPY
            ${CMAKE_SOURCE_DIR}/test_samples/
            DESTINATION ${CMAKE_BINARY_DIR}/test_samples/)

endif(WITH_CODE_COVERAGE)

#==============================================================================
include(ExternalProject)
include(GNUInstallDirs)

add_subdirectory(external/ANN)
add_subdirectory(external/Predicates_psm)
add_subdirectory(external/inih)
add_subdirectory(external/OpenNL)
add_subdirectory(external/gco-v3)



################################################################
# External: GLPK
################################################################
# Issue to compile on MacOS so give the path to lib and inc
set(GLPK_LIB "NOT_DEFINED" CACHE PATH "Glpk lib location")
set(GLPK_INC "NOT_DEFINED" CACHE PATH "Glpk headers location")
if( (NOT IS_ABSOLUTE ${GLPK_LIB}) OR
(NOT IS_ABSOLUTE ${GLPK_INC}))
    MESSAGE(STATUS "GLPK NOT DEFINED > require to build it as an external project")
    ExternalProject_Add(glpk
            PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/glpk
            URL ${CMAKE_SOURCE_DIR}/external/glpk-4.65.tar.gz
            PATCH_COMMAND autoreconf -if
            CONFIGURE_COMMAND /${CMAKE_CURRENT_BINARY_DIR}/external/glpk/src/glpk/configure --enable-silent-rules
            CC=${CMAKE_C_COMPILER}
            CXX=${CMAKE_CXX_COMPILER}
            "CXXFLAGS=${EXTERNAL_CXX_FLAGS}"
            "LDFLAGS=${EXTERNAL_LD_FLAGS}"
            --prefix=${CMAKE_CURRENT_BINARY_DIR}/glpk
            ${SHARED_LIB_CONFIG_COMMAND}
            BUILD_COMMAND make V=0
            INSTALL_COMMAND make install)
    set(GLPK_LIB ${CMAKE_BINARY_DIR}/glpk/lib)
    set(GLPK_INC ${CMAKE_BINARY_DIR}/glpk/include)
endif()


#==============================================================================
# PROJECT STRUCTURE
#==============================================================================
#for each component, we define a library name to use and a directory
set(LIB_GMDS_UTILS GMDSUtils)
add_subdirectory(utils)
set(LIB_GMDS_MATH GMDSMath)
add_subdirectory(math)
set(LIB_GMDS_IG GMDSIg)
add_subdirectory(ig)
set(LIB_GMDS_IO GMDSIo)
add_subdirectory(io)
set(LIB_GMDS_IG_ALGO GMDSIgAlgo)
add_subdirectory(igalgo)
set(LIB_GMDS_CAD GMDSCad)
add_subdirectory(cad)
set(LIB_GMDS_IG_SHEET GMDSSheet)
add_subdirectory(sheet)
set(LIB_GMDS_SERVICES GMDSServices)
add_subdirectory(services)
#==============================================================================
list(LENGTH GMDS_ACTIVE_COMPONENTS count)
math(EXPR count "${count}-1")
foreach(i RANGE ${count})
    list(GET GMDS_ACTIVE_COMPONENTS ${i} name)
    list(GET GMDS_ACTIVE_LIBS ${i} lib)
    list(GET GMDS_ACTIVE_DIRS ${i} dir)
    set(LIB_GMDS_${name} ${lib} )
    add_subdirectory(${dir})
endforeach()
#==============================================================================
# DOCUMENTATION
#==============================================================================
if (WITH_DOC)
    add_subdirectory(doc)
endif (WITH_DOC)
#==============================================================================
# Mise en place pour l'install avec les export
#==============================================================================
install(EXPORT GMDS_SUITE
        DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/gmds-${PROJECT_VERSION}
        )

set(INCLUDE_INSTALL_DIR include/)
set(LIB_INSTALL_DIR lib/)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        gmdsConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/gmdsConfig.cmake
        INSTALL_DESTINATION ${LIB_INSTALL_DIR}/gmds/cmake
        PATH_VARS INCLUDE_INSTALL_DIR
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/gmdsConfigVersion.cmake
        VERSION 0.7.2
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/gmdsConfig.cmake
#       ${CMAKE_CURRENT_BINARY_DIR}/gmdsConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/gmds/cmake)
