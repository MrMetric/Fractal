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

#include <stdint.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

inline void clearLine(uint_fast32_t spaces)
{
	std::cout << "\r";
	for(uint_fast32_t c = 0; c < spaces; ++c)
	{
		std::cout << " ";
	}
	std::cout << std::flush;
}

enum FractalType
{
	mandelbrot,
	julia,
	julia2,
	burning_ship,
	tricorn,
	neuron,
	clouds,
	oops,
	stupidbrot,
	untitled1,
	dots,
	magnet1,
	magnet2,
	experiment
};

FractalType type = mandelbrot;
std::string typeStrings[] =
{
	"Mandelbrot",
	"Julia",
	"Julia2",
	"Burning Ship",
	"Tricorn",
	"Neuron",
	"Clouds",
	"Oops",
	"Stupidbrot",
	"Untitled 1",
	"Dots",
	"Magnet 1",
	"Magnet 2",
	"Experiment"
};

FractalType string_to_fractal_type(const std::string& typestr)
{
	const char* typestrc = typestr.c_str();

	if(strcasecmp(typestrc, "mandelbrot") == 0)
	{
		return mandelbrot;
	}
	if(strcasecmp(typestrc, "julia") == 0)
	{
		return julia;
	}
	if(strcasecmp(typestrc, "julia2") == 0)
	{
		return julia2;
	}
	if(strcasecmp(typestrc, "burning_ship") == 0)
	{
		return burning_ship;
	}
	if(strcasecmp(typestrc, "tricorn") == 0)
	{
		return tricorn;
	}
	if(strcasecmp(typestrc, "neuron") == 0)
	{
		return neuron;
	}
	if(strcasecmp(typestrc, "clouds") == 0)
	{
		return clouds;
	}
	if(strcasecmp(typestrc, "oops") == 0)
	{
		return oops;
	}
	if(strcasecmp(typestrc, "stupidbrot") == 0)
	{
		return stupidbrot;
	}
	if(strcasecmp(typestrc, "untitled1") == 0)
	{
		return untitled1;
	}
	if(strcasecmp(typestrc, "dots") == 0)
	{
		return dots;
	}
	if(strcasecmp(typestrc, "magnet1") == 0)
	{
		return magnet1;
	}
	if(strcasecmp(typestrc, "experiment") == 0)
	{
		return experiment;
	}
	throw std::runtime_error("Unknown fractal type: " + typestr);
}

kompleks_type exponent = 2;
bool smooth = false;
bool disableFancy = false;
kompleks_type escapeLimit = 4;
kompleks_type colorMul = 1;

// https://github.com/kobalicek/rgbhsv/blob/master/src/rgbhsv.cpp
void HSV2RGB(kompleks_type h, kompleks_type s, kompleks_type v, uint_fast8_t dst[3])
{
	if(h >= 1)
	{
		h -= 1;
	}
	h *= 6;

	int index = static_cast<int>(h);
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

const png::rgb_pixel getColor(uint_fast32_t color_method, kompleks Z, kompleks c, uint_fast64_t n)
{
	uint_fast64_t red, green, blue;
	kompleks_type Zr2 = Z.real*Z.real;
	kompleks_type Zi2 = Z.imag*Z.imag;
	switch(color_method)
	{
		case 0: // escape time (gold)
		{
			if(smooth)
			{
				/*Z = iterate(Z, c);
				Z = iterate(Z, c);
				n += 2;
				kompleks_type newColor = (n - (log(log(Z.abs()))) / log(exponent));
				red = floor(newColor * 2.0 + 0.5);
				green = floor(newColor + 0.5);
				blue = floor(newColor / 2.0 + 0.5);*/

				// from http://www.hpdz.net/TechInfo/Colorizing.htm
				kompleks_type dx = (log(log(escapeLimit)) - log(log(Z.abs()))) / log(exponent);
				kompleks_type newColor = (n + 1 * dx);
				red = floor(newColor * 2.0 + 0.5);
				green = floor(newColor + 0.5);
				blue = floor(newColor / 2.0 + 0.5);
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
			if(!disableFancy)
			{
				red = Zr2;
				blue = Zi2;
			}
			else
			{
				red = 0;
				blue = 0;
			}
			if(smooth)
			{
				/*Z = iterate(Z, c);
				Z = iterate(Z, c);
				n += 2;
				green = floor((n - (log(log(Z.abs()))) / log((kompleks_type)exponent)) + 0.5);*/

				kompleks_type dx = (log(log(escapeLimit)) - log(log(Z.abs()))) / log(exponent);
				green = floor((n + 1 * dx) + 0.5);
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
			//red = (Zr2 * Zr2 * Zr2 + 1) / Zi2;
			//green = ((Zi2 + 1) / Zr2) - (Zi2 / (Zr2 + 1));
			//blue = ((Zr2 + 1) / Zi2);// - (Zr2 / (Zi2 + 1));

			if(Zr2 <= 0.00392)
			{
				red = 255;
			}
			else
			{
				red = floor(1 / Zr2 + 0.5);
			}
			if(Zr2 <= 0.00588)
			{
				green = 255;
			}
			else
			{
				green = floor(1.5 / Zr2 + 0.5);
			}
			if(Zr2 <= 0.00294)
			{
				blue = 255;
			}
			else
			{
				blue = floor(0.75 / Zr2 + 0.5);
			}
			break;
		}
		case 6: // Glow (Pink)
		{
			if(Zr2 <= 0.00588)
			{
				red = 255;
			}
			else
			{
				red = floor(1.5 / Zr2 + 0.5);
			}
			if(Zr2 <= 0.00294)
			{
				green = 255;
			}
			else
			{
				green = floor(0.75 / Zr2 + 0.5);
			}
			if(Zr2 <= 0.00392)
			{
				blue = 255;
			}
			else
			{
				blue = floor(1 / Zr2 + 0.5);
			}
			break;
		}
		case 7: // Glow (Blue)
		{
			if(Zr2 <= 0.00294)
			{
				red = 255;
			}
			else
			{
				red = floor(0.75 / Zr2 + 0.5);
			}
			if(Zr2 <= 0.00392)
			{
				green = 255;
			}
			else
			{
				green = floor(1 / Zr2 + 0.5);
			}
			if(Zr2 <= 0.00588)
			{
				blue = 255;
			}
			else
			{
				blue = floor(1.5 / Zr2 + 0.5);
			}
			break;
		}
		case 8: // Bright pink with XOR
		{
			if(Zr2 == 0)
			{
				red = 255;
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
			blue = (uint_fast64_t)floor(Zi2 * 255 + 0.5) ^ (uint_fast64_t)floor(Zr2 * 255 + 0.5);
			red += blue * 0.5;
			green += blue * 0.2;
			break;
		}
		case 9:
		{
			png::rgb_pixel color_fractal = getColor(0, Z, c, n);
			uint_fast64_t red_fractal = color_fractal.red,
						  green_fractal = color_fractal.green,
						  blue_fractal = color_fractal.blue;

			red = (uint_fast64_t)floor(Zr2*8 + 0.5) ^ (uint_fast64_t)floor(Zi2*8 + 0.5);
			green = (uint_fast64_t)floor(Zr2*2 + 0.5) ^ (uint_fast64_t)floor(Zi2*2 + 0.5);
			blue = (uint_fast64_t)floor(Zr2*4 + 0.5) ^ (uint_fast64_t)floor(Zi2*4 + 0.5);

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
				blue_stripe = (uint_fast64_t)floor(Zi2 / Zr2 + 0.5);
			}
			uint_fast64_t green_stripe;
			if(Zi2 == 0)
			{
				green_stripe = 255;
			}
			else
			{
				green_stripe = (uint_fast64_t)floor(Zr2 / Zi2 + 0.5);
			}
			green_stripe += blue_stripe;

			/*if(red > 255) red = red % 255;
			if(green > 255) green = green % 255;
			if(blue > 255) blue = blue % 255;
			if(green_stripe > 255) green_stripe = green_stripe % 255;
			if(blue_stripe > 255) blue_stripe = blue_stripe % 255;*/

			red *= colorMul;
			green *= colorMul;
			blue *= colorMul;

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
			//red = floor(abs(Z.imag - Z.real) * 8 + 0.5);
			//green = floor(abs(lenorm(Z)) * 4 + 0.5);

			//kompleks_type Zr = Z.real;
			//kompleks_type Zi = Z.imag;
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
		case 13: // purple
		{
			/*
1:  8,  3,  7
2: 13,  5, 11
3: 17,  7, 14
4: 22,  9, 18
5: 26, 10, 22
			*/

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
		default:
		{
			red = n;
			green = n;
			blue = n;
		}
	}

	if(colorMul != 1 && color_method != 9)
	{
		red *= colorMul;
		green *= colorMul;
		blue *= colorMul;
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

	//red %= 255;
	//green %= 255;
	//blue %= 255;

	return png::rgb_pixel(red, green, blue);
}

kompleks iterate(kompleks Z, kompleks& c, uint_fast64_t n)
{
	if(type == mandelbrot || type == julia)
	{
		return (Z^exponent) + c;
	}
	if(type == julia2)
	{
		return sqrt(std_sinh(Z^exponent).to_std()) + c;
	}
	if(type == burning_ship)
	{
		kompleks_type real_abs = abs(Z.real);
		kompleks_type imag_abs = abs(Z.imag);
		return (kompleks(real_abs, imag_abs)^exponent) + c;
	}
	if(type == tricorn)
	{
		// this formula shows it flipped horizontally
		//return (Z.swap_xy()^exponent) + c;

		// this formula is the one given on Wikipedia
		return (Z.conjugate()^exponent) + c;
	}
	if(type == neuron)
	{
		// original flipped formula; higher exponents are rotated slightly
		return (Z.swap_xy()^exponent) + Z;

		// this formula matches the tricorn; use this to get unrotated images
		//return (Z.conjugate()^exponent) + Z;
	}
	if(type == clouds || type == oops)
	{
		kompleks new_z = (Z.swap_xy()^exponent) + c;
		c = Z;
		return new_z;
	}
	if(type == stupidbrot)
	{
		Z = (Z^exponent);
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
	if(type == untitled1)
	{
		std::complex<kompleks_type> Z_std = pow(Z.to_std(), Z.to_std());
		return kompleks(Z_std) + Z;
	}
	if(type == dots)
	{
		return (Z^exponent) / c;
	}
	if(type == magnet1)
	{
		return (((Z^2) + (c - 1)) / (Z * 2 + (c - 2))) ^ 2;
	}
	if(type == experiment)
	{
		//return lepow(c, exponent) + Z;

		// diagonal line
		//return kompleks(Z.imag, Z.real) + c;

		//return (Z^(exponent + 1)) + (Z^exponent) + c;
		return (Z^exponent) + c.reciprocal();
	}
	// TODO: throw an exception
	return Z;
}

uint_fast32_t width_px = 512;
uint_fast32_t height_px = 512;
uint_fast64_t max_iterations = 1024;
bool single = false;
uint_fast32_t pCheck = 1; // periodicity checking

bool cancel = false;
void createFractal(
	kompleks_type lbound, kompleks_type rbound, kompleks_type bbound, kompleks_type ubound,
	uint_fast32_t color_method, kompleks_type juliaA, kompleks_type juliaB)
{
	double width = (rbound - lbound);
	double height = (ubound - bbound);
	double xinterval = width / width_px;
	double yinterval = height / height_px;

	uint_fast32_t totalPoints = width_px * height_px;
	uint_fast32_t currentPoint = 0;

	uint_fast64_t periodic = 0; // amount of periodic points
	uint_fast64_t escaped = 0; // amount of escaped points
	uint_fast64_t notEscaped = 0; // amount of points that did not escape
	uint_fast64_t skipped = 0;
	uint_fast64_t run = 0; // amount of iterations processed
	uint_fast64_t max_n = 0; // maximum iterations used on a point that escaped

	kompleks c;
	std::stringstream ss;
	ss << "Rendering " << typeStrings[type] << "...";
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

			kompleks_type x = lbound + pX * xinterval + xinterval / 2;
			kompleks_type y = -(bbound + pY * yinterval + yinterval / 2);
			kompleks_type y2 = y*y;

			kompleks_type q = (x - 0.25)*(x - 0.25) + y2;
			if(!single && type == mandelbrot && exponent == 2 && escapeLimit >=4 && q * (q + (x - 0.25)) < 0.25 * y2) // cardioid
			{
				++skipped;
			}
			else if(!single && type == mandelbrot && exponent == 2 && escapeLimit >= 4 && (x+1)*(x+1) + y2 < 0.0625) // bulb
			{
				++skipped;
			}
			else
			{
				kompleks Z;
				if(type != clouds) // clouds must start at 0; forgetting this resulted in oops
				{
					Z.real = x;
					Z.imag = y;
				}
				if(type == julia || type == julia2)
				{
					c = kompleks(juliaA, juliaB);
				}
				else
				{
					c = kompleks(x, y);
				}

				// TODO: Finish implementing this
				kompleks pCheckArray[pCheck];
				for(uint_fast32_t p = 0; p < pCheck; ++p)
				{
					pCheckArray[p] = Z;
				}

				for(uint_fast64_t n = 0; n <= max_iterations; ++n)
				{
					++run;
					if((single && n == max_iterations) || (!single && Z.norm() > escapeLimit && n > 0))
					{
						++escaped;
						if(n > max_n)
						{
							max_n = n;
						}
						image.set_pixel(pX, pY, getColor(color_method, Z, c, n));
						break;
					}
					if(n == max_iterations)
					{
						++notEscaped;
						break;
					}
					Z = iterate(Z, c, n);

					if(!single && pCheck > 0)
					{
						bool exitLoop = false;
						for(uint_fast32_t p = 0; p < pCheck; ++p)
						{
							if(pCheckArray[p] == Z)
							{
								++periodic;
								if(type == neuron && (color_method == 0 || color_method == 1 || color_method == 9))
								{
									image.set_pixel(pX, pY, png::rgb_pixel(255, 255, 255));
								}
								exitLoop = true;
								break;
							}
						}
						if(exitLoop)
						{
							break;
						}
						//pn = Z;
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

	// make filename
	ss.clear();
	ss.str("");
	ss << "tiles/" << typeStrings[type] << "/" << color_method << "/";

	if(single)
	{
		ss << "single_";
	}
	ss << "e" << exponent;

	if(lbound != -2)
	{
		ss << "_lb" << lbound;
	}
	if(rbound != 2)
	{
		ss << "_rb" << rbound;
	}
	if(bbound != -2)
	{
		ss << "_bb" << bbound;
	}
	if(ubound != 2)
	{
		ss << "_ub" << ubound;
	}

	if(type == julia || type == julia2)
	{
		ss << "_jx" << juliaA << "_jy" << juliaB;
	}
	if(color_method == 1 && disableFancy)
	{
		ss << "_df";
	}

	if(!single)
	{
		ss << "_el" << escapeLimit;
	}
	ss << "_mi" << (single ? max_iterations : max_n);

	if((color_method == 0 || color_method == 1) && smooth)
	{
		ss << "_smooth";
	}
	ss << "_" << width_px << "x";
	if(width_px != height_px)
	{
		ss << height_px;
	}
	ss << "_cm" << colorMul;
	if(cancel)
	{
		ss << "_partial";
	}
	ss << ".png";
	std::string imgPath = ss.str();

	// save stuff
	std::cout << "\r" << startString << " saving..." << std::flush;
	image.write(imgPath);

	double seconds = ts.tv_sec + ts.tv_nsec / 1e9;

	std::cout << " done in " << seconds << " second" << (seconds != 1 ? "s":"");
	std::cout << " (" << escaped << " e, " << notEscaped << " ne, " << periodic << " p, " << skipped << " s, " << run << " i, " << max_n << " mi)\n";

	escaped = 0;
	notEscaped = 0;
	periodic = 0;
	skipped = 0;
	run = 0;
	max_n = 0;
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
	std::cout << " -c         [i] The coloring method to use. Available values:\n";
	std::cout << "                 0 - gold (escape time)\n";
	std::cout << "                 1 - green (escape time) with red/blue crap\n";
	std::cout << "                 2 - green/orange crap with blue laser things\n";
	std::cout << "                 3 - red/blue crap with green laser thingies\n";
	std::cout << "                 4 - weird white and black crap\n";
	std::cout << "                 5 - glowing (green)\n";
	std::cout << "                 6 - glowing (pink)\n";
	std::cout << "                 7 - glowing (blue)\n";
	std::cout << "                 8 - pinkish XOR\n";
	std::cout << " -df            Disable fancy coloring for method 1\n";
	std::cout << " -cm        [d] Color multiplier";
	std::cout << " -r         [i] Picture size (width and height)\n";
	std::cout << " -i         [i] Maximum iterations for each point\n";
	std::cout << " -e         [d] Exponent (default = 2); higher absolute value = slower\n";
	std::cout << " -el        [d] Escape limit (default = 4)\n";
	std::cout << "\n";
	std::cout << "If an invalid value is specified, the default will be used. For the filters, the value you specify is how many iterations are run before the filter starts checking points.\n";
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

	ArgParser argp;
	argp.add("-df", false);
	argp.add("-s", false);
	argp.add("-S", false);

	argp.add("-c", 0);
	argp.add("-cm", 1.0);
	argp.add("-e", 2.0);
	argp.add("-el", 4.0);
	argp.add("-i", 1024);
	argp.add("-ja", -0.8);
	argp.add("-jb", 0.156);
	argp.add("-pc", 1);
	argp.add("-r", 1024);
	argp.add("-t", "mandelbrot");
	argp.add("-lbound", -2.0);
	argp.add("-rbound", 2.0);
	argp.add("-bbound", -2.0);
	argp.add("-ubound", 2.0);
	argp.add("-box", 2.0);

	try
	{
		argp.parse(argc, argv);
	}
	catch(std::runtime_error e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	disableFancy = argp.get_bool("-df");
	smooth = argp.get_bool("-s");
	single = argp.get_bool("-S");

	uint_fast32_t color_method = argp.get_int("-c");
	if(color_method > 16) color_method = 0;

	colorMul = argp.get_double("-cm");
	exponent = argp.get_double("-e");
	escapeLimit = argp.get_double("-el");
	max_iterations = argp.get_int("-i");
	double juliaA = argp.get_double("-ja");
	double juliaB = argp.get_double("-jb");
	pCheck = argp.get_int("-pc");
	width_px = height_px = argp.get_int("-r");
	try
	{
		type = string_to_fractal_type(argp.get_string("-t"));
	}
	catch(std::runtime_error e)
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

	// end arguments

	std::stringstream ss;

	ss << "tiles";
	create_directory("tiles");

	ss << "/" << typeStrings[type];
	create_directory(ss.str());

	ss << "/" << color_method;
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

	createFractal(lbound, rbound, bbound, ubound, color_method, juliaA, juliaB);

	return 0;
}