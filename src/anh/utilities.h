/*
 This file is part of MMOServer. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2010 The SWG:ANH Team

 MMOServer is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 MMOServer is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with MMOServer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ANH_UTILITIES_H_
#define ANH_UTILITIES_H_

#include <cstdint>
#include <algorithm>
#include <limits>

namespace anh {

    namespace detail {
        /// @TODO Remove quotes from around constexpr when visual studio begins
        /// supporting that c++0x feature.
        /*constexpr*/ bool inline is_big_endian() {
            uint16_t x = 1;
            return !(*reinterpret_cast<char*>(&x));
        }

        struct bit8_tag{};
        struct bit16_tag{};
        struct bit32_tag{};
        struct bit64_tag{};

        template<typename T> struct integer_traits;
        
        template<>
        struct integer_traits<int8_t> {
            typedef bit8_tag category;
        };
        
        template<>
        struct integer_traits<uint8_t> {
            typedef bit8_tag category;
        };
        
        template<>
        struct integer_traits<int16_t> {
            typedef bit16_tag category;
        };
        
        template<>
        struct integer_traits<uint16_t> {
            typedef bit16_tag category;
        };
        
        template<>
        struct integer_traits<int32_t> {
            typedef bit32_tag category;
        };
        
        template<>
        struct integer_traits<uint32_t> {
            typedef bit32_tag category;
        };
        
        template<>
        struct integer_traits<int64_t> {
            typedef bit64_tag category;
        };
        
        template<>
        struct integer_traits<uint64_t> {
            typedef bit64_tag category;
        };
        
        template<typename T>
        T swap_endian_(T value, bit8_tag) {
            return value;
        }

        template<typename T>
        T swap_endian_(T value, bit16_tag) {
            return (value >> 8) | (value << 8);
        }
        
        template<typename T>
        T swap_endian_(T value, bit32_tag) {
            return (value >> 24) |
                ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) |
                (value << 24);
        }
        
        template<typename T>
        T swap_endian_(T value, bit64_tag) {
            return (value  >> 56) |
            #ifdef _WIN32
                ((value & 0x00FF000000000000) >> 40) |
                ((value & 0x0000FF0000000000) >> 24) |
                ((value & 0x000000FF00000000) >> 8)  |
                ((value & 0x00000000FF000000) << 8)  |
                ((value & 0x0000000000FF0000) << 24) |
                ((value & 0x000000000000FF00) << 40) |
            #else
                ((value & 0x00FF000000000000LLU) >> 40) |
                ((value & 0x0000FF0000000000LLU) >> 24) |
                ((value & 0x000000FF00000000LLU) >> 8)  |
                ((value & 0x00000000FF000000LLU) << 8)  |
                ((value & 0x0000000000FF0000LLU) << 24) |
                ((value & 0x000000000000FF00LLU) << 40) |
            #endif
            (value  << 56);
        }
    }

    /*! Swaps the endianness of the passed in value and returns the results.
    *
    * For standard integer types (any of the intX_t/uintX_t types)
    * specializations exist to ensure the fastest performance. All other types
    * are treated as char* and reversed.
    */
    template<typename T> 
    T swap_endian(T value) {
        if (std::numeric_limits<T>::is_integer) {
            return detail::swap_endian_<T>(value, detail::integer_traits<T>::category());
        }

        unsigned char* tmp = reinterpret_cast<unsigned char*>(&value);
        std::reverse(tmp, tmp + sizeof(T));
        return value;
    }

    /*! Converts a value from host-byte order to little endian.
    *
    * Only works with integer types.
    *
    * \param value The value to convert to little endian
    * \return The value converted to endian order.
    */
    template<typename T>
    T host_to_little(T value) {
        static_assert(std::numeric_limits<T>::is_integer);
        return detail::is_big_endian() ? swap_endian(value) : value;
    }
    
    /*! Converts a value from host-byte order to big endian.
    *
    * Only works with integer types.
    *
    * \param value The value to convert to big endian
    * \return The value converted to endian order.
    */
    template<typename T>
    T host_to_big(T value) {
        static_assert(std::numeric_limits<T>::is_integer);
        return detail::is_big_endian() ? value : swap_endian(value);
    }
    
    /*! Converts a value from big endian to host-byte order.
    *
    * Only works with integer types.
    *
    * \param value The value to convert to host-byte order.
    * \return The value converted to host-byte order.
    */
    template<typename T>
    T big_to_host(T value) {
        static_assert(std::numeric_limits<T>::is_integer);
        return detail::is_big_endian() ? value : swap_endian(value);
    }
    
    /*! Converts a value from little endian to host-byte order.
    *
    * Only works with integer types.
    *
    * \param value The value to convert to host-byte order.
    * \return The value converted to host-byte order.
    */
    template<typename T>
    T little_to_host(T value) {
        static_assert(std::numeric_limits<T>::is_integer);
        return detail::is_big_endian() ? swap_endian(value) : value;
    }
        
}  // namespace anh

#endif  // ANH_UTILITIES_H_
