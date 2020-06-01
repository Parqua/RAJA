//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#ifndef __TEST_FORALL_RANGE_REDUCEMAX_MULTIPLE_HPP__
#define __TEST_FORALL_RANGE_REDUCEMAX_MULTIPLE_HPP__

#include "RAJA/RAJA.hpp"

#include "test-forall-range-reduce-multiple.hpp"

#include <cfloat>
#include <climits>
#include <cstdlib>
#include <numeric>
#include <random>

template <typename DATA_TYPE, typename WORKING_RES, 
          typename EXEC_POLICY, typename REDUCE_POLICY>
void ForallRangeReduceMaxMultipleTest(RAJA::Index_type first, 
                                      RAJA::Index_type last)
{
  RAJA::TypedRangeSegment<RAJA::Index_type> r1(first, last);

  RAJA::Index_type index_len = last - first;

  camp::resources::Resource working_res{WORKING_RES()};
  DATA_TYPE* working_array;
  DATA_TYPE* check_array;
  DATA_TYPE* test_array;

  allocateForallTestData<DATA_TYPE>(last,
                                    working_res,
                                    &working_array,
                                    &check_array,
                                    &test_array);

  const DATA_TYPE default_val = static_cast<DATA_TYPE>(-SHRT_MAX);
  const DATA_TYPE big_val = 500;

  for (RAJA::Index_type i = 0; i < last; ++i) {
    test_array[i] = default_val;
  }

  
  static std::random_device rd;
  static std::mt19937 mt(rd());
  static std::uniform_real_distribution<double> dist(-100, 100);
  static std::uniform_real_distribution<double> dist2(0, index_len - 1);

  DATA_TYPE current_max = default_val;

  RAJA::ReduceMax<REDUCE_POLICY, DATA_TYPE> max0;
  max0.reset(default_val);
  RAJA::ReduceMax<REDUCE_POLICY, DATA_TYPE> max1(default_val);
  RAJA::ReduceMax<REDUCE_POLICY, DATA_TYPE> max2(big_val);

  const int nloops = 8;
  for (int j = 0; j < nloops; ++j) {

    DATA_TYPE roll = static_cast<DATA_TYPE>( dist(mt) );
    RAJA::Index_type max_index = static_cast<RAJA::Index_type>(dist2(mt));
    if ( test_array[max_index] < roll ) {
      test_array[max_index] = roll;
      current_max = RAJA_MAX( current_max, roll );
    }

    working_res.memcpy(working_array, test_array, sizeof(DATA_TYPE) * last);

    RAJA::forall<EXEC_POLICY>(r1, [=] RAJA_HOST_DEVICE(RAJA::Index_type idx) {
      max0.max(working_array[idx]);
      max1.max(2 * working_array[idx]);
      max2.max(working_array[idx]);
    });

    ASSERT_EQ(current_max, static_cast<DATA_TYPE>(max0.get()));
    ASSERT_EQ(current_max * 2, static_cast<DATA_TYPE>(max1.get()));
    ASSERT_EQ(big_val, static_cast<DATA_TYPE>(max2.get()));

  }

  max0.reset(default_val); 
  max1.reset(default_val); 
  max2.reset(big_val); 

  const int nloops_b = 4;
  for (int j = 0; j < nloops_b; ++j) {

    DATA_TYPE roll = dist(mt);
    RAJA::Index_type max_index = static_cast<RAJA::Index_type>(dist2(mt));
    if ( test_array[max_index] < roll ) {
      test_array[max_index] = roll;
      current_max = RAJA_MAX(current_max, roll );
    }

    working_res.memcpy(working_array, test_array, sizeof(DATA_TYPE) * last);

    RAJA::forall<EXEC_POLICY>(r1, [=] RAJA_HOST_DEVICE(RAJA::Index_type idx) {
      max0.max(working_array[idx]);
      max1.max(2 * working_array[idx]);
      max2.max(working_array[idx]);    
    });

    ASSERT_EQ(current_max, static_cast<DATA_TYPE>(max0.get()));
    ASSERT_EQ(current_max * 2, static_cast<DATA_TYPE>(max1.get()));
    ASSERT_EQ(big_val, static_cast<DATA_TYPE>(max2.get()));

  }

  deallocateForallTestData<DATA_TYPE>(working_res,
                                      working_array,
                                      check_array,
                                      test_array);
}


TYPED_TEST_P(ForallRangeReduceMultipleTest, ReduceMaxMultipleRangeForall)
{
  using DATA_TYPE     = typename camp::at<TypeParam, camp::num<0>>::type;
  using WORKING_RES   = typename camp::at<TypeParam, camp::num<1>>::type;
  using EXEC_POLICY   = typename camp::at<TypeParam, camp::num<2>>::type;
  using REDUCE_POLICY = typename camp::at<TypeParam, camp::num<3>>::type;

  ForallRangeReduceMaxMultipleTest<DATA_TYPE, WORKING_RES, 
                                   EXEC_POLICY, REDUCE_POLICY>(0, 2115);
}

#endif  // __TEST_FORALL_RANGE_REDUCEMAX_MULTIPLE_HPP__
