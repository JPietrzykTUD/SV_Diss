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
 * @file factories.hpp
 * @author jpietrzyk
 * @date 26.08.22
 * @brief A brief description.
 *
 * @details A detailed description.
 */

#ifndef SHAREDVECTOR_SHAREDVECTOR_IOP_INPUT_INCLUDE_SV_INPUT_FACTORIES_HPP
#define SHAREDVECTOR_SHAREDVECTOR_IOP_INPUT_INCLUDE_SV_INPUT_FACTORIES_HPP

#include <sv_input_buffer_load.hpp>

namespace tuddbs{
  using namespace tvl;

  template<VectorProcessingStyle Vec>
  struct sv_input_factory {


    template<template<class> class SVInputProcessor, DataSource... Args>
      static auto create(Args const &... args) {
        ((std::cout << ( void * ) args.data() << '\n'), ...);
        static_assert(sizeof...(Args) <= Vec::vector_element_count(), "Number of data sources must be less or equal to available SIMD register slots.");
        auto slots_per_source = Vec::vector_element_count() / sizeof...(Args);
        auto args_array = []< size_t... Is >(auto tuple, std::index_sequence<Is...>) {
          return std::to_array({std::get<Is>(tuple).data()...});
        }(std::make_tuple(args...), std::make_index_sequence<sizeof...(Args)>());
        //      this should also work,... but the array only contains a single value...
        //      auto data_ptr_lambda = [](DataSource auto s) { return s.data(); };
        //      auto args_array = std::to_array({(data_ptr_lambda(args),...)});
        unsigned long args_pos = 0;
        auto target_pos = 0;
        auto current_increment = 0;
        typename Vec::base_type max_increment = 1;
        auto last_target_start_idx = 0;
        std::array<typename Vec::base_type const *, Vec::vector_element_count()> arr{};
        std::array<typename Vec::base_type, Vec::vector_element_count()> increment_arr{};

        if ((Vec::vector_element_count() % sizeof...(Args)) == 0) {
          while (target_pos != Vec::vector_element_count()) {
            arr[target_pos++] = args_array[args_pos] + current_increment;
            if ((target_pos % slots_per_source) == 0) {
              ++args_pos;
              current_increment = 0;
              for (int i = last_target_start_idx; i < target_pos; ++i) {
                increment_arr[i] = max_increment;
              }
              max_increment = 1;
              last_target_start_idx = target_pos;
            } else {
              ++current_increment;
              ++max_increment;
            }
          }
        } else {
          while ((args_pos < sizeof...(Args) - 1)) {
            arr[target_pos++] = args_array[args_pos] + current_increment;
            if ((target_pos % slots_per_source) == 0) {
              ++args_pos;
              current_increment = 0;
              for (int i = last_target_start_idx; i < target_pos; ++i) {
                increment_arr[i] = max_increment;
              }
              max_increment = 1;
              last_target_start_idx = target_pos;
            } else {
              ++current_increment;
              ++max_increment;
            }
          }
          current_increment = 0;
          max_increment = Vec::vector_element_count() - (args_pos * slots_per_source);
          while (target_pos != Vec::vector_element_count()) {
            arr[target_pos] = args_array[args_pos] + current_increment;
            ++current_increment;
            increment_arr[target_pos] = max_increment;
            ++target_pos;
          }
        }
        return SVInputProcessor<Vec>(arr, loadu<Vec>(increment_arr.data()));
      }
  };
}
#endif //SHAREDVECTOR_SHAREDVECTOR_IOP_INPUT_INCLUDE_SV_INPUT_FACTORIES_HPP
