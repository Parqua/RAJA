/*
 * Copyright (c) 2016, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and
 * further review from Lawrence Livermore National Laboratory.
 */

/*!
 ******************************************************************************
 *
 * \file
 *
 * \brief   Main RAJA header file.
 *     
 *          This is the main header file to include in code that uses RAJA.
 *          It includes other RAJA headers files that define types, index
 *          sets, ieration methods, etc.
 *
 *          IMPORTANT: If changes are made to this file, note that contents
 *                     of some header files require that they are included
 *                     in the order found here.
 *
 ******************************************************************************
 */

#ifndef RAJA_HXX
#define RAJA_HXX

#include "config.hxx"

#include "int_datatypes.hxx"
#include "real_datatypes.hxx"

#include "reducers.hxx"

#include "RangeSegment.hxx"
#include "ListSegment.hxx"
#include "IndexSet.hxx"


//
//////////////////////////////////////////////////////////////////////
//
// These contents of the header files included here define index set 
// and segment execution methods whose implementations depend on 
// programming model choice.
//
// The ordering of these file inclusions must be preserved since there
// are dependencies among them.
//
//////////////////////////////////////////////////////////////////////
//

//
// All platforms should support simd execution.  
//
#include "exec-simd/raja_simd.hxx"

//
// All platforms must support sequential execution.  
//
#include "exec-sequential/raja_sequential.hxx"


#if defined(RAJA_USE_CUDA)
#include "exec-cuda/raja_cuda.hxx"
#endif

#if defined(RAJA_USE_OPENMP)
#include "exec-openmp/raja_openmp.hxx"
#endif

#if defined(RAJA_USE_CILK)
#include "exec-cilk/raja_cilk.hxx"
#endif

//
// Macros for decorating host/device functions
//
#if defined(RAJA_USE_CUDA)

#define RAJA_HOST_DEVICE __host__ __device__
#define RAJA_DEVICE __device__
#else

#define RAJA_HOST_DEVICE 
#define RAJA_DEVICE 
#endif


//
// Generic iteration templates require specializations defined 
// in the files included above.
//
#include "forall_generic.hxx"


#include "IndexSetUtils.hxx"

#endif  // closing endif for header file include guard