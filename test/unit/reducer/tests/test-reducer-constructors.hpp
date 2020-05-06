//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

///
/// Header file containing tests for RAJA reducer constructors and initialization.
///

#ifndef __TEST_REDUCER_CONSTRUCTOR__
#define __TEST_REDUCER_CONSTRUCTOR__

#include "gtest/gtest.h"

#include "RAJA/RAJA.hpp"
#include "RAJA_gtest.hpp"
#include "RAJA/internal/MemUtils_CPU.hpp"
#include "camp/resource.hpp"

#if defined(RAJA_ENABLE_CUDA)
#include "RAJA_unit_forone.hpp"
#endif

#include <tuple>

template <typename T>
class ReducerBasicConstructorUnitTest : public ::testing::Test
{
};

template <typename T>
class ReducerInitConstructorUnitTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(ReducerBasicConstructorUnitTest);
TYPED_TEST_SUITE_P(ReducerInitConstructorUnitTest);

template <typename ReducePolicy,
          typename NumericType>
void testReducerConstructor()
{
  RAJA::ReduceSum<ReducePolicy, NumericType> reduce_sum;
  RAJA::ReduceMin<ReducePolicy, NumericType> reduce_min;
  RAJA::ReduceMax<ReducePolicy, NumericType> reduce_max;
  RAJA::ReduceMinLoc<ReducePolicy, NumericType> reduce_minloc;
  RAJA::ReduceMaxLoc<ReducePolicy, NumericType> reduce_maxloc;

  RAJA::ReduceMinLoc<ReducePolicy, NumericType, RAJA::tuple<RAJA::Index_type, RAJA::Index_type>> reduce_minloctup;
  RAJA::ReduceMaxLoc<ReducePolicy, NumericType, RAJA::tuple<RAJA::Index_type, RAJA::Index_type>> reduce_maxloctup;

  ASSERT_EQ((NumericType)reduce_sum.get(), NumericType());
  ASSERT_EQ((NumericType)reduce_min.get(), NumericType());
  ASSERT_EQ((NumericType)reduce_max.get(), NumericType());

  ASSERT_EQ((NumericType)reduce_minloc.get(), NumericType());
  ASSERT_EQ((NumericType)reduce_maxloc.get(), NumericType());
  ASSERT_EQ((RAJA::Index_type)reduce_minloc.getLoc(), RAJA::Index_type());
  ASSERT_EQ((RAJA::Index_type)reduce_maxloc.getLoc(), RAJA::Index_type());

  ASSERT_EQ((NumericType)reduce_minloctup.get(), NumericType());
  ASSERT_EQ((NumericType)reduce_maxloctup.get(), NumericType());
  ASSERT_EQ((RAJA::Index_type)(RAJA::get<0>(reduce_minloctup.getLoc())), RAJA::Index_type());
  ASSERT_EQ((RAJA::Index_type)(RAJA::get<1>(reduce_minloctup.getLoc())), RAJA::Index_type());
  ASSERT_EQ((RAJA::Index_type)(RAJA::get<0>(reduce_maxloctup.getLoc())), RAJA::Index_type());
  ASSERT_EQ((RAJA::Index_type)(RAJA::get<1>(reduce_maxloctup.getLoc())), RAJA::Index_type());
}

TYPED_TEST_P(ReducerBasicConstructorUnitTest, BasicReducerConstructor)
{
  using ReducePolicy = typename camp::at<TypeParam, camp::num<0>>::type;
  using NumericType = typename camp::at<TypeParam, camp::num<1>>::type;

  testReducerConstructor< ReducePolicy, NumericType >();
}

template  < typename ReducePolicy,
            typename NumericType,
            typename ResourceType >
typename  std::enable_if< // Host policy sets value.
            //!std::is_same<ReducePolicy, RAJA::cuda_reduce>::value
            std::is_same<ResourceType, camp::resources::Host>::value
            #if defined(RAJA_ENABLE_TARGET_OPENMP)
            || std::is_same<ReducePolicy, camp::resources::Omp>::value
            #endif
          >::type
exec_dispatcher( NumericType * theVal, NumericType * initVal )
{
  theVal[0] = initVal[0];
}

template  < typename ReducePolicy,
            typename NumericType,
            typename ResourceType >
typename  std::enable_if< // Cuda policy sets value.
            !std::is_same<ResourceType, camp::resources::Host>::value
            #if defined(RAJA_ENABLE_CUDA)
            && std::is_same<ResourceType, camp::resources::Cuda>::value
            #endif
          >::type
exec_dispatcher( NumericType * theVal, NumericType * initVal )
{
  #if defined(RAJA_ENABLE_CUDA)
  forone<<<1,1>>>( [=] __device__ () {
                        theVal[0] = initVal[0];
                 });
  cudaErrchk(cudaDeviceSynchronize());
  #endif
}

template <typename ReducePolicy,
          typename NumericType,
          typename WORKING_RES>
void testInitReducerConstructor()
{
  camp::resources::Resource work_res{WORKING_RES()};
  camp::resources::Resource host_res{camp::resources::Host()};

  NumericType * theVal = nullptr;
  NumericType * initVal = nullptr;

  initVal = work_res.allocate<NumericType>(1);
  theVal = host_res.allocate<NumericType>(1);

  initVal[0] = (NumericType)5;
  theVal[0] = (NumericType)10;

  #if defined(RAJA_ENABLE_CUDA)
  cudaErrchk(cudaDeviceSynchronize());
  #endif

  RAJA::ReduceSum<ReducePolicy, NumericType> reduce_sum(initVal[0]);
  RAJA::ReduceMin<ReducePolicy, NumericType> reduce_min(initVal[0]);
  RAJA::ReduceMax<ReducePolicy, NumericType> reduce_max(initVal[0]);
  RAJA::ReduceMinLoc<ReducePolicy, NumericType> reduce_minloc(initVal[0], 1);
  RAJA::ReduceMaxLoc<ReducePolicy, NumericType> reduce_maxloc(initVal[0], 1);

  RAJA::tuple<RAJA::Index_type, RAJA::Index_type> LocTup(1, 1);
  RAJA::ReduceMinLoc<ReducePolicy, NumericType, RAJA::tuple<RAJA::Index_type, RAJA::Index_type>> reduce_minloctup(initVal[0], LocTup);
  RAJA::ReduceMaxLoc<ReducePolicy, NumericType, RAJA::tuple<RAJA::Index_type, RAJA::Index_type>> reduce_maxloctup(initVal[0], LocTup);

  // move a value onto device and assign
  exec_dispatcher < ReducePolicy,
                    NumericType,
                    WORKING_RES
                  >
                  ( theVal, initVal );

  ASSERT_EQ((NumericType)(theVal[0]), (NumericType)(initVal[0]));

  ASSERT_EQ((NumericType)reduce_sum.get(), (NumericType)(initVal[0]));
  ASSERT_EQ((NumericType)reduce_min.get(), (NumericType)(initVal[0]));
  ASSERT_EQ((NumericType)reduce_max.get(), (NumericType)(initVal[0]));

  ASSERT_EQ((NumericType)reduce_minloc.get(), (NumericType)(initVal[0]));
  ASSERT_EQ((NumericType)reduce_maxloc.get(), (NumericType)(initVal[0]));
  ASSERT_EQ((RAJA::Index_type)reduce_minloc.getLoc(), (RAJA::Index_type)1);
  ASSERT_EQ((RAJA::Index_type)reduce_maxloc.getLoc(), (RAJA::Index_type)1);

  ASSERT_EQ((NumericType)reduce_minloctup.get(), (NumericType)(initVal[0]));
  ASSERT_EQ((NumericType)reduce_maxloctup.get(), (NumericType)(initVal[0]));
  ASSERT_EQ((RAJA::Index_type)(RAJA::get<0>(reduce_minloctup.getLoc())), (RAJA::Index_type)1);
  ASSERT_EQ((RAJA::Index_type)(RAJA::get<1>(reduce_minloctup.getLoc())), (RAJA::Index_type)1);
  ASSERT_EQ((RAJA::Index_type)(RAJA::get<0>(reduce_maxloctup.getLoc())), (RAJA::Index_type)1);
  ASSERT_EQ((RAJA::Index_type)(RAJA::get<1>(reduce_maxloctup.getLoc())), (RAJA::Index_type)1);

  work_res.deallocate( initVal );
  host_res.deallocate( theVal );
}

TYPED_TEST_P(ReducerInitConstructorUnitTest, InitReducerConstructor)
{
  using ReduceType = typename camp::at<TypeParam, camp::num<0>>::type;
  using NumericType = typename camp::at<TypeParam, camp::num<1>>::type;
  using ResourceType = typename camp::at<TypeParam, camp::num<2>>::type;

  testInitReducerConstructor< ReduceType, NumericType, ResourceType >();
}


REGISTER_TYPED_TEST_CASE_P(ReducerBasicConstructorUnitTest,
                           BasicReducerConstructor);

REGISTER_TYPED_TEST_CASE_P(ReducerInitConstructorUnitTest,
                           InitReducerConstructor);
#endif  //__TEST_REDUCER_CONSTRUCTOR__
