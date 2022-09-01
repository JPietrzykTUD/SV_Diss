// ------------------------------------------------------------------- //
/*
   This file is part of the SharedVector Project.
   Copyright (c) 2022 Johannes Pietrzyk.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.
 
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.
 
   You should have received a copy of the GNU General Public License 
   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// ------------------------------------------------------------------- //

/*
 * @file buffer_load.hpp
 * @author jpietrzyk
 * @date 26.08.22
 * @brief A brief description.
 *
 * @details A detailed description.
 */

#ifndef SHAREDVECTOR_SHAREDVECTOR_IOP_INPUT_INCLUDE_SV_INPUT_BUFFER_LOAD_HPP
#define SHAREDVECTOR_SHAREDVECTOR_IOP_INPUT_INCLUDE_SV_INPUT_BUFFER_LOAD_HPP


#include <tvlintrin.hpp>
#include <data_containers.hpp>

#include <iomanip>
#include <iostream>

namespace tuddbs {
  using namespace tvl;


//  template<VectorProcessingStyle Vec, uint16_t NumberOfPipelineOperators, uint16_t NumberOfColumns>
//  class sv_ip_buffer_load_uniform;
//
//  template<>
//  class sv_ip_buffer_load_uniform<simd<uint32_t, avx2>, 1, 1> {
//   private:
//    std::array<uint32_t const *, 8> m_sources;
//    uint32_t * m_buf;
//   public:
//    explicit sv_ip_buffer_load_uniform(
//        std::array<uint32_t const *, 8> p_sources
//    )
//    : m_sources{p_sources},
//      m_buf{allocate_aligned<simd<uint32_t, avx2>>(simd<uint32_t, avx2>::vector_size_B(), simd<uint32_t, avx2>::vector_alignment())} {}
//    ~sv_ip_buffer_load_uniform(){
//      if(m_buf!= nullptr) {
//        deallocate<simd<uint32_t, avx2>>(m_buf);
//      }
//    }
//    auto fetch(){
//      m_buf[0] = *m_sources[0]; m_sources[0] += 8;
//      m_buf[1] = *m_sources[1]; m_sources[1] += 8;
//      m_buf[2] = *m_sources[2]; m_sources[2] += 8;
//      m_buf[3] = *m_sources[3]; m_sources[3] += 8;
//      m_buf[4] = *m_sources[4]; m_sources[4] += 8;
//      m_buf[5] = *m_sources[5]; m_sources[5] += 8;
//      m_buf[6] = *m_sources[6]; m_sources[6] += 8;
//      m_buf[7] = *m_sources[7]; m_sources[7] += 8;
//      return load<simd<uint32_t, avx2>>(m_buf);
//    }
//  };
//  template<>
//  class TEST_sv_buffer_load_proxy<simd<uint32_t, avx2>,0> {
//   private:
//    std::array<uint32_t const *, 8> m_sources;
//    uint32_t * m_buf;
//   public:
//    explicit TEST_sv_buffer_load_proxy(
//        std::array<uint32_t const *, 8> p_sources
//    )
//        : m_sources{p_sources},
//          m_buf{allocate_aligned<simd<uint32_t, avx2>>(simd<uint32_t, avx2>::vector_size_B(), simd<uint32_t, avx2>::vector_alignment())} {}
//    ~TEST_sv_buffer_load_proxy(){
//      if(m_buf!= nullptr) {
//        deallocate<simd<uint32_t, avx2>>(m_buf);
//      }
//    }
//    auto fetch(){
//      m_buf[0] = *m_sources[0]; m_sources[0] += 8;
//      m_buf[1] = *m_sources[1]; m_sources[1] += 8;
//      m_buf[2] = *m_sources[2]; m_sources[2] += 8;
//      m_buf[3] = *m_sources[3]; m_sources[3] += 8;
//      m_buf[4] = *m_sources[4]; m_sources[4] += 8;
//      m_buf[5] = *m_sources[5]; m_sources[5] += 8;
//      m_buf[6] = *m_sources[6]; m_sources[6] += 8;
//      m_buf[7] = *m_sources[7]; m_sources[7] += 8;
//      return load<simd<uint32_t, avx2>>(m_buf);
//    }
//  };

  template<VectorProcessingStyle Vec>
  class sv_buffer_load_proxy {
   private:

   public:
    explicit sv_buffer_load_proxy(
        std::array<typename Vec::base_type const *, Vec::vector_element_count()> p_sources,
        typename Vec::register_type increment
    ) {
      std::cout << "[ ";
      for (std::size_t i = 0; i < Vec::vector_element_count(); ++i) {
        std::cout << std::setw(16) << ( void * ) p_sources[i] << " | ";
      }
      std::cout << " ]" << std::endl;
//      std::cout << "[ ";
//      for (std::size_t i = 0; i < Vec::vector_element_count(); ++i) {
//        std::cout << std::setw(16) << increment_arr[i] << " | ";
//      }
//      std::cout << " ]" << std::endl;
    }
  };
}

#endif //SHAREDVECTOR_SHAREDVECTOR_IOP_INPUT_INCLUDE_SV_INPUT_BUFFER_LOAD_HPP
