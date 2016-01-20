#pragma once

#include <iostream>
#include <cmath>
#include <complex>

// note to future me: change _ld if changing this!
#define kompleks_type long double

struct kompleks;
constexpr kompleks operator /(const kompleks& x, const kompleks_type y);
struct kompleks
{
	constexpr kompleks(const kompleks_type real = 0, const kompleks_type imag = 0) : real(real), imag(imag) {}
	constexpr kompleks(const std::complex<kompleks_type>& z) : real(std::real(z)), imag(std::imag(z)) {}

	kompleks_type real;
	kompleks_type imag;

	constexpr kompleks_type norm() const
	{
		return real*real + imag*imag;
	}

	constexpr kompleks_type abs() const
	{
		return std::sqrt(norm()); // TODO: more precision
	}

	constexpr kompleks conjugate() const
	{
		return kompleks(real, -imag);
	}

	constexpr kompleks reciprocal() const
	{
		return conjugate() / norm();
	}

	constexpr kompleks_type arg() const
	{
		return std::atan2(imag, real);
	}

	constexpr kompleks swap_xy() const
	{
		return kompleks(imag, real);
	}

	constexpr std::complex<kompleks_type> to_std() const
	{
		return std::complex<kompleks_type>(real, imag);
	}
};

bool operator ==(const kompleks& x, const kompleks& y)
{
	return x.real == y.real && x.imag == y.imag;
}

std::ostream& operator <<(std::ostream& o, const kompleks& x)
{
	if(x.imag < 0)
	{
		return o << "(" << x.real << " - " << -x.imag << "i)";
	}
	else
	{
		return o << "(" << x.real << " + " << x.imag << "i)";
	}
}

// + real
constexpr kompleks operator +(const kompleks& x, const kompleks_type y)
{
	return kompleks(x.real + y, x.imag);
}
constexpr kompleks operator +(const kompleks_type y, const kompleks& x)
{
	return x + y;
}

// + complex
constexpr kompleks operator +(const kompleks& x, const kompleks& y)
{
	return kompleks(x.real + y.real, x.imag + y.imag);
}

// - real
constexpr kompleks operator -(const kompleks& x, const kompleks_type y)
{
	return kompleks(x.real - y, x.imag);
}
// real -
constexpr kompleks operator -(const kompleks_type y, const kompleks& x)
{
	/*
	y - (a + bi)
	(y - a) - bi
	*/
	return kompleks(y - x.real, -x.imag);
}

// - complex
constexpr kompleks operator -(const kompleks& x, const kompleks& y)
{
	return kompleks(x.real - y.real, x.imag - y.imag);
}

// * real
constexpr kompleks operator *(const kompleks& x, const kompleks_type y)
{
	return kompleks(x.real * y, x.imag * y);
}
constexpr kompleks operator *(const kompleks_type y, const kompleks& x)
{
	return x * y;
}

// * complex
constexpr kompleks operator *(const kompleks& x, const kompleks& y)
{
	/*
	x = a + bi
	y = c + di

	(a + bi)(c + di)
	ac + bci + adi + bidi
	ac + bci + adi - bd
	(ac - bd) + (ad + bc)i
	*/

	return kompleks(x.real * y.real - x.imag * y.imag, x.real * y.imag + x.imag * y.real);
}

// / real
constexpr kompleks operator /(const kompleks& x, const kompleks_type y)
{
	return kompleks(x.real / y, x.imag / y);
}
// real /
constexpr kompleks operator /(const kompleks_type y, const kompleks& x)
{
	return y * x.reciprocal();
}

// / complex
constexpr kompleks operator /(const kompleks& x, const kompleks& y)
{
	return x * y.reciprocal();
}

const kompleks operator ^(kompleks x, kompleks_type y)
{
	int n = static_cast<int>(y);
	if(n != y)
	{
		return pow(x.to_std(), y);
	}
	//return pow(x.to_std(), (int)y);
	if(n == 0)
	{
		return 1;
	}
	if(x == 0 || n == 1)
	{
		return x;
	}
	bool negative = false;
	if(n < 0)
	{
		negative = true;
		n = -n;
	}

	// my slow solution
	/*
	bool odd = n % 2;
	int left = n / 2;
	kompleks result = (x^left);
	result = result*result;
	if(odd)
	{
		result = result*x;
	}
	*/

	// copied from std::complex
	kompleks result = (n % 2) ? x : 1;
	while(n >>= 1)
	{
		x = x*x;
		if(n % 2)
		{
			result = result*x;
		}
	}

	if(negative)
	{
		return result.reciprocal();
	}
	return result;
}

kompleks sinh(const kompleks& z)
{
	const kompleks_type x = z.real;
	const kompleks_type y = z.imag;
	return kompleks(std::sinh(x) * std::cos(y), std::cosh(x) * std::sin(y));
}

kompleks cos(const kompleks& z)
{
	return kompleks(std::cos(z.real) * std::cosh(z.imag), -1 * std::sin(z.real) * std::sinh(z.imag));
}