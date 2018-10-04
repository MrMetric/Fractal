#pragma once

#include <complex>
#include <ostream>

// note to future me: change _ld if changing this!
using kompleks_type = long double;

struct kompleks
{
	constexpr kompleks()
	:
		real(0),
		imag(0)
	{
	}

	constexpr kompleks(const kompleks_type real, const kompleks_type imag)
	:
		real(real),
		imag(imag)
	{
	}

	constexpr explicit kompleks(const std::complex<kompleks_type>& z)
	:
		real(std::real(z)),
		imag(std::imag(z))
	{
	}

	kompleks_type real;
	kompleks_type imag;

	kompleks_type norm() const;
	kompleks_type abs() const;
	kompleks conjugate() const;
	kompleks reciprocal() const;
	kompleks_type arg() const;
	kompleks swap_xy() const;
	std::complex<kompleks_type> to_std() const;
};

bool operator==(const kompleks&, const kompleks&);
inline bool operator==(const kompleks& x, kompleks_type y)
{
	return x.real == y && x.imag == 0;
}

std::ostream& operator<<(std::ostream&, const kompleks&);

// + real
kompleks operator+(const kompleks&, kompleks_type);
kompleks operator+(kompleks_type, const kompleks&);

// + complex
kompleks operator+(const kompleks&, const kompleks&);

// - real
kompleks operator-(const kompleks&, kompleks_type);
// real -
kompleks operator-(kompleks_type, const kompleks&);

// - complex
kompleks operator-(const kompleks&, const kompleks&);

// * real
kompleks operator*(const kompleks&, kompleks_type);
kompleks operator*(kompleks_type, const kompleks&);

// * complex
kompleks operator*(const kompleks&, const kompleks&);
inline kompleks& operator*=(kompleks& x, const kompleks& y)
{
	x = x * y;
	return x;
}

// / real
kompleks operator/(const kompleks&, kompleks_type);
// real /
kompleks operator/(kompleks_type, const kompleks&);
// / complex
kompleks operator/(const kompleks&, const kompleks&);
kompleks operator^(kompleks, kompleks_type);
kompleks sinh(const kompleks&);
kompleks cos(const kompleks&);
