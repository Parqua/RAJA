###############################################################################
# Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
# and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
###############################################################################

#
# List of indexset test types for generating test files.
#
set(INDEXSETTESTTYPES ForallIndexSetView ForallIcountIndexSetView)


#
# Generate tests for each enabled RAJA back-end.
#
# Note: FORALL_BACKENDS is defined in ../CMakeLists.txt
#
foreach( BACKEND ${FORALL_BACKENDS} )
  foreach( INDEXSETTESTTYPE ${INDEXSETTESTTYPES} )
    configure_file( test-forall-indexset-view.cpp.in
                    test-${INDEXSETTESTTYPE}-${BACKEND}.cpp )
    raja_add_test( NAME test-${INDEXSETTESTTYPE}-${BACKEND}
                   SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test-${INDEXSETTESTTYPE}-${BACKEND}.cpp )

    target_include_directories(test-${INDEXSETTESTTYPE}-${BACKEND}.exe
                               PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  endforeach()
endforeach()

unset( INDEXSETTESTTYPES )
