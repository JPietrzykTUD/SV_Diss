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
#include <array>

namespace tuddbs {


  template<typename T>
  concept DataContainer =
    requires {
      typename T::data_type;
    } &&
    requires(T const & o){
      { o.element_count() } -> std::same_as<size_t>;
    };
  template<typename T>
  concept DataSource = DataContainer<T> &&
    requires(T const & o) {
      { o.data() } -> std::same_as<typename T::data_type const *>;
    };
  template<typename T>
  concept DataSink = DataContainer<T> &&
      requires(T & o) {
        { o.data() } -> std::same_as<typename T::base_type *>;
      };


  using namespace tvl;

  template<TVLArithmetic T>
  class column {
   public:
    using data_ptr_t = std::unique_ptr<T[], std::function<void(T*)>>;
    using data_type = T;
   private:
    data_ptr_t m_data;
    size_t m_element_count;

   public:
    friend struct column_factory;
   private:
    column(data_ptr_t&& p_data, size_t p_element_count)
    : m_data{std::move(p_data)},
      m_element_count{p_element_count}{}

   public:
    class iterator {
     public:
      using data_type         = T;
      using iterator_category = std::input_iterator_tag;
      using difference_type   = std::ptrdiff_t;
      using value_type        = data_type;
      using pointer           = data_type*;
      using reference         = data_type&;
     public:
      explicit iterator(pointer p_ptr, size_t p_stepwidth)
          : m_ptr{p_ptr},
            m_stepwidth{p_stepwidth}{}
      auto operator*() const -> reference {return *m_ptr;}
      auto operator->() -> pointer {return m_ptr;}
      auto data() const -> const data_type * { return m_ptr; }
      auto end_data() const -> pointer { return m_ptr + m_stepwidth; }
      auto operator++() -> iterator & {
        m_ptr += m_stepwidth;
        return *this;
      }
      auto operator++(int) -> iterator {
        auto tmp = *this;
        ++(*this);
        return tmp;
      }
      friend auto operator==(const iterator & a, const iterator & b ) {
        return a.m_ptr == b.m_ptr;
      }
      friend auto operator!=(const iterator & a, const iterator & b ) {
        return a.m_ptr != b.m_ptr;
      }
      auto element_count() const -> size_t { return m_stepwidth; }
     private:
      pointer m_ptr;
      size_t m_stepwidth;
    };

    template<uint32_t PartitionElementCount>
    class logical_partition_iterator {
     public:
      using data_type         = T;
      using iterator_category = std::input_iterator_tag;
      using difference_type   = std::ptrdiff_t;
      using value_type        = data_type;
      using pointer           = data_type*;
      using reference         = data_type&;
     public:
      explicit logical_partition_iterator(pointer p_ptr)
          : m_ptr{p_ptr}{}
      auto operator*() const -> reference {return *m_ptr;}
      auto operator->() -> pointer {return m_ptr;}
      auto data() -> const pointer { return m_ptr; }
      auto end_data() const -> pointer { return m_ptr + PartitionElementCount; }
      auto operator++() -> logical_partition_iterator & {
        m_ptr += PartitionElementCount;
        return *this;
      }
      auto operator++(int) -> logical_partition_iterator {
        auto tmp = *this;
        ++(*this);
        return tmp;
      }
      friend auto operator==(const logical_partition_iterator & a, const logical_partition_iterator & b) {
        return a.m_ptr == b.m_ptr;
      }
      friend auto operator!=(const logical_partition_iterator & a, const logical_partition_iterator & b) {
        return a.m_ptr != b.m_ptr;
      }
      friend auto operator==(const logical_partition_iterator & a, const iterator & b) {
        return a.m_ptr == b.data();
      }
      friend auto operator!=(const logical_partition_iterator & a, const iterator & b) {
        return a.m_ptr != b.data();
      }
      constexpr auto element_count() const -> size_t { return PartitionElementCount; }
     private:
      pointer m_ptr;
    };


   public:
    auto data() const -> const data_type * {
      return m_data.get();
    }
    auto data() -> data_type * {
      return m_data.get();
    }
    [[nodiscard]] auto element_count() const -> size_t {
      return m_element_count;
    }
    template<uint32_t PartitionSize>
    auto iterators() const {
      static_assert(((PartitionSize%sizeof(data_type))==0), "PartitionSize must be a multiple of data_type size.");
      auto ptr = m_data.get();
      if constexpr (PartitionSize==0) {
        auto end = iterator(ptr+m_element_count, 0);
        return std::make_tuple(
            iterator(ptr, m_element_count),
            end, end
        );
      } else {
        auto full_partition_remainder_count = m_element_count % (PartitionSize/sizeof(data_type));
        auto full_partition_end = m_element_count - full_partition_remainder_count;
        return std::make_tuple(
            logical_partition_iterator<PartitionSize / sizeof(data_type)>(ptr),
            iterator(ptr + full_partition_end, 0),
            iterator(ptr + m_element_count, 0)
        );
      }
    }
    template<uint32_t PartitionSize>
    auto begin() const {
      static_assert(((PartitionSize%sizeof(data_type))==0), "PartitionSize must be a multiple of data_type size.");
      auto ptr = m_data.get();
      if constexpr (PartitionSize==0) {
        return iterator(ptr, m_element_count);
      } else {
        return logical_partition_iterator<PartitionSize / sizeof(data_type)>(ptr);
      }
    }
  };

  template<uint32_t PartitionSize>
  struct logical_partitioner {
    template<TVLArithmetic T>
    auto iterators(const column<T> & c) {
      return c.template iterators<PartitionSize>();
    }
    template<TVLArithmetic T>
    auto begin(const column<T> & c) {
      return c.template begin<PartitionSize>();
    }
    template<typename... Args>
    auto begin(const Args &... args) {
      return std::make_tuple(args.template begin<PartitionSize>()...);
    }
  };

  struct column_factory {
    template<VectorProcessingStyle Vec>
      static auto create_aligned_column(size_t p_element_count, size_t alignment = Vec::vector_size_B()) -> column<typename Vec::base_type> {
        using T = typename Vec::base_type;
        static constexpr auto deallocator = [](T* ptr){deallocate<Vec>(ptr);};
        return column<T>{std::move(typename column<T>::data_ptr_t(allocate_aligned<Vec>(p_element_count*sizeof(T), alignment), deallocator)), p_element_count};
      }
  };

}
#endif //SHAREDVECTOR_CORE_TABLE_HPP_
