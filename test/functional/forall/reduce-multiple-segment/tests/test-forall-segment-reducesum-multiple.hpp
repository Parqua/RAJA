//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#ifndef __TEST_FORALL_SEGMENT_REDUCESUM_MULTIPLE_HPP__
#define __TEST_FORALL_SEGMENT_REDUCESUM_MULTIPLE_HPP__

#include "RAJA/RAJA.hpp"

#include "test-forall-segment-reduce-multiple.hpp"

#include <cstdlib>
#include <numeric>

template <typename DATA_TYPE, typename WORKING_RES, 
          typename EXEC_POLICY, typename REDUCE_POLICY>
void ForallSegmentReduceSumMultipleTest(RAJA::Index_type first, 
                                        RAJA::Index_type last)
{
  RAJA::TypedRangeSegment<RAJA::Index_type> r1(first, last);

  camp::resources::Resource working_res{WORKING_RES()};
  DATA_TYPE* working_array;
  DATA_TYPE* check_array;
  DATA_TYPE* test_array;

  allocateForallTestData<DATA_TYPE>(last,
                                    working_res,
                                    &working_array,
                                    &check_array,
                                    &test_array);

  const DATA_TYPE initval = 2;

  for (RAJA::Index_type i = 0; i < last; ++i) {
    test_array[i] = initval;
  }

  working_res.memcpy(working_array, test_array, sizeof(DATA_TYPE) * last);


  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum0(0);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum1(initval * 1);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum2(0);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum3(initval * 3);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum4(0);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum5(initval * 5);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum6(0);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum7(initval * 7);

  const DATA_TYPE index_len = static_cast<DATA_TYPE>(last - first);

  const int nloops = 2;
  for (int j = 0; j < nloops; ++j) {

    RAJA::forall<EXEC_POLICY>(r1, [=] RAJA_HOST_DEVICE(RAJA::Index_type idx) {
      sum0 += working_array[idx];
      sum1 += working_array[idx] * 2;
      sum2 += working_array[idx] * 3;
      sum3 += working_array[idx] * 4;
      sum4 += working_array[idx] * 5;
      sum5 += working_array[idx] * 6;
      sum6 += working_array[idx] * 7;
      sum7 += working_array[idx] * 8;
    });

    DATA_TYPE check_val = initval * index_len * (j + 1);

    ASSERT_EQ(1 * check_val, static_cast<DATA_TYPE>(sum0.get()));
    ASSERT_EQ(2 * check_val + (initval*1), static_cast<DATA_TYPE>(sum1.get()));
    ASSERT_EQ(3 * check_val, static_cast<DATA_TYPE>(sum2.get()));
    ASSERT_EQ(4 * check_val + (initval*3), static_cast<DATA_TYPE>(sum3.get()));
    ASSERT_EQ(5 * check_val, static_cast<DATA_TYPE>(sum4.get()));
    ASSERT_EQ(6 * check_val + (initval*5), static_cast<DATA_TYPE>(sum5.get()));
    ASSERT_EQ(7 * check_val, static_cast<DATA_TYPE>(sum6.get()));
    ASSERT_EQ(8 * check_val + (initval*7), static_cast<DATA_TYPE>(sum7.get()));

  }


  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum0_b(initval);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum1_b;
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum2_b(initval);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum3_b;
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum4_b(initval);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum5_b;
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum6_b(initval);
  RAJA::ReduceSum<REDUCE_POLICY, DATA_TYPE> sum7_b;

  sum0_b.reset(0);
  sum1_b.reset(initval * 1);
  sum2_b.reset(0);
  sum3_b.reset(initval * 3);
  sum4_b.reset(0.0);
  sum5_b.reset(initval * 5);
  sum6_b.reset(0.0);
  sum7_b.reset(initval * 7);

  const int nloops_b = 3;
  for (int j = 0; j < nloops_b; ++j) {

    RAJA::forall<EXEC_POLICY>(r1, [=] RAJA_HOST_DEVICE(RAJA::Index_type idx) {
      sum0_b += working_array[idx];
      sum1_b += working_array[idx] * 2;
      sum2_b += working_array[idx] * 3;
      sum3_b += working_array[idx] * 4;
      sum4_b += working_array[idx] * 5;
      sum5_b += working_array[idx] * 6;
      sum6_b += working_array[idx] * 7;
      sum7_b += working_array[idx] * 8;
    });

    DATA_TYPE check_val = initval * index_len * (j + 1);

    ASSERT_EQ(1 * check_val, static_cast<DATA_TYPE>(sum0_b.get()));
    ASSERT_EQ(2 * check_val + (initval*1), static_cast<DATA_TYPE>(sum1_b.get()));
    ASSERT_EQ(3 * check_val, static_cast<DATA_TYPE>(sum2_b.get()));
    ASSERT_EQ(4 * check_val + (initval*3), static_cast<DATA_TYPE>(sum3_b.get()));
    ASSERT_EQ(5 * check_val, static_cast<DATA_TYPE>(sum4_b.get()));
    ASSERT_EQ(6 * check_val + (initval*5), static_cast<DATA_TYPE>(sum5_b.get()));
    ASSERT_EQ(7 * check_val, static_cast<DATA_TYPE>(sum6_b.get()));
    ASSERT_EQ(8 * check_val + (initval*7), static_cast<DATA_TYPE>(sum7_b.get()));

  }

  deallocateForallTestData<DATA_TYPE>(working_res,
                                      working_array,
                                      check_array,
                                      test_array);
}


TYPED_TEST_P(ForallSegmentReduceMultipleTest, ReduceSumMultipleSegmentForall)
{
  using DATA_TYPE     = typename camp::at<TypeParam, camp::num<0>>::type;
  using WORKING_RES   = typename camp::at<TypeParam, camp::num<1>>::type;
  using EXEC_POLICY   = typename camp::at<TypeParam, camp::num<2>>::type;
  using REDUCE_POLICY = typename camp::at<TypeParam, camp::num<3>>::type;

  ForallSegmentReduceSumMultipleTest<DATA_TYPE, WORKING_RES, 
                                     EXEC_POLICY, REDUCE_POLICY>(0, 3072);
}

#endif  // __TEST_FORALL_SEGMENT_REDUCESUM_MULTIPLE_HPP__
