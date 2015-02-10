#pragma once
#include <iostream>
#include <cmath>
#include <complex>

#define kompleks_type double

constexpr inline kompleks_type square(kompleks_type x)
{
	return x*x;
}

struct kompleks;
constexpr kompleks operator /(const kompleks& x, const kompleks_type y);
struct kompleks
{
	constexpr kompleks(const kompleks_type real = 0, const kompleks_type imag = 0) : real(real), imag(imag) {}
	constexpr kompleks(const std::complex<kompleks_type>& z) : real(std::real(z)), imag(std::imag(z)) {}

	kompleks_type real, imag;

	constexpr kompleks_type norm()
	{
		return real*real + imag*imag;
	}

	constexpr kompleks_type abs()
	{
		return sqrt(norm());
	}

	constexpr kompleks conjugate()
	{
		return kompleks(real, -imag);
	}

	constexpr kompleks reciprocal()
	{
		return conjugate() / square(abs());
	}

	constexpr kompleks_type arg()
	{
		return atan2(imag, real);
	}

	constexpr kompleks swap_xy()
	{
		return kompleks(imag, real);
	}

	constexpr std::complex<kompleks_type> to_std()
	{
		return std::complex<kompleks_type>(real, imag);
	}
};

bool operator ==(const kompleks& x, const kompleks& y)
{
	return x.real == y.real && x.imag == y.imag;
}

std::ostream& operator <<(std::ostream &o, const kompleks x)
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
constexpr kompleks operator +(const kompleks& x, const kompleks y)
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
	x - (a + bi)
	(x - a) - bi
	*/
	return kompleks(y - x.real, -x.imag);
}

// - complex
constexpr kompleks operator -(const kompleks& x, const kompleks y)
{
	return kompleks(x.real - y.real, x.imag - y.imag);
}

// * real
constexpr kompleks operator *(const kompleks& x, const kompleks_type y)
{
	return kompleks(x.real * y, x.imag * y);
}
constexpr kompleks operator *(const kompleks_type y, const kompleks x)
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
	ac - bd + (ad + bc)i
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
	if((int)y != y)
	{
		return pow(x.to_std(), y);
	}
	//return pow(x.to_std(), (int)y);
	int n = (int)y;
	if(n == 0)
	{
		return 1;
	}
	if(x.real == 0 && x.imag == 0)
	{
		return 0;
	}
	if(n == 1)
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
	/*bool odd = n % 2;
	int left = n / 2;
	kompleks result = (x^left);
	result = result*result;
	if(odd)
	{
		result = result*x;
	}*/

	// copied from std::complex
	kompleks result = n % 2 ? x : 1;
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

kompleks std_sinh(const kompleks& z)
{
	const kompleks_type x = z.real;
	const kompleks_type y = z.imag;
	return kompleks(sinh(x) * cos(y), cosh(x) * sin(y));
}

/*kompleks std_sqrt(const kompleks& z)
{
	kompleks_type x = z.real;
	kompleks_type y = z.imag;

	if (x == kompleks_type())
	{
		kompleks_type t = sqrt(abs(y) / 2);
		return kompleks(t, y < kompleks_type() ? -t : t);
	}
	else
	{
		kompleks_type t = sqrt(2 * (z.abs() + abs(x)));
		kompleks_type u = t / 2;
		return x > kompleks_type()
			? kompleks(u, y / t)
			: kompleks(abs(y) / t, y < kompleks_type() ? -u : u);
	}
}*/

void testkompleks()
{
	kompleks x(1, 2);
	kompleks y(3, -4);
	std::complex<double> x2(x.real, x.imag);
	std::complex<double> y2(y.real, y.imag);

	std::cout << "x = " << x << " : " << x2 << "\n";
	std::cout << "y = " << y << " : " << y2 << "\n";
	std::cout << "\n";

	std::cout << "x + 2 = " << (x + 2) << " : " << (x2 + 2.0) << "\n";
	std::cout << "2 + x = " << (2 + x) << " : " << (2.0 + x2) << "\n";
	std::cout << "\n";
	std::cout << "x + y = " << (x + y) << " : " << (x2 + y2) << "\n";
	std::cout << "y + x = " << (y + x) << " : " << (y2 + x2) << "\n";
	std::cout << "\n";

	std::cout << "x - 2 = " << (x - 2) << " : " << (x2 - 2.0) << "\n";
	std::cout << "2 - x = " << (2 - x) << " : " << (2.0 - x2) << "\n";
	std::cout << "\n";
	std::cout << "x - y = " << (x - y) << " : " << (x2 - y2) << "\n";
	std::cout << "y - x = " << (y - x) << " : " << (y2 - x2) << "\n";
	std::cout << "\n";

	std::cout << "x * 2 = " << (x * 2) << " : " << (x2 * 2.0) << "\n";
	std::cout << "2 * x = " << (2 * x) << " : " << (2.0 * x2) << "\n";
	std::cout << "\n";
	std::cout << "x * y = " << (x * y) << " : " << (x2 * y2) << "\n";
	std::cout << "y * x = " << (y * x) << " : " << (y2 * x2) << "\n";
	std::cout << "\n";

	std::cout << "x / 2 = " << (x / 2) << " : " << (x2 / 2.0) << "\n";
	std::cout << "2 / x = " << (2 / x) << " : " << (2.0 / x2) << "\n";
	std::cout << "\n";
	std::cout << "x / y = " << (x / y) << " : " << (x2 / y2) << "\n";
	std::cout << "y / x = " << (y / x) << " : " << (y2 / x2) << "\n";
	std::cout << "\n";

	std::cout << "abs(x) = " << x.abs() << " : " << abs(x2) << "\n";
	std::cout << "abs(y) = " << y.abs() << " : " << abs(y2) << "\n";
	std::cout << "\n";
	std::cout << "x conjugate = " << x.conjugate() << "\n";
	std::cout << "y conjugate = " << y.conjugate() << "\n";
	std::cout << "\n";
	std::cout << "arg(x) = " << x.arg() << " : " << arg(x2) << "\n";
	std::cout << "arg(y) = " << y.arg() << " : " << arg(y2) << "\n";
	std::cout << "\n";

	std::cout << "x^-1 = " << (x^-1) << " : " << pow(x2, -1) << "\n";
	std::cout << "x^0 = " << (x^0) << " : " << pow(x2, 0) << "\n";
	std::cout << "x^1 = " << (x^1) << " : " << pow(x2, 1) << "\n";
	std::cout << "x^2 = " << (x^2) << " : " << pow(x2, 2) << "\n";
	std::cout << "x^2.5 = " << (x^2.5) << " : " << pow(x2, 2.5) << "\n";
	std::cout << "x^3 = " << (x^3) << " : " << pow(x2, 3) << "\n";
	std::cout << "\n";

	// test zero
	kompleks z(0, 0);
	std::complex<double> z2(0, 0);
	std::cout << "z = " << z << " : " << z2 << "\n";
	std::cout << "\n";

	std::cout << "z / 2 = " << (z / 2) << " : " << (z2 / 2.0) << "\n";
	std::cout << "2 / z = " << (2 / z) << " : " << (2.0 / z2) << "\n";
	std::cout << "\n";

	std::cout << "abs(z) = " << z.abs() << " : " << (abs(z2)) << "\n";
	std::cout << "arg(z) = " << z.arg() << " : " << (arg(z2)) << "\n";
	std::cout << "z^0 = " << (z^0) << " : " << pow(z2, 0) << "\n";
	//std::cout << "0^z = " << (0^z) << " : " << pow(0, z2) << "\n";
	std::cout << "z^2 = " << (z^2) << " : " << pow(z2, 2) << "\n";
}