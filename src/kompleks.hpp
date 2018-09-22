#pragma once

#include <iostream>
#include <cmath>
#include <complex>

// note to future me: change _ld if changing this!
using kompleks_type = long double;

struct kompleks
{
	constexpr kompleks(const kompleks_type real = 0, const kompleks_type imag = 0) : real(real), imag(imag) {}
	constexpr kompleks(const std::complex<kompleks_type>& z) : real(std::real(z)), imag(std::imag(z)) {}

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

bool operator ==(const kompleks& x, const kompleks& y);

std::ostream& operator <<(std::ostream& o, const kompleks& x);

// + real
kompleks operator +(const kompleks& x, const kompleks_type y);
kompleks operator +(const kompleks_type y, const kompleks& x);

// + complex
kompleks operator +(const kompleks& x, const kompleks& y);

// - real
kompleks operator -(const kompleks& x, const kompleks_type y);
// real -
kompleks operator -(const kompleks_type y, const kompleks& x);

// - complex
kompleks operator -(const kompleks& x, const kompleks& y);

// * real
kompleks operator *(const kompleks& x, const kompleks_type y);
kompleks operator *(const kompleks_type y, const kompleks& x);

// * complex
kompleks operator *(const kompleks& x, const kompleks& y);

// / real
kompleks operator /(const kompleks& x, const kompleks_type y);
// real /
kompleks operator /(const kompleks_type y, const kompleks& x);
// / complex
kompleks operator /(const kompleks& x, const kompleks& y);
kompleks operator ^(kompleks x, kompleks_type y);
kompleks sinh(const kompleks& z);
kompleks cos(const kompleks& z);
