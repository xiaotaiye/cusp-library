/*
 *  Copyright 2008-2009 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*! \file csr_matrix.h
 *  \brief Compressed Sparse Row matrix format.
 */

#pragma once

#include <cusp/detail/config.h>

#include <cusp/array1d.h>
#include <cusp/detail/matrix_base.h>

namespace cusp
{

/*! \addtogroup container_classes Container Classes 
 *  \addtogroup sparse_matrix_formats Sparse Matrices
 *  \ingroup container_classes
 *  \{
 */
    template<typename IndexType, class SpaceOrAlloc>
    class csr_pattern : public detail::matrix_base<IndexType>
    {
        public:
        typedef typename cusp::choose_memory_allocator<IndexType, SpaceOrAlloc>::type index_allocator_type;
        typedef typename cusp::allocator_space<index_allocator_type>::type memory_space;
        typedef typename cusp::csr_pattern<IndexType, SpaceOrAlloc> pattern_type;
        
        typedef IndexType index_type;
       
        template<typename SpaceOrAlloc2>
        struct rebind { typedef csr_pattern<IndexType, SpaceOrAlloc2> type; };

        cusp::array1d<IndexType, index_allocator_type> row_offsets;
        cusp::array1d<IndexType, index_allocator_type> column_indices;
    
        csr_pattern();
    
        csr_pattern(IndexType num_rows, IndexType num_cols, IndexType num_entries);
        
        template <typename IndexType2, typename SpaceOrAlloc2>
        csr_pattern(const csr_pattern<IndexType2,SpaceOrAlloc2>& pattern);

        void resize(IndexType num_rows, IndexType num_cols, IndexType num_entries);
        
        void swap(csr_pattern& pattern);
    }; // class csr_pattern


/*! \p csr_matrix : Compressed Sparse Row matrix format
 *
 * \tparam IndexType Type used for matrix indices (e.g. \c int).
 * \tparam ValueType Type used for matrix values (e.g. \c float).
 * \tparam SpaceOrAlloc Either a memory space such as \c cusp::host_memory or 
 *         \c cusp::device_memory or a specific memory allocator type such as
 *         \c thrust::device_malloc_allocator<T>.
 *
 * \note The matrix entries within the same row must be sorted by column index.
 * \note The matrix should not contain duplicate entries.
 *
 *  The following code snippet demonstrates how to create a 4-by-3
 *  \p csr_matrix on the host with 6 nonzeros and then copies the
 *  matrix to the device.
 *
 *  \code
 *  #include <cusp/csr_matrix.h>
 *  ...
 *
 *  // allocate storage for (4,3) matrix with 4 nonzeros
 *  cusp::csr_matrix<int,float,cusp::host_memory> A(4,3,6);
 *
 *  // initialize matrix entries on host
 *  A.row_offsets[0] = 0;  // first offset is always zero
 *  A.row_offsets[1] = 2;  
 *  A.row_offsets[2] = 2;  
 *  A.row_offsets[3] = 3;
 *  A.row_offsets[4] = 6; // last offset is always num_entries
 *  A.columns_indices[0] = 0; A.values[0] = 10; 
 *  A.columns_indices[1] = 2; A.values[1] = 20;
 *  A.columns_indices[2] = 2; A.values[2] = 30;
 *  A.columns_indices[3] = 0; A.values[3] = 40;
 *  A.columns_indices[4] = 1; A.values[4] = 50;
 *  A.columns_indices[5] = 2; A.values[5] = 60;
 *
 *  // A now represents the following matrix
 *  //    [10  0 20]
 *  //    [ 0  0  0]
 *  //    [ 0  0 30]
 *  //    [40 50 60]
 *
 *  // copy to the device
 *  cusp::csr_matrix<int,float,cusp::device_memory> A = B;
 *  \endcode
 *
 */
    template <typename IndexType, typename ValueType, class SpaceOrAlloc>
    class csr_matrix : public csr_pattern<IndexType, SpaceOrAlloc>
    {
        public:
        typedef typename cusp::choose_memory_allocator<ValueType, SpaceOrAlloc>::type value_allocator_type;
        typedef typename cusp::csr_matrix<IndexType, ValueType, SpaceOrAlloc> matrix_type;
    
        typedef ValueType value_type;
        
        template<typename SpaceOrAlloc2>
        struct rebind { typedef csr_matrix<IndexType, ValueType, SpaceOrAlloc2> type; };
        
        /*! Storage for the nonzero entries of the CSR data structure.
         */
        cusp::array1d<ValueType, value_allocator_type> values;
    
        /*! Construct an empty \p csr_matrix.
         */
        csr_matrix();
    
        /*! Construct a \p csr_matrix with a specific shape and number of nonzero entries.
         *
         *  \param num_rows Number of rows.
         *  \param num_cols Number of columns.
         *  \param num_entries Number of nonzero matrix entries.
         */
        csr_matrix(IndexType num_rows, IndexType num_cols, IndexType num_entries);
    
        /*! Construct a \p csr_matrix from another \p csr_matrix.
         *
         *  \param matrix Another \p csr_matrix.
         */
        template <typename IndexType2, typename ValueType2, typename SpaceOrAlloc2>
        csr_matrix(const csr_matrix<IndexType2, ValueType2, SpaceOrAlloc2>& matrix);
        
        /*! Construct a \p csr_matrix from another matrix format.
         *
         *  \param matrix Another sparse or dense matrix.
         */
        template <typename MatrixType>
        csr_matrix(const MatrixType& matrix);
        
        void resize(IndexType num_rows, IndexType num_cols, IndexType num_entries);

        /*! Swap the contents of two \p csr_matrix objects.
         *
         *  \param matrix Another \p csr_matrix with the same IndexType and ValueType.
         */
        void swap(csr_matrix& matrix);
        
        /*! Assignment from another \p csr_matrix.
         *
         *  \param matrix Another \p csr_matrix with possibly different IndexType and ValueType.
         */
        template <typename IndexType2, typename ValueType2, typename SpaceOrAlloc2>
        csr_matrix& operator=(const csr_matrix<IndexType2, ValueType2, SpaceOrAlloc2>& matrix);

        /*! Assignment from another matrix format.
         *
         *  \param matrix Another sparse or dense matrix.
         */
        template <typename MatrixType>
        csr_matrix& operator=(const MatrixType& matrix);
    }; // class csr_matrix
/*! \}
 */

} // end namespace cusp

#include <cusp/detail/csr_matrix.inl>

