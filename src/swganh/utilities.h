// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <type_traits>

namespace swganh {

    namespace detail {
        //// @TODO Remove comment when visual studio begins supporting constexpr.
        /*constexpr*/ bool inline isBigEndian() {
            uint16_t x = 1;
            return !(*reinterpret_cast<char*>(&x));
        }

		template<typename T>
		T swapBits(T value)
		{
			static_assert(std::is_arithmetic<T>::value, "swap_bits<T> requires T to be an integral type.");

			int8_t* bits = reinterpret_cast<int8_t*>(&value);

			for (int i = 0, end = sizeof(T) / 2; i < end; ++i)
			{
				std::swap(bits[i], bits[sizeof(T) - i - 1]);
			}

			return value;
		}
    }

    /*! Swaps the endianness of integral values and returns the results.
    *
    * @param value An integral value for which to swap the endianness.
    * @return A copy of the input parameter with its endianness swapped.
    */
    template<typename T>
    T swapEndian(T value) {
        static_assert(std::is_arithmetic<T>::value, "swap_endian<T> requires T to be an integral type.");
		return detail::swapBits(value);
    }

    /*! Converts an integral value from host-byte order to little endian.
    *
    * @param value An integral value to convert to little endian.
    * @return The value converted to little endian order.
    */
    template<typename T>
    T hostToLittle(T value) {
		static_assert(std::is_arithmetic<T>::value, "host_to_little<T> requires T to be an integral type.");
		return detail::isBigEndian() ? swapEndian(value) : value;
    }

    /*! Converts an integral value from host-byte order to big endian.
    *
    * @param value The value to convert to big endian
    * @return The value converted to big endian order.
    */
    template<typename T>
    T hostToBig(T value) {
		static_assert(std::is_arithmetic<T>::value, "host_to_big<T> requires T to be an integral type.");
		return detail::isBigEndian() ? value : swapEndian(value);
    }

    /*! Converts an integral value from big endian to host-byte order.
    *
    * @param value The value to convert to host-byte order.
    * @return The value converted to host-byte order.
    */
    template<typename T>
    T bigToHost(T value) {
		static_assert(std::is_arithmetic<T>::value, "big_to_host<T> requires T to be an integral type.");
		return detail::isBigEndian() ? value : swapEndian(value);
    }

    /*! Converts an integral value from little endian to host-byte order.
    *
    * @param value The value to convert to host-byte order.
    * @return The value converted to host-byte order.
    */
    template<typename T>
    T littleToHost(T value) {
		static_assert(std::is_arithmetic<T>::value, "little_to_host<T> requires T to be an integral type.");
		return detail::isBigEndian() ? swapEndian(value) : value;
    }

    int KeyboardHit();

    char GetHitKey();

    template<typename T, typename ...ArgsT>
    std::unique_ptr<T> make_unique(ArgsT&& ...args)
    {
        return std::unique_ptr<T>(new T(std::forward<ArgsT>(args)...));
    }

}  // namespace swganh
