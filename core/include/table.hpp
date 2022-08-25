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
 * @file table.hpp
 * @author jpietrzyk
 * @date 25.08.22
 * @brief A brief description.
 *
 * @details A detailed description.
 */

#ifndef SHAREDVECTOR_CORE_TABLE_HPP_
#define SHAREDVECTOR_CORE_TABLE_HPP_


#include "tvlintrin.hpp"
#include <functional>
#include <utility>

namespace tuddbs {
  using namespace tvl;

  template<TVLArithmetic T>
  class column_t {
   private:
    using data_ptr_t = std::unique_ptr<T[], std::function<void(T*)>>;
    data_ptr_t m_data;

   public:
//    template<VectorProcessingStyle Vec>
//    friend column_t<typename Vec::base_type> create_aligned_column(size_t p_element_count, size_t alignment);
    friend struct column_factory;
   private:
    explicit column_t(data_ptr_t&& p_data)
    : m_data{std::move(p_data)} {}
  };

  struct column_factory {
    template<VectorProcessingStyle Vec>
      static auto create_aligned_column(size_t p_element_count, size_t alignment = Vec::vector_size_B()) -> column_t<typename Vec::base_type> {
        using T = typename Vec::base_type;
        static constexpr auto deallocator = [](T* ptr){deallocate<Vec>(ptr);};
        return column_t<T>{std::move(typename column_t<T>::data_ptr_t(allocate_aligned<Vec>(p_element_count*sizeof(T), alignment), deallocator))};
      }
  };

}
#endif //SHAREDVECTOR_CORE_TABLE_HPP_
