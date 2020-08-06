#pragma once

#include <limits>
#include <type_traits>
#include <typeinfo>

namespace nonstd
{
/**
 * Attempts to convert one arithmetic type to another. If the destination *type* cannot
 * hold the source *value* without loss of precision, an exception is thrown.
 *
 * If the success of the conversion can be determined at compile time,
 * the value will not be inspected.
 */
template <typename To, typename From>
constexpr To narrow( From x )
{
	using _From = std::remove_reference_t<std::remove_cv_t<From>>;

	static_assert(
	    std::is_same_v<To, std::remove_reference_t<std::remove_cv_t<To>>>,
	    "narrow() may only return value types" );
	static_assert(
	    std::is_arithmetic_v<_From> && std::is_arithmetic_v<To>,
	    "narrow() is inteded for arithmetic types only" );
	static_assert( !std::is_same_v<_From, To>, "pointless conversion between identical types" );

	constexpr bool from_real = std::is_floating_point_v<_From>;
	constexpr bool from_int = std::is_integral_v<_From>;
	constexpr bool from_signed = std::is_signed_v<_From>;
	constexpr bool from_unsigned = std::is_unsigned_v<_From>;
	constexpr bool to_real = std::is_floating_point_v<To>;
	constexpr bool to_int = std::is_integral_v<To>;
	constexpr bool to_unsigned = std::is_unsigned_v<To>;
	constexpr auto from_size = std::numeric_limits<_From>::digits;
	constexpr auto to_size = std::numeric_limits<To>::digits;
	constexpr auto to_min = std::numeric_limits<To>::min();
	constexpr auto to_max = std::numeric_limits<To>::max();

	// int -> int
	if constexpr( from_int && to_int )
	{
		// signed -> unsigned
		if constexpr( from_signed && to_unsigned )
			if( x < 0 ) throw std::bad_cast();

		// signed -> signed
		// unsigned -> unsigned
		// unsigned -> signed
		if constexpr( to_size >= from_size )
			return static_cast<To>( x );
		else if( ( from_unsigned || x >= to_min ) && x <= to_max )
			return static_cast<To>( x );
	}

	// int -> real
	if constexpr( from_int && to_real )
	{
		if constexpr( to_size >= from_size ) return static_cast<To>( x );

		std::make_unsigned_t<_From> y;
		if constexpr( from_unsigned )
			y = x;
		else
			y = std::abs( x );

		if( y <= 1llu << to_size ) return static_cast<To>( x );

		int a = std::numeric_limits<decltype( y )>::digits - 1;
		int b = 0;

		for( ; a >= 0; a-- )
			if( ( y >> a ) & 1 ) break;
		for( ; b <= a; b++ )
			if( ( y >> b ) & 1 ) break;

		if( a - b + 1 <= to_size ) return static_cast<To>( x );
	}

	// real -> int
	if constexpr( from_real && to_int )
	{
		_From tmp;
		auto frac = modf( x, &tmp );

		if( frac == 0 && x >= to_min && x < std::exp2( to_size ) ) return static_cast<To>( x );
	}

	// real -> real
	if constexpr( from_real && to_real )
	{
		if constexpr( to_size >= from_size ) return static_cast<To>( x );
		if( x == static_cast<_From>( static_cast<To>( x ) ) ) return static_cast<To>( x );
	}

	throw std::bad_cast();
}
} // namespace nonstd
