#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "kompleks.hpp"
#include <cstdlib>
#include <ctime>
#include <cerrno>
#include <strings.h>
#include <signal.h>
#include <cstring>
#include <png++/png.hpp>
#include <exception>
#include "ArgParser.hpp"

#include <cstdint>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

inline void clearLine(uint_fast32_t spaces)
{
	std::cout << "\r" << std::string(spaces, ' ') << std::flush;
}

enum FractalType
{
	mandelbrot,
	julia,
	burning_ship,
	tricorn,
	neuron,
	clouds,
	oops,
	stupidbrot,
	untitled1,
	dots,
	magnet1,
	experiment,
	mandelbox,
	negamandelbrot,
};

const uint_fast8_t type_string_count = 14;
const char* type_strings[type_string_count] =
{
	"mandelbrot",
	"julia",
	"burning ship",
	"tricorn",
	"neuron",
	"clouds",
	"oops",
	"stupidbrot",
	"untitled 1",
	"dots",
	"magnet 1",
	"experiment",
	"mandelbox",
	"negamandelbrot",
};

FractalType string_to_fractal_type(const char* typestr)
{
	for(uint_fast8_t i = 0; i < type_string_count; ++i)
	{
		if(strcasecmp(typestr, type_strings[i]) == 0)
		{
			return static_cast<FractalType>(i);
		}
	}

	throw std::runtime_error("Unknown fractal type: " + std::string(typestr));
}

struct FractalOptions
{
	static FractalType type;
	static kompleks_type exponent;
	static kompleks_type escape_limit;
	static bool single;
	static kompleks_type lbound;
	static kompleks_type rbound;
	static kompleks_type bbound;
	static kompleks_type ubound;
	static kompleks_type juliaA;
	static kompleks_type juliaB;
} fractal_opt;
FractalType FractalOptions::type = mandelbrot;
kompleks_type FractalOptions::exponent = 2;
kompleks_type FractalOptions::escape_limit = 4;
bool FractalOptions::single = false;
kompleks_type FractalOptions::lbound = 2;
kompleks_type FractalOptions::rbound = 2;
kompleks_type FractalOptions::bbound = 2;
kompleks_type FractalOptions::ubound = 2;
kompleks_type FractalOptions::juliaA = -0.8;
kompleks_type FractalOptions::juliaB = 0.156;

struct ColorOptions
{
	static uint_fast16_t method;
	static bool smooth;
	static bool disable_fancy;
	static kompleks_type multiplier;
} color_opt;
uint_fast16_t ColorOptions::method = 0;
bool ColorOptions::smooth = false;
bool ColorOptions::disable_fancy = false;
kompleks_type ColorOptions::multiplier = 1;

// https://github.com/kobalicek/rgbhsv/blob/master/src/rgbhsv.cpp
void HSV2RGB(kompleks_type h, kompleks_type s, kompleks_type v, uint_fast8_t dst[3])
{
	if(h >= 1)
	{
		h -= 1;
	}
	h *= 6;

	uint_fast8_t index = static_cast<uint_fast8_t>(h);
	double f = h - static_cast<double>(index);
	double p = (v * (1.0f - s)) * 255;
	double q = (v * (1.0f - s * f)) * 255;
	double t = (v * (1.0f - s * (1.0f - f))) * 255;
	v *= 255;

	switch(index)
	{
		case 0: dst[0] = v; dst[1] = t; dst[2] = p; break;
		case 1: dst[0] = q; dst[1] = v; dst[2] = p; break;
		case 2: dst[0] = p; dst[1] = v; dst[2] = t; break;
		case 3: dst[0] = p; dst[1] = q; dst[2] = v; break;
		case 4: dst[0] = t; dst[1] = p; dst[2] = v; break;
		case 5: dst[0] = v; dst[1] = p; dst[2] = q; break;
	}
}

const png::rgb_pixel colorize(uint_fast32_t color_method, const kompleks& Z, const kompleks& c, uint_fast64_t n)
{
	uint_fast64_t red, green, blue;
	kompleks_type Zr2 = Z.real*Z.real;
	kompleks_type Zi2 = Z.imag*Z.imag;
	switch(color_method)
	{
		case 0: // escape time (gold)
		{
			if(color_opt.smooth)
			{
				// from http://www.hpdz.net/TechInfo/Colorizing.htm#FractionalCounts
				kompleks_type dx = (log(log(fractal_opt.escape_limit)) - log(log(Z.abs()))) / log(fractal_opt.exponent);
				kompleks_type nprime = n + dx;
				red = round(nprime * 2);
				green = round(nprime);
				blue = round(nprime / 2);
			}
			else
			{
				red = n << 1;
				green = n;
				blue = n >> 1;
			}

			break;
		}
		case 1: // escape time (green + some shit)
		{
			if(!color_opt.disable_fancy)
			{
				red = Zr2;
				blue = Zi2;
			}
			else
			{
				red = 0;
				blue = 0;
			}
			if(color_opt.smooth)
			{
				kompleks_type dx = (log(log(fractal_opt.escape_limit)) - log(log(Z.abs()))) / log(fractal_opt.exponent);
				green = round(n + dx);
			}
			else
			{
				green = n;
			}
			if(green > 255)
			{
				uint_fast32_t difference = green - 255;
				green = 255;
				blue = difference << 1;
				if(blue > 255)
				{
					red = blue << 1;
					blue = 200;
					green = 200;
				}
			}
			break;
		}
		case 2: // lazer shit 1
		{
			red = Zr2 * Zi2;
			green = Zr2 + Zi2;
			if(Zi2 == 0)
			{
				blue = 255;
			}
			else
			{
				blue = Zr2 / Zi2;
			}
			break;
		}
		case 3: // lazer shit 2
		{
			if(Zr2 == 0)
			{
				red = 255;
				green = 255;
			}
			else
			{
				red = (Zr2 * Zr2 * Zr2 + 1) / Zr2;
				green = Zi2 / Zr2;
			}
			blue = Zi2 * Zi2;
			break;
		}
		case 4: // Ben
		{
			red = green = blue = Z.real * sin(Z.imag + Zi2) - Zr2;
			break;
		}
		case 5: // Glow (Green)
		{
			if(Zr2 <= (1.0 / UINT_FAST64_MAX))
			{
				red = UINT_FAST64_MAX;
			}
			else
			{
				red = round(1 / Zr2);
			}
			if(Zr2 <= 0.00588)
			{
				green = UINT_FAST64_MAX;
			}
			else
			{
				green = round(1.5 / Zr2);
			}
			if(Zr2 <= 0.00294)
			{
				blue = UINT_FAST64_MAX;
			}
			else
			{
				blue = round(0.75 / Zr2);
			}
			break;
		}
		case 6: // Glow (Pink)
		{
			if(Zr2 == 0)
			{
				red = UINT_FAST64_MAX;
			}
			else
			{
				red = round(1.5 / Zr2);
			}
			if(Zr2 == 0)
			{
				green = UINT_FAST64_MAX;
			}
			else
			{
				green = round(0.75 / Zr2);
			}
			if(Zr2 == 0)
			{
				blue = UINT_FAST64_MAX;
			}
			else
			{
				blue = round(1 / Zr2);
			}
			break;
		}
		case 7: // Glow (Blue)
		{
			if(Zr2 <= 0.00294)
			{
				red = UINT_FAST64_MAX;
			}
			else
			{
				red = round(0.75 / Zr2);
			}
			if(Zr2 <= 0.00392)
			{
				green = UINT_FAST64_MAX;
			}
			else
			{
				green = round(1 / Zr2);
			}
			if(Zr2 <= 0.00588)
			{
				blue = UINT_FAST64_MAX;
			}
			else
			{
				blue = round(1.5 / Zr2);
			}
			break;
		}
		case 8: // Bright pink with XOR
		{
			if(Zr2 == 0)
			{
				red = 255; // TODO: UINT64_MAX?
			}
			else
			{
				red = Zi2 / Zr2 + (n << 1);
			}
			if(Zi2 == 0)
			{
				green = 255;
			}
			else
			{
				green = Zr2 / Zi2 + n;
			}
			blue = static_cast<uint_fast64_t>(round(Zi2 * 255)) ^ static_cast<uint_fast64_t>(round(Zr2 * 255));
			red += blue * 0.5;
			green += blue * 0.2;
			break;
		}
		case 9:
		{
			png::rgb_pixel color_fractal = colorize(0, Z, c, n);
			uint_fast64_t red_fractal = color_fractal.red,
						  green_fractal = color_fractal.green,
						  blue_fractal = color_fractal.blue;

			red = static_cast<uint_fast64_t>(round(Zr2*8)) ^ static_cast<uint_fast64_t>(round(Zi2*8));
			green = static_cast<uint_fast64_t>(round(Zr2*2)) ^ static_cast<uint_fast64_t>(round(Zi2*2));
			blue = static_cast<uint_fast64_t>(round(Zr2*4)) ^ static_cast<uint_fast64_t>(round(Zi2*4));

			// darken the colors a bit
			red *= 0.7;
			green *= 0.7;
			blue *= 0.7;

			uint_fast64_t blue_stripe;
			if(Zr2 == 0)
			{
				blue_stripe = 255;
			}
			else
			{
				blue_stripe = static_cast<uint_fast64_t>(round(Zi2 / Zr2));
			}
			uint_fast64_t green_stripe;
			if(Zi2 == 0)
			{
				green_stripe = 255;
			}
			else
			{
				green_stripe = static_cast<uint_fast64_t>(round(Zr2 / Zi2));
			}
			green_stripe += blue_stripe;

			/*if(red > 255) red = red % 255;
			if(green > 255) green = green % 255;
			if(blue > 255) blue = blue % 255;
			if(green_stripe > 255) green_stripe = green_stripe % 255;
			if(blue_stripe > 255) blue_stripe = blue_stripe % 255;*/

			red *= color_opt.multiplier;
			green *= color_opt.multiplier;
			blue *= color_opt.multiplier;

			if(red > 255) red = 255;
			if(green > 255) green = 255;
			if(blue > 255) blue = 255;
			if(green_stripe > 255) green_stripe = 255;
			if(blue_stripe > 255) blue_stripe = 255;

			red -= (blue_stripe > red ? red : blue_stripe);
			red -= (green_stripe > red ? red : green_stripe);
			green -= (blue_stripe > green ? green : blue_stripe);
			green -= (green_stripe > green ? green : green_stripe);
			blue -= (blue_stripe > blue ? blue : blue_stripe);
			blue -= (green_stripe > blue ? blue : green_stripe);

			uint_fast64_t sub = red_fractal + green_fractal + blue_fractal;
			red -= (sub > red ? red : sub);
			green_stripe -= (sub > green_stripe ? green_stripe : sub);
			blue_stripe -= (sub > blue_stripe ? blue_stripe : sub);

			red += red_fractal;
			green += green_stripe + green_fractal;
			blue += blue_stripe + blue_fractal;
			break;
		}
		case 10:
		{
			red = (n << 1) ^ n;
			green = (n);
			blue = (n >> 1) ^ n;
			break;
		}
		case 11:
		{
			red = Zr2;
			green = Zr2 * Zi2;
			blue = Zi2;
			break;
		}
		case 12: // binary
		{
			red = green = blue = 255;
			break;
		}
		case 13: // purple (escape time)
		{
			red = (n << 2) + 5;
			green = (n << 1) + 1;
			blue = (n << 2) + 2;
			break;
		}
		case 14: // random; todo
		{
			srand(n);
			red = rand() & 0xFF;
			green = rand() & 0xFF;
			blue = rand() & 0xFF;
			break;
		}
		case 15: // hue
		{
			uint_fast8_t colors[3];
			HSV2RGB((n % 32) / 32.0, 1, 1, colors);
			red = colors[0];
			green = colors[1];
			blue = colors[2];
			break;
		}
		case 16:
		{
			red = n * n * 0.1;
			green = n;
			blue = Zr2 * Zi2;
			break;
		}
		case 17:
		{
			double r = 2 * sin(Zr2);
			double g = 2 * cos(Zi2);
			double b = r * g;
			red = r * 127;
			green = g * 127;
			blue = b * 127;
			break;
		}
		default:
		{
			throw std::runtime_error("Invalid color method: " + std::to_string(color_method));
		}
	}

	if(color_method != 9)
	{
		if(color_opt.multiplier > 1)
		{
			uint_fast64_t max = UINT_FAST64_MAX / color_opt.multiplier; // prevent overflow
			red = red > max ? UINT_FAST64_MAX : red * color_opt.multiplier;
			green = green > max ? UINT_FAST64_MAX : green * color_opt.multiplier;
			blue = blue > max ? UINT_FAST64_MAX : blue * color_opt.multiplier;
		}
		else
		{
			red *= color_opt.multiplier;
			green *= color_opt.multiplier;
			blue *= color_opt.multiplier;
		}
	}

	if(red > 255)
	{
		red = 255;
	}
	if(green > 255)
	{
		green = 255;
	}
	if(blue > 255)
	{
		blue = 255;
	}

	return png::rgb_pixel(red, green, blue);
}

kompleks iterate(kompleks Z, kompleks& c, uint_fast64_t n)
{
	switch(fractal_opt.type)
	{
		case mandelbrot:
		case julia:
		{
			return (Z^fractal_opt.exponent) + c;
		}
		case burning_ship:
		{
			kompleks_type real_abs = abs(Z.real);
			kompleks_type imag_abs = abs(Z.imag);
			return (kompleks(real_abs, imag_abs)^fractal_opt.exponent) + c;
		}
		case tricorn:
		{
			// this formula shows it flipped horizontally
			//return (Z.swap_xy()^fractal_opt.exponent) + c;

			// this is the formula given on Wikipedia
			return (Z.conjugate()^fractal_opt.exponent) + c;
		}
		case neuron:
		{
			// original flipped formula; higher exponents are rotated slightly
			return (Z.swap_xy()^fractal_opt.exponent) + Z;

			// this formula matches the tricorn; use this to get unrotated images
			//return (Z.conjugate()^fractal_opt.exponent) + Z;
		}
		case clouds:
		case oops:
		{
			kompleks new_z = (Z.swap_xy()^fractal_opt.exponent) + c;
			c = Z;
			return new_z;
		}
		case stupidbrot:
		{
			Z = (Z^fractal_opt.exponent);
			if(n % 2 == 0)
			{
				Z = Z + c;
			}
			else
			{
				Z = Z - c;
			}
			return Z;
		}
		case untitled1:
		{
			std::complex<kompleks_type> Z_std = pow(Z.to_std(), Z.to_std());
			return kompleks(Z_std) + Z;
		}
		case dots:
		{
			return (Z^fractal_opt.exponent) * c.reciprocal(); // equivalent to & faster than: (Z^fractal_opt.exponent) / c
		}
		case magnet1:
		{
			return (((Z^2) + (c - 1)) / (Z * 2 + (c - 2))) ^ 2;
		}
		case experiment:
		{
			//return lepow(c, fractal_opt.exponent) + Z;

			// diagonal line
			//return Z.swap_xy() + c;

			//return (Z^(fractal_opt.exponent + 1)) + (Z^fractal_opt.exponent) + c;
			return (Z^fractal_opt.exponent) + c.reciprocal();
		}
		case mandelbox:
		{
			auto boxfold = [](kompleks_type component)
			{
				if(component > 1)
				{
					return 2 - component;
				}
				if(component < -1)
				{
					return -2 - component;
				}
				return component;
			};
			Z.real = boxfold(Z.real);
			Z.imag = boxfold(Z.imag);

			if(Z.abs() < 0.5)
			{
				Z = Z / (0.5*0.5);
			}
			else if(Z.abs() < 1)
			{
				Z = Z / Z.norm();
			}

			return fractal_opt.exponent * Z + c;
		}
		case negamandelbrot:
		{
			return (Z^(1 / fractal_opt.exponent)) - c;
		}
		default:
		{
			// TODO: throw an exception
			return Z;
		}
	}
}

bool can_skip(kompleks_type x, kompleks_type y)
{
	if(fractal_opt.exponent == 2)
	{
		kompleks_type y2 = y*y;
		kompleks_type q = (x - 0.25)*(x - 0.25) + y2;
		return (q * (q + (x - 0.25)) < 0.25 * y2 // cardioid
			|| (x+1)*(x+1) + y2 < 0.0625);		 // p2 bulb

		return false;
	}

	/*
	See: http://cosinekitty.com/mandel_orbits_analysis.html
	It has:
		c = z - z^2
		(∂/∂z) (z^2 + c) = e^(i*θ)
		2z = e^(i*θ)
		z = (e^(i*θ)) / 2
		c = ((e^(i*θ)) / 2) - ((e^(i*θ)) / 2)^2

	If the exponent is 3:
		z^3 + c = z
		c = z - z^3
		(∂/∂z) (z^3 + c) = e^(i*θ)
		3*z^2 = e^(i*θ)
	I used Mathematica to solve for c and separate its components. As a parametric equation:
		x(t) = (3*cos(t/2) - cos(3*t/2)) / (3*sqrt(3))
		y(t) = ±((4*sin(t/2)^3) / (3*sqrt(3)))
	For some y value, I want the corresponding x value, so I solved for t and got inverse y:
		t(y) = 2*arcsin(cuberoot(3*sqrt(3)/4 * y))
	Then I used Mathematica to help simplify:
		x(t(y)) = ±(sqrt(4/3 - a) * (3a + 2))/6
		where a = cuberoot(2*y)^2
	Then I squared it and simplified
	*/
	if(fractal_opt.exponent == 3)
	{
		/* ellipse method that gets some (not all!) points
		const kompleks_type a = 0.384900179459750509673; // x(0)
		const kompleks_type b = 0.769800358919501019346; // y(tau/2)
		return (x*x)/(a*a) + (y*y)/(b*b) < 1;*/

		/* I was tired when I did this
		kompleks_type a = pow(2 * y, 1.0 / 3.0); a *= a;
		kompleks_type b = sqrt(4.0 / 3.0 - a) * (3*a + 2) / 6.0;
		return x < b && x > -b;*/

		kompleks_type y2 = y*y;
		if(x*x < 4.0/27.0 - y2 + pow(4 * y2, 1.0 / 3.0)/3.0)
		{
			return true;
		}
	}

	/*
	If the exponent is 4:
		z^4 + c = z
		c = z - z^4
		(∂/∂z) (z^4 + c) = e^(i*θ)
		4*z^3 = e^(i*θ)
	I used Mathematica to solve for c and separate its components. As a parametric equation:
		x(t) = (3*cos(t/2) - cos(3*t/2)) / (3*sqrt(3))
		y(t) = ±((4*sin(t/2)^3) / (3*sqrt(3)))
	For some y value, I want the corresponding x value, so I solved for t and got inverse y:
		t(y) = 2*arcsin(cuberoot(3*sqrt(3)/4 * y))
	Then I used Mathematica to help simplify:
		x(t(y)) = ±(sqrt(4/3 - a) * (3a + 2))/6
		where a = cuberoot(2*y)^2
	*/
	if(fractal_opt.exponent == 4)
	{
		//
	}

	return false;
}

uint_fast32_t width_px = 512;
uint_fast32_t height_px = 512;
uint_fast64_t max_iterations = 1024;
uint_fast32_t pCheck = 1; // periodicity checking

bool cancel = false;
void createFractal()
{
	double width = (fractal_opt.rbound - fractal_opt.lbound);
	double height = (fractal_opt.ubound - fractal_opt.bbound);
	double xinterval = width / width_px;
	double yinterval = height / height_px;

	uint_fast32_t totalPoints = width_px * height_px;
	uint_fast32_t currentPoint = 0;

	uint_fast64_t periodic = 0; // amount of periodic points
	uint_fast64_t escaped = 0; // amount of escaped points
	uint_fast64_t not_escaped = 0; // amount of points that did not escape
	uint_fast64_t skipped = 0;
	uint_fast64_t run = 0; // amount of iterations processed
	uint_fast64_t max_n = 0; // maximum iterations used on a point that escaped
	kompleks pCheckArray[pCheck];

	kompleks c;
	std::stringstream ss;
	ss << "Rendering " << type_strings[fractal_opt.type] << "...";
	std::string startString = ss.str();
	std::cout << startString << std::flush;
	uint_fast32_t spaces = 0;

	time_t statusT1, statusT2;
	time(&statusT1);
	time(&statusT2);

	timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 0;
	clock_settime(CLOCK_PROCESS_CPUTIME_ID, &ts);

	png::image<png::rgb_pixel> image(width_px, height_px);
	for(uint_fast32_t pY = 0; pY < height_px; ++pY)
	{
		for(uint_fast32_t pX = 0; pX < width_px; ++pX)
		{
			if(difftime(statusT2, statusT1) >= 1)
			{
				ss.str("");
				ss.clear();
				ss << startString << " point " << currentPoint << " of " << totalPoints;;
				std::string status = ss.str();
				spaces = status.length();
				clearLine(spaces);
				std::cout << "\r" << status << std::flush;
				time(&statusT1);
			}
			else
			{
				time(&statusT2);
			}

			kompleks_type x = fractal_opt.lbound + pX * xinterval + xinterval / 2;
			kompleks_type y = fractal_opt.ubound - pY * yinterval - yinterval / 2;

			if(!fractal_opt.single && fractal_opt.type == mandelbrot && fractal_opt.escape_limit == 4 && can_skip(x, y))
			{
				++skipped;
				//image.set_pixel(pX, pY, png::rgb_pixel(0, 255, 0));
			}
			else
			{
				kompleks Z;
				if(fractal_opt.type != clouds // clouds must start at 0; forgetting this resulted in oops
				&& fractal_opt.type != mandelbrot)
				{
					Z.real = x;
					Z.imag = y;
				}

				if(fractal_opt.type == julia)
				{
					c = kompleks(fractal_opt.juliaA, fractal_opt.juliaB);
				}
				else
				{
					c = kompleks(x, y);
				}

				std::fill_n(pCheckArray, pCheck, Z);

				for(uint_fast64_t n = 0; n <= max_iterations; ++n)
				{
					++run;
					if((fractal_opt.single && n == max_iterations)
					|| (!fractal_opt.single && Z.norm() > fractal_opt.escape_limit && n > 0))
					{
						++escaped;
						if(n > max_n)
						{
							max_n = n;
						}
						image.set_pixel(pX, pY, colorize(color_opt.method, Z, c, n));
						break;
					}
					if(n == max_iterations)
					{
						++not_escaped;
						//image.set_pixel(pX, pY, png::rgb_pixel(255, 0, 0));
						break;
					}

					Z = iterate(Z, c, n);

					if(!fractal_opt.single && pCheck > 0)
					{
						// if Z has had its current value in a previous iteration, stop iterating
						if(std::find(pCheckArray, pCheckArray + pCheck, Z) != pCheckArray + pCheck)
						{
							++periodic;
							/*if(fractal_opt.type == neuron && (color_opt.method == 0 || color_opt.method == 1 || color_opt.method == 9))
							{
								image.set_pixel(pX, pY, png::rgb_pixel(255, 255, 255));
							}*/
							//image.set_pixel(pX, pY, png::rgb_pixel(255, 255, 255));
							//image.set_pixel(pX, pY, colorize(color_opt.method, Z, c, UINT64_MAX));
							goto end_iteration; // double break
						}
						// shift array
						if(pCheck > 1)
						{
							for(uint_fast32_t p = 0; p < pCheck - 1; ++p)
							{
								pCheckArray[p] = pCheckArray[p + 1];
							}
						}
						pCheckArray[pCheck - 1] = Z;
					}
					if(cancel) // pressed CTRL+C
					{
						break;
					}
				}
				end_iteration:;
			}
			if(cancel) // pressed CTRL+C
			{
				break;
			}
			++currentPoint;
		}
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);

	clearLine(spaces);

	std::cout << "\r" << startString << " saving..." << std::flush;

	// make filename
	ss.clear();
	ss.str("");
	ss << "tiles/" << type_strings[fractal_opt.type] << "/" << color_opt.method << "/";

	if(fractal_opt.single)
	{
		ss << "single_";
	}
	ss << "e" << fractal_opt.exponent;

	if(fractal_opt.lbound != -2)
	{
		ss << "_lb" << fractal_opt.lbound;
	}
	if(fractal_opt.rbound != 2)
	{
		ss << "_rb" << fractal_opt.rbound;
	}
	if(fractal_opt.bbound != -2)
	{
		ss << "_bb" << fractal_opt.bbound;
	}
	if(fractal_opt.ubound != 2)
	{
		ss << "_ub" << fractal_opt.ubound;
	}

	if(fractal_opt.type == julia)
	{
		ss << "_jx" << fractal_opt.juliaA << "_jy" << fractal_opt.juliaB;
	}
	if(color_opt.method == 1 && color_opt.disable_fancy)
	{
		ss << "_df";
	}

	if(!fractal_opt.single)
	{
		ss << "_el" << fractal_opt.escape_limit;
	}
	ss << "_mi" << (fractal_opt.single ? max_iterations : max_n);

	if((color_opt.method == 0 || color_opt.method == 1) && color_opt.smooth)
	{
		ss << "_smooth";
	}
	ss << "_" << width_px << "x";
	if(width_px != height_px)
	{
		ss << height_px;
	}
	if(color_opt.multiplier != 1)
	{
		ss << "_cm" << color_opt.multiplier;
	}
	if(cancel)
	{
		ss << "_partial";
	}
	else if(fractal_opt.type == mandelbrot && not_escaped == 0 && !fractal_opt.single)
	{
		ss << "_0ne";
	}
	ss << ".png";

	image.write(ss.str());

	double seconds = ts.tv_sec + ts.tv_nsec / 1e9;

	std::cout << " done in " << seconds << " second" << (seconds != 1 ? "s":"");
	std::cout << " (" << escaped << " e, " << not_escaped << " ne, " << periodic << " p, " << skipped << " s, " << run << " i, " << max_n << " mi)\n";

	if(!cancel)
	{
		if(escaped + not_escaped + periodic + skipped != totalPoints)
		{
			std::cout << "There is a bug somewhere (e + ne + p + s != total)\n";
		}
	}
}

void create_directory(const std::string& dirname)
{
	if(mkdir(dirname.c_str(), 0777) != 0)
	{
		if(errno != EEXIST)
		{
			throw std::runtime_error(strerror(errno));
		}
	}
}

void show_help()
{
	std::cout << "[s] means string, [d] means double, and [i] means integer. Options that take a value will fail without one.\n";
	std::cout << " -s             Smooth the color bands for methods 0 and 1\n";
	std::cout << " -S             Color all points with the specified iteration count\n";
	std::cout << "                 instead of the escape time\n";
	std::cout << " -t         [s] Fractal type:\n";
	std::cout << "                 mandelbrot\n";
	std::cout << "                 julia\n";
	std::cout << "                 burning_ship\n";
	std::cout << "                 tricorn\n";
	std::cout << "                 neuron\n";
	std::cout << "                 stupidbrot\n";
	std::cout << "                 untitled1\n";
	std::cout << "                 dots\n";
	std::cout << " -jx        [d] The real part of c (for julia only)\n";
	std::cout << " -jy        [d] The imaginary part of c (for julia only)\n";
	std::cout << " -c         [i] The coloring method to use (default = 0):\n";
	std::cout << " -colors        List coloring methods\n";
	std::cout << " -df            Disable fancy coloring for method 1\n";
	std::cout << " -cm        [d] Color multiplier";
	std::cout << " -r         [i] Picture size (width and height)\n";
	std::cout << " -i         [i] Maximum iterations for each point\n";
	std::cout << " -e         [d] Exponent (default = 2); higher absolute value = slower\n";
	std::cout << " -el        [d] Escape limit (default = 4)\n";
	std::cout << "\n";
	std::cout << "If an invalid value is specified, the default will be used. For the filters, the value you specify is how many iterations are run before the filter starts checking points.\n";
}

void show_colors()
{
	std::cout << "Coloring methods:\n";
	std::cout << "          0 - gold (escape time)\n";
	std::cout << "          1 - green (escape time) with red/blue crap\n";
	std::cout << "          2 - green/orange crap with blue laser things\n";
	std::cout << "          3 - red/blue crap with green laser thingies\n";
	std::cout << "          4 - weird white and black crap\n";
	std::cout << "          5 - glowing (green)\n";
	std::cout << "          6 - glowing (pink)\n";
	std::cout << "          7 - glowing (blue)\n";
	std::cout << "          8 - pinkish XOR (might need -cm)\n";
	std::cout << "          9 - weird XOR stuff with lots of stripes\n";
	std::cout << "         10 - ugly pink thing\n";
	std::cout << "         11 - ugly green thing\n";
	std::cout << "         12 - black (set) and white (background)\n";
	std::cout << "         13 - purple (escape time)\n";
	std::cout << "         14 - random (escape time)\n";
	std::cout << "         15 - hue (escape time)\n";
	std::cout << "         16 - oversaturated orange/yellow (escape time) with blue crap\n";
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		show_help();
		return 0;
	}
	std::string firstArg(argv[1]);
	if(firstArg == "--help" || firstArg == "-h" || firstArg == "-?")
	{
		show_help();
		return 0;
	}
	if(firstArg == "-colors")
	{
		show_colors();
		return 0;
	}

	ArgParser argp;
	argp.add("-df", false);
	argp.add("-s", false);
	argp.add("-S", false);

	argp.add("-c", 0);
	argp.add("-cm", 1.0);
	argp.add("-e", 2.0);
	argp.add("-el", 4.0);
	argp.add("-i", 1024);
	argp.add("-jx", -0.8);
	argp.add("-jy", 0.156);
	argp.add("-pc", 1);
	argp.add("-r", 1024);
	argp.add("-t", "mandelbrot");
	argp.add("-lbound", -2.0);
	argp.add("-rbound", 2.0);
	argp.add("-bbound", -2.0);
	argp.add("-ubound", 2.0);
	argp.add("-box", 2.0);
	argp.add("-wm", 1.0); // width multiplier

	try
	{
		argp.parse(argc, argv);
	}
	catch(std::runtime_error& e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	color_opt.disable_fancy = argp.get_bool("-df");
	color_opt.smooth = argp.get_bool("-s");
	fractal_opt.single = argp.get_bool("-S");

	color_opt.method = argp.get_int("-c");

	color_opt.multiplier		= argp.get_double("-cm");
	fractal_opt.exponent		= argp.get_double("-e");
	fractal_opt.escape_limit	= argp.get_double("-el");
	max_iterations				= argp.get_int("-i");
	fractal_opt.juliaA				= argp.get_double("-jx");
	fractal_opt.juliaB				= argp.get_double("-jy");
	pCheck						= argp.get_int("-pc");
	width_px					= height_px = argp.get_int("-r");
	width_px					= std::round(width_px * argp.get_double("-wm"));
	try
	{
		fractal_opt.type = string_to_fractal_type(argp.get_string("-t").c_str());
	}
	catch(std::runtime_error& e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	double lbound, rbound, bbound, ubound;
	if(argp.get_double("-box") != 2)
	{
		rbound = ubound = argp.get_double("-box");
		lbound = bbound = -rbound;
	}
	else
	{
		lbound = argp.get_double("-lbound");
		rbound = argp.get_double("-rbound");
		bbound = argp.get_double("-bbound");
		ubound = argp.get_double("-ubound");
	}
	fractal_opt.lbound = lbound;
	fractal_opt.rbound = rbound;
	fractal_opt.bbound = bbound;
	fractal_opt.ubound = ubound;

	// end arguments

	std::stringstream ss;

	ss << "tiles";
	create_directory("tiles");

	ss << "/" << type_strings[fractal_opt.type];
	create_directory(ss.str());

	ss << "/" << color_opt.method;
	create_directory(ss.str());

	// if Ctrl+C is pressed, stop iteration and save partial image
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = [](int s)
	{
		if(s == 2) // CTRL + C
		{
			cancel = true;
		}
	};
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	createFractal();

	return 0;
}