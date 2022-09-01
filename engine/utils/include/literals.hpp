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
 * @file literals.hpp
 * @author jpietrzyk
 * @date 26.08.22
 * @brief A brief description.
 *
 * @details A detailed description.
 */

#ifndef SHAREDVECTOR_SHAREDVECTOR_UTILS_INCLUDE_LITERALS_HPP_
#define SHAREDVECTOR_SHAREDVECTOR_UTILS_INCLUDE_LITERALS_HPP_

namespace tuddbs {
  constexpr unsigned long long int operator""_b(unsigned long long int v) {
    return (v > 7) ? (v>>3) : 1;
  }
  constexpr unsigned long long int operator""_B(unsigned long long int v) {
    return v;
  }
  constexpr unsigned long long int operator""_KiB(unsigned long long int v) {
    return v<<10;
  }
  constexpr unsigned long long int operator""_MiB(unsigned long long int v) {
    return v<<20;
  }
  constexpr unsigned long long int operator""_GiB(unsigned long long int v) {
    return v<<30;
  }
}
#endif //SHAREDVECTOR_SHAREDVECTOR_UTILS_INCLUDE_LITERALS_HPP_
