#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <cerrno>
#include <strings.h>
#include <signal.h>
#include <png++/png.hpp>
#define exprtk_lean_and_mean
//#include <exprtk.hpp>

//#define USE_GMP

#ifdef USE_GMP
#include <gmp.h>
#endif

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

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679L

#define TILE 512

#define MODE 0777
#define ERR_ARGS  1 // invalid argument(s)
#define ERR_DIR0  2 // tiles
#define ERR_DIR1  3 // tiles/TYPE
#define ERR_DIR2  4 // tiles/TYPE/COLOR
#define ERR_DIR3  5 // tiles/TYPE/COLOR/TILE
#define ERR_DIR4  6 // tiles/TYPE/COLOR/TILE/EXPONENT
#define ERR_DIR5  7 // JA_JB
#define ERR_DIR6  8 // tiles/TYPE/COLOR/TILE/EXPONENT/RES
#define ERR_DIR7  9 // tiles/TYPE/COLOR/TILE/EXPONENT/RES/ITERATIONS
#define ERR_DIR8 10 // tiles/TYPE/COLOR/TILE/EQUATION
#define ERR_TOOB 11 // tile out of bounds
#define ERR_NOEQ 12 // no equation given
#define ERR_EQTN 13 // error parsing equation

enum FractalType
{
	mandelbrot,
	julia,
	burning_ship,
	tricorn,
	neuron,
	stupidbrot,
	untitled1,
	dots,
	magnet1,
	magnet2,
	experiment
};

FractalType type = mandelbrot;
std::string typeStrings[11] =
{
	"Mandelbrot",
	"Julia",
	"Burning Ship",
	"Tricorn",
	"Neuron",
	"Stupidbrot",
	"Untitled 1",
	"Dots",
	"Magnet 1",
	"Magnet 2",
	"Experiment"
};

enum FractalPlane
{
	mu,
	lambda
};

FractalPlane plane = mu;
std::string planeStrings[2] =
{
	"mu",
	"lambda"
};

long double juliaA = -0.8L;
long double juliaB = 0.156L;
uint_fast32_t colorMethod = 0;
uint_fast32_t fractalRes = 512;
uint_fast32_t iterations = 1024;
uint_fast32_t tileX = 0;
uint_fast32_t tileY = 0;
uint_fast32_t exponent = 2;
long double exponentD = 2;
std::complex<long double> exponentC(exponentD, 0);
bool negative = false;
bool smooth = false;
bool filter1 = false;
uint_fast32_t filter1T = 512;
bool filter2 = false;
uint_fast32_t filter2T = 512;
bool disableFancy = false;
long double escapeLimit = 4;
bool single = false;
long double colorMul = 1;
bool wallpaper = false;
long double wallpaperScale = 1;
uint_fast32_t wallpaperW = 1600;
uint_fast32_t wallpaperH = 900;
uint_fast32_t pCheck = 1; // periodicity checking
long double scale = 4;

std::complex<long double> c_1(1, 0);
std::complex<long double> c_2(2, 0);

std::complex<long double> lepow(std::complex<long double> base, uint_fast32_t e)
{
	if(exponentD != 2)
	{
		return pow(base, exponentC);
	}

	if(e == 0)
	{
		return std::complex<long double>(1, 0);
	}
	else if(e == 1)
	{
		return base;
	}
	std::complex<long double> out = base;
	for(uint_fast32_t i = 1; i < e; ++i)
	{
		out *= base;
	}
	if(negative)
	{
		out = c_1 / out;
	}
	return out;
}

//long double Zr;
//long double Zi;
long double Zr2;
long double Zi2;
uint_fast32_t n;

std::string imgPath = "";
//std::string path2;

// modified from http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
void RGB2HSV(long double r, long double g, long double b, long double &h, long double &s, long double &v)
{
	long double K = 0.f;

	if(g < b)
	{
		std::swap(g, b);
		K = -1.f;
	}

	if(r < g)
	{
		std::swap(r, g);
		K = -2.f / 6.f - K;
	}

	long double chroma = r - std::min(g, b);
	h = abs(K + (g - b) / (6.f * chroma + 1e-20f));
	s = chroma / (r + 1e-20f);
	v = r;
}

#define HSVMODE 1
void SetHSV(long double h, long double s, long double v, uint_fast8_t color[3])
{
	long double r = 0, g = 0, b = 0;
	if(s == 0)
	{
		r = g = b = v;
	}
	else
	{
		#if(HSVMODE == 1)
		if(h == 1)
		{
			h = 0;
		}
		long double z = floor(h * 6);
		int_fast32_t i = int(z);
		double f = double(h * 6 - z);
		#elif(HSVMODE == 2)
		h /= 60;
		int i = floor(h);
		double f = h - i;
		#endif

		long double p = v * (1 - s);
		long double q = v * (1 - s * f);
		long double t = v * (1 - s * (1 - f));

		switch(i)
		{
			case 0:
				r = v;
				g = t;
				b = p;
				break;
			case 1:
				r = q;
				g = v;
				b = p;
				break;
			case 2:
				r = p;
				g = v;
				b = t;
				break;
			case 3:
				r = p;
				g = q;
				b = v;
				break;
			case 4:
				r = t;
				g = p;
				b = v;
				break;
			case 5:
				r = v;
				g = p;
				b = q;
				break;
		}
	}
	int_fast32_t c;
	c = int(256 * r);
	if(c > 255) c = 255;
	color[0] = c;
	c = int(256 * g);
	if(c > 255) c = 255;
	color[1] = c;
	c = int(256 * b);
	if(c > 255) c = 255;
	color[2] = c;
}

std::complex<long double>iterate(std::complex<long double> Z, std::complex<long double> c)
{
	if(type == mandelbrot || type == julia)
	{
		Z = lepow(Z, exponent) + c;
	}
	else if(type == burning_ship)
	{
		Z = lepow(std::complex<long double>(abs(real(Z)), abs(imag(Z))), exponent) + c;
	}
	else if(type == tricorn)
	{
		// this formula shows it flipped horizontally
		//Z = lepow(std::complex<long double>(imag(Z), real(Z)), exponent) + c;

		// this formula is the one given on Wikipedia
		Z = lepow(std::complex<long double>(real(Z), -imag(Z)), exponent) + c;
	}
	else if(type == neuron)
	{
		// original flipped formula; higher exponents are rotated slightly
		Z = lepow(std::complex<long double>(imag(Z), real(Z)), exponent) + Z;

		// this formula matches the tricorn; use this to get unrotated images
		//Z = lepow(std::complex<long double>(real(Z), -imag(Z)), exponent) + Z;
	}
	else if(type == stupidbrot)
	{
		Z = lepow(Z, exponent);
		if(n % 2 == 0)
		{
			Z = Z + c;
		}
		else
		{
			Z = Z - c;
		}
	}
	else if(type == untitled1)
	{
		Z = pow(Z, Z) + Z;
	}
	else if(type == dots)
	{
		Z = lepow(Z, exponent) / c;
	}
	else if(type == magnet1)
	{
		Z = lepow((lepow(Z, 2) + (c - c_1)) / (Z * c_2 + (c - c_2)), 2);
	}
	else if(type == experiment)
	{
		//Z = lepow(c, exponent) + Z;

		// diagonal line
		//Z = std::complex<long double>(imag(Z), real(Z)) + c;

		//Z = lepow(Z, exponent + 1) + lepow(Z, exponent) + c;
		Z = lepow(Z, exponent) + c_1/c;
	}
	return Z;
}

uint_fast64_t red, green, blue;
void getColor(std::complex<long double> c, std::complex<long double> Z)
{
	Zr2 = real(Z)*real(Z);
	Zi2 = imag(Z)*imag(Z);
	switch(colorMethod)
	{
		case 0: // escape time (gold)
		{
			if(smooth)
			{
				/*Z = iterate(Z, c);
				Z = iterate(Z, c);
				n += 2;
				long double newColor = (n - (log(log(abs(Z)))) / log(exponent));
				red = floor(newColor * 2.0 + 0.5);
				green = floor(newColor + 0.5);
				blue = floor(newColor / 2.0 + 0.5);*/

				// from http://www.hpdz.net/TechInfo/Colorizing.htm
				long double dx = (log(log(escapeLimit)) - log(log(abs(Z)))) / log(exponent);
				long double newColor = (n + 1 * dx);
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
				green = floor((n - (log(log(abs(Z)))) / log((long double)exponent)) + 0.5);*/

				long double dx = (log(log(escapeLimit)) - log(log(abs(Z)))) / log(exponent);
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
			red = green = blue = real(Z) * sin(imag(Z) + Zi2) - Zr2;
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
			uint_fast64_t red_fractal = n << 1,
						  green_fractal = n,
						  blue_fractal = n >> 1;
			if(red_fractal > 255) red_fractal = 255;
			if(green_fractal > 255) green_fractal = 255;
			if(blue_fractal > 255) blue_fractal = 255;

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
			//red = floor(abs(imag(Z) - real(Z)) * 8 + 0.5);
			//green = floor(abs(norm(Z)) * 4 + 0.5);

			//long double Zr = real(Z);
			//long double Zi = imag(Z);
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
			red = rand() % 255 + 1;
			green = rand() % 255 + 1;
			blue = rand() % 255 + 1;
			break;
		}
		case 15: // hue
		{
			uint_fast8_t colors[3];
			SetHSV((n % 32) / 32.0, 1, 1, colors);
			red = colors[0];
			green = colors[1];
			blue = colors[2];
			break;
		}
		default:
		{
			red = n;
			green = n;
			blue = n;
		}
	}

	if(colorMul != 1)
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
}

/*string equation;
template<typename T> bool createEquation()
{
	png::image<png::rgb_pixel> image(TILE, TILE);
	uint_fast32_t startY = tileY * TILE;
	uint_fast32_t pY = startY;
	uint_fast32_t pX = 0;

	T x = T(pX);
	T y = T(pY);

	exprtk::symbol_table<T> symbol_table;
	symbol_table.add_variable("x", x);
	symbol_table.add_variable("y", y);
	symbol_table.add_constants();

	exprtk::expression<T> expression;
	expression.register_symbol_table(symbol_table);

	exprtk::parser<T> parser;

	for(; pY < startY + TILE; ++pY)
	{
		uint_fast32_t startX = tileX * TILE;
		pX = startX;
		for(; pX < startX + TILE; ++pX)
		{
			x = T(pX);
			y = T(pY);
			if(!parser.compile(equation, expression))
			{
				std::cout << "Parsing error: " << parser.error() << "\n";
				return false;
			}
			uint_fast32_t color = floor(expression.value() + 0.5);
			red = color >> 16 & 0xFF;
			green = color >> 8 & 0xFF;
			blue = color & 0xFF;
			uint_fast32_t drawX = pX;
			while(drawX >= TILE)
			{
				drawX -= TILE;
			}
			uint_fast32_t drawY = pY;
			while(drawY >= TILE)
			{
				drawY -= TILE;
			}
			image.set_pixel(drawX, drawY, png::rgb_pixel(red, green, blue));
		}
	}
	image.write(imgPath);
	std::cout << "Saved tile: [" << tileX << ", " << tileY << "]\n";
	return true;
}*/

bool cancel = false;
void createFractal(bool tile)
{
	uint_fast32_t totalPoints = (tile ? TILE * TILE : (wallpaper ? wallpaperW * wallpaperH : fractalRes * fractalRes));
	uint_fast32_t currentPoint = 0;

	#ifdef USE_GMP
	mpz_t recursive;
	mpz_init(recursive);

	mpz_t escaped;
	mpz_init(escaped);

	mpz_t notEscaped;
	mpz_init(notEscaped);

	mpz_t skipped;
	mpz_init(skipped);

	mpz_t run;
	mpz_init(run);

	mpz_t maxIter;
	mpz_init(maxIter);
	#else
	uint_fast64_t recursive = 0; // amount of recursive points
	uint_fast64_t escaped = 0; // amount of escaped points
	uint_fast64_t notEscaped = 0; // amount of points that did not escape
	uint_fast64_t skipped = 0;
	uint_fast64_t run = 0; // amount of iterations processed
	uint_fast64_t maxIter = 0; // maximum iterations used on a point that escaped
	#endif

	std::complex<long double> c;
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

	png::image<png::rgb_pixel> image(tile ? TILE : (wallpaper ? wallpaperW : fractalRes), tile ? TILE : (wallpaper ? wallpaperH : fractalRes));

	uint_fast32_t startY = (tile ? tileY * TILE : 0);
	uint_fast32_t endY = (tile ? startY + TILE : (wallpaper ? wallpaperH :fractalRes));
	for(uint_fast32_t pY = startY; pY < endY; ++pY)
	{
		uint_fast32_t startX = (tile ? tileX * TILE : 0);
		uint_fast32_t endX = (tile ? startX + TILE : (wallpaper ? wallpaperW : fractalRes));
		for(uint_fast32_t pX = startX; pX < endX; ++pX)
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

			//long double a = (pX / (long double)fractalRes) * 4.0 - 2.0;
			long double a = scale;
			if(wallpaper)
			{
				a *= (pX / (double)wallpaperW);
			}
			else
			{
				a *= (pX / (double)fractalRes);
			}
			a -= 2.0;

			//long double b = (pY / (long double)fractalRes) * 4.0 - 2.0;
			long double b = scale;
			if(wallpaper)
			{
				b *= (pY / (double)wallpaperH);
			}
			else
			{
				b *= (pY / (double)fractalRes);
			}
			b -= 2.0;

			if(wallpaper)
			{
				a *= 16.0 / 9.0;
			}

			long double q = (a - 0.25)*(a - 0.25) + b*b;
			// TODO: add exponentD
			if(!single && type == mandelbrot && exponent == 2 && escapeLimit >=4 && q * (q + (a - 0.25)) < 0.25 * (b*b)) // cardioid
			{
				#ifdef USE_GMP
				mpz_add_ui(skipped, skipped, 1);
				#else
				++skipped;
				#endif
				//image.set_pixel(pX, pY, png::rgb_pixel(0, 255, 0));
			}
			else if(!single && type == mandelbrot && exponent == 2 && escapeLimit >= 4 && (a+1)*(a+1) + b*b < 0.0625) // bulb
			{
				#ifdef USE_GMP
				mpz_add_ui(skipped, skipped, 1);
				#else
				++skipped;
				#endif
				//image.set_pixel(pX, pY, png::rgb_pixel(0, 255, 0));
			}
			else
			{
				std::complex<long double> Z(a, b);
				if(type == julia)
				{
					c = std::complex<long double>(juliaA, juliaB);
				}
				else
				{
					c = std::complex<long double>(a, b);
				}
				std::complex<long double> pn(a, b); // previous iteration
				// TODO: Finish implementing this
				std::complex<long double> pCheckArray[pCheck];
				for(uint_fast32_t p = 0; p < pCheck; ++p)
				{
					pCheckArray[p] = std::complex<long double>(a, b);
				}

				//stringstream pointLog;

				long double maxNorm = norm(Z);
				long double minNorm = norm(Z);

				bool shrinking = true;
				long double lastNorm = norm(Z);

				for(n = 0; n < iterations; ++n)
				{
					if(filter1)
					{
						if(n > filter1T && minNorm > 0.02 && norm(Z) > minNorm && norm(Z) < maxNorm)
						{
							#ifdef USE_GMP
							mpz_add_ui(skipped, skipped, 1);
							#else
							++skipped;
							#endif
							if(type == neuron && colorMethod == 0)
							{
								image.set_pixel(pX, pY, png::rgb_pixel(255, 255, 255));
							}
							break;
						}
						if(norm(Z) > maxNorm)
						{
							maxNorm = norm(Z);
						}
						if(norm(Z) < minNorm)
						{
							minNorm = norm(Z);
						}
					}
					if(filter2 && n > 1 && shrinking)
					{
						if(norm(Z) < lastNorm)
						{
							if(n > filter2T)
							{
								#ifdef USE_GMP
								mpz_add_ui(skipped, skipped, 1);
								#else
								++skipped;
								#endif
								if(type == neuron && (colorMethod == 0 || colorMethod == 1 || colorMethod == 9))
								{
									image.set_pixel(pX, pY, png::rgb_pixel(255, 255, 255));
								}
								break;
							}
							else
							{
								lastNorm = norm(Z);
							}
						}
						else
						{
							shrinking = false;
						}
					}
					//pointLog << "Point " << c << ", iteration " << n << ": " << Z << " (norm is " << norm(Z) << ")\n";
					#ifdef USE_GMP
					mpz_add_ui(run, run, 1);
					#else
					++run;
					#endif
					if((single && n == iterations - 1) || (!single && norm(Z) > escapeLimit && n > 0))
					{
						//cout << "Point " << c2 << " escaped at iteration " << n << ": " << Z << "\n";
						#ifdef USE_GMP
						mpz_add_ui(escaped, escaped, 1);
						// TODO: maxIter
						#else
						++escaped;
						if(n > maxIter)
						{
							maxIter = n;
						}
						#endif
						getColor(c, Z);
						if(tile)
						{
							uint_fast32_t drawX = pX;
							while(drawX >= TILE)
							{
								drawX -= TILE;
							}
							uint_fast32_t drawY = pY;
							while(drawY >= TILE)
							{
								drawY -= TILE;
							}
							image.set_pixel(drawX, drawY, png::rgb_pixel(red, green, blue));
						}
						else
						{
							image.set_pixel(pX, pY, png::rgb_pixel(red, green, blue));
						}
						break;
					}
					if(n + 1 == iterations)
					{
						//cout << "Point " << c << " did not escape: " << Z << "\n" << pointLog.str() << "\n";
						#ifdef USE_GMP
						mpz_add_ui(notEscaped, notEscaped, 1);
						#else
						++notEscaped;
						#endif
						//image.set_pixel(pX, pY, png::rgb_pixel(255, 0, 0));
						break;
					}
					Z = iterate(Z, c);
					//if(!single && pn == Z) // do not detect recursive points in single mode
					if(!single && pCheck > 0)
					{
						bool exitLoop = false;
						for(uint_fast32_t p = 0; p < pCheck; ++p)
						{
							if(pCheckArray[p] == Z)
							{
								//cout << c2 << " is recursive at iteration " << n << ": " << pn << " == " << Z << "\n";
								#ifdef USE_GMP
								mpz_add_ui(recursive, recursive, 1);
								#else
								++recursive;
								#endif
								if(type == neuron && (colorMethod == 0 || colorMethod == 1 || colorMethod == 9))
								{
									image.set_pixel(pX, pY, png::rgb_pixel(255, 255, 255));
								}
								/*else
								{
									image.set_pixel(pX, pY, png::rgb_pixel(0, 0, 255));
								}*/
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

	ss.clear();
	ss.str("");
	ss << "tiles/" << typeStrings[type] << "/" << colorMethod << "/";

	//ss << TILE << "/" << path2 << "/" << tileX << "x" << tileY;

	if(!tile)
	{
		ss << (single ? "single_" : "full_");
		ss << colorMul << "_";
		if(negative)
		{
			ss << "-";
		}
		ss << (exponentD == 2 ? exponent : exponentD) << "_";
		if(type == julia)
		{
			ss << juliaA << "_" << juliaB << "_";
		}
		if(wallpaper)
		{
			ss << "wall" << wallpaperScale;
		}
		else
		{
			ss << fractalRes;
		}
		ss << "_" << (single ? iterations : maxIter + 1);
		if(colorMethod == 1 && disableFancy)
		{
			ss << "_df";
		}

		if(filter1)
		{
			ss << "_f1-" << filter1T;
		}
		if(filter2)
		{
			ss << "_f2-" << filter2T;
		}

		if(!single)
		{
			ss << "_" << escapeLimit;
		}

		if((colorMethod == 0 || colorMethod == 1) && smooth)
		{
			ss << "_smooth";
		}
	}
	else
	{
		ss << TILE << "/" << exponent;
		if(type == julia)
		{
			ss << "/" << juliaA << "_" << juliaB;
		}
		ss << "/" << fractalRes << "/" << iterations << "/" << tileX << "x" << tileY;
	}
	ss << ".png";
	imgPath = ss.str();

	std::cout << "\r" << startString << " saving..." << std::flush;
	image.write(imgPath);

	#ifdef USE_GMP
	char *strE = mpz_get_str(NULL, 10, escaped);
	char *strNE = mpz_get_str(NULL, 10, notEscaped);
	char *strR = mpz_get_str(NULL, 10, recursive);
	char *strS = mpz_get_str(NULL, 10, skipped);
	char *strI = mpz_get_str(NULL, 10, run);
	char *strMI = mpz_get_str(NULL, 10, maxIter);
	#endif

	std::cout << " done in ";
	if(ts.tv_sec > 0)
	{
		std::cout << ts.tv_sec << " second" << (ts.tv_sec != 1 ? "s":"") << " + ";
	}
	std::cout << ts.tv_nsec << " nanoseconds (";
	#ifdef USE_GMP
	std::cout << strE << " e, " << strNE << " ne, " << strR << " r, " << strS << " s, " << strI << " i)\n";

	free(strE);
	free(strNE);
	free(strR);
	free(strS);
	free(strI);
	free(strMI);

	mpz_set_ui(escaped, 0);
	mpz_set_ui(notEscaped, 0);
	mpz_set_ui(recursive, 0);
	mpz_set_ui(skipped, 0);
	mpz_set_ui(run, 0);
	mpz_set_ui(maxIter, 0);
	#else
	std::cout << escaped << " e, " << notEscaped << " ne, " << recursive << " r, " << skipped << " s, " << run << " i, " << maxIter << " mi)\n";

	/*std::cout << "Command: ./fractal ";
	if(type != mandelbrot)
	{
		std::cout << "-t " << typeStrings[type] << " ";
		if(type == julia)
		{
			if(juliaA != -0.8)
			{
				std::cout << "-ja " << juliaA << " ";
			}
			if(juliaB != 0.156)
			{
				std::cout << "-jb " << juliaB << " ";
			}
		}
	}
	if((exponent != 2 || (exponent == 2 && negative)) || exponentD != 2)
	{
		if(exponentD == 2)
		{
			std::cout << "-e " << (negative ? "-" : "") << exponent << " ";
		}
		else
		{
			std::cout << "-e " << exponentD << " ";
		}
	}
	if(escapeLimit != 4)
	{
		std::cout << "-el " << escapeLimit << " ";
	}
	if(maxIter != 511)
	{
		std::cout << "-i " << maxIter + 1 << " ";
	}
	if(pCheck != 0)
	{
		std::cout << "-pc " << pCheck << " ";
	}
	if(filter1)
	{
		std::cout << "-f1 " << filter1T << " ";
	}
	if(filter2)
	{
		std::cout << "-f2 " << filter2T << " ";
	}
	if(colorMethod != 0)
	{
		std::cout << "-c " << colorMethod << " ";
	}
	if(colorMul != 1)
	{
		std::cout << "-cm " << colorMul << " ";
	}
	if(smooth)
	{
		std::cout << "-s ";
	}
	if(disableFancy)
	{
		std::cout << "-df ";
	}
	if(wallpaper)
	{
		std::cout << "-w ";
	}
	else if(fractalRes != 512)
	{
		std::cout << "-r " << fractalRes << " ";
	}
	if(single)
	{
		std::cout << "-S ";
	}
	std::cout << "\n";*/

	escaped = 0;
	notEscaped = 0;
	recursive = 0;
	skipped = 0;
	run = 0;
	maxIter = 0;
	#endif // USE_GMP
}

void handler(int s)
{
	if(s == 2) // CTRL + C
	{
		cancel = true;
	}
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		argv[1] = (char*)"-h";
	}
	std::string firstArg(argv[1]);
	if(firstArg == "--help" || firstArg == "-h" || firstArg == "-?")
	{
		std::cout << "[s] means string, [d] means double, and [i] means integer. Options that take a value will fail without one.\n";
		std::cout << " -s  or --smooth          Smooth the color bands for methods 0 and 1\n";
		std::cout << " -S  or --single          Color all points with the specified iteration count\n";
		std::cout << "                           instead of the escape time\n";
		std::cout << " -df or --disableFancy    Disable fancy coloring for method 1\n";
		std::cout << " -t  or --type        [s] Choose the type of fractal to render. Available types:\n";
		std::cout << "                           mandelbrot\n";
		std::cout << "                           julia\n";
		std::cout << "                           burning_ship\n";
		std::cout << "                           tricorn\n";
		std::cout << "                           neuron\n";
		std::cout << "                           stupidbrot\n";
		std::cout << "                           untitled1\n";
		std::cout << "                           dots\n";
		std::cout << " -ja or --juliaA      [d] The real part of c (for julia only)\n";
		std::cout << " -jb or --juliaB      [d] The imaginary part of c (for julia only)\n";
		std::cout << " -c  or --colo[u]r    [i] The coloring method to use. Available values:\n";
		std::cout << "                           0 - gold (escape time)\n";
		std::cout << "                           1 - green (escape time) with red/blue crap\n";
		std::cout << "                           2 - green/orange crap with blue laser things\n";
		std::cout << "                           3 - red/blue crap with green laser thingies\n";
		std::cout << "                           4 - weird white and black crap\n";
		std::cout << "                           5 - glowing (green)\n";
		std::cout << "                           6 - glowing (pink)\n";
		std::cout << "                           7 - glowing (blue)\n";
		std::cout << "                           8 - pinkish XOR\n";
		std::cout << " -cm                  [d] Color multiplier";
		std::cout << " -r  or --resolution  [i] Picture size (width and height)\n";
		std::cout << " -i  or --iterations  [i] Maximum iterations for each point\n";
		std::cout << " -e  or --exponent    [d] Change the exponent used. Higher = slower generating\n";
		std::cout << " -f1 or --filter1     [d] A crappy point filter (not reliable)\n";
		std::cout << " -f2 or --filter2     [d] Works best with Neuron and color method 0 (try -f2 512)\n";
		std::cout << " -el or --escapeLimit [d] Be careful with this\n";
		std::cout << "\n";
		std::cout << "If an invalid value is specified, the default will be used. For the filters, the value you specify is how many iterations are run before the filter starts checking points.\n";
		return 0;
	}
	bool tile = false;
	for(int arg = 1; arg < argc; ++arg)
	{
		std::string argument(argv[arg]);
		if(argument == "-df" || argument == "--disableFancy")
		{
			disableFancy = true;
		}
		else if(argument == "-s" || argument == "--smooth")
		{
			smooth = true;
		}
		else if(argument == "-S" || argument == "--single")
		{
			single = true;
		}
		else
		{
			++arg;
			std::string value;
			try
			{
				value = std::string(argv[arg]);
			}
			catch(...)
			{
				std::cerr << "No value given for " << argument << "\n";
				return ERR_ARGS;
			}
			if(argument == "-c" || argument == "--color" || argument == "--colour")
			{
				std::stringstream ss;
				ss << value;
				ss >> colorMethod;
				//if(colorMethod > 11) colorMethod = 0;
			}
			else if(argument == "-cm")
			{
				std::stringstream ss;
				ss << value;
				ss >> colorMul;
			}
			else if(argument == "-e" || argument == "--exponent")
			{
				int e = 0;
				std::stringstream ss;
				ss << value;
				ss >> e;
				ss >> exponentD;
				if(floor(exponentD) == exponentD)
				{
					exponentD = 2;
					if(e < 0)
					{
						exponent = -e;
						negative = true;
					}
					else
					{
						exponent = e;
						negative = false;
					}
				}
				else
				{
					exponentD += e;
					exponent = floor(exponentD + 0.5);
					exponentC = std::complex<long double>(exponentD, 0);
				}
			}
			else if(argument == "-el" || argument == "--escapeLimit")
			{
				std::stringstream ss;
				ss << value;
				ss >> escapeLimit;
			}
			else if(argument == "-f1" || argument == "--filter1")
			{
				filter1 = true;
				std::stringstream ss;
				ss << value;
				ss >> filter1T;
			}
			else if(argument == "-f2" || argument == "--filter2")
			{
				filter2 = true;
				std::stringstream ss;
				ss << value;
				ss >> filter2T;
			}
			else if(argument == "-i" || argument == "--iterations")
			{
				std::stringstream ss;
				ss << value;
				ss >> iterations;
			}
			else if(argument == "-ja" || argument == "--juliaA")
			{
				std::stringstream ss;
				ss << value;
				ss >> juliaA;
			}
			else if(argument == "-jb" || argument == "--juliaB")
			{
				std::stringstream ss;
				ss << value;
				ss >> juliaB;
			}
			else if(argument == "-p" || argument == "--plane")
			{
				if(strcasecmp(value.c_str(), "lambda") == 0)
				{
					plane = lambda;
				}
				else if(strcasecmp(value.c_str(), "mu") != 0)
				{
					std::cerr << "Unknown fractal plane specified: " << value << "\n";
					return ERR_ARGS;
				}
			}
			else if(argument == "-pc")
			{
				std::stringstream ss;
				ss << value;
				ss >> pCheck;
			}
			else if(argument == "-r" || argument == "--resolution")
			{
				std::stringstream ss;
				ss << value;
				ss >> fractalRes;
			}
			else if(argument == "-t" || argument == "--type")
			{
				if(strcasecmp(value.c_str(), "julia") == 0)
				{
					type = julia;
				}
				else if(strcasecmp(value.c_str(), "burning_ship") == 0)
				{
					type = burning_ship;
				}
				else if(strcasecmp(value.c_str(), "tricorn") == 0)
				{
					type = tricorn;
				}
				else if(strcasecmp(value.c_str(), "neuron") == 0)
				{
					type = neuron;
				}
				else if(strcasecmp(value.c_str(), "stupidbrot") == 0)
				{
					type = stupidbrot;
				}
				else if(strcasecmp(value.c_str(), "untitled1") == 0)
				{
					type = untitled1;
				}
				else if(strcasecmp(value.c_str(), "dots") == 0)
				{
					type = dots;
				}
				else if(strcasecmp(value.c_str(), "magnet1") == 0)
				{
					type = magnet1;
				}
				else if(strcasecmp(value.c_str(), "experiment") == 0)
				{
					type = experiment;
				}
				else if(strcasecmp(value.c_str(), "mandelbrot") != 0)
				{
					std::cerr << "Unknown fractal type specified: " << value << "\n";
					return ERR_ARGS;
				}
			}
			else if(argument == "-tx" || argument == "--tileX")
			{
				tile = true;
				std::stringstream ss;
				ss << value;
				ss >> tileX;
			}
			else if(argument == "-ty" || argument == "--tileY")
			{
				tile = true;
				std::stringstream ss;
				ss << value;
				ss >> tileY;
			}
			else if(argument == "-w" || argument == "--wallpaper")
			{
				wallpaper = true;
				std::stringstream ss;
				ss << value;
				ss >> wallpaperScale;
				if(wallpaperScale != 0)
				{
					wallpaperW *= wallpaperScale;
					wallpaperH *= wallpaperScale;
				}
				else
				{
					std::cerr << "Wallpaper scale can not be 0\n";
					return ERR_ARGS;
				}
			}
			else if(argument == "-z" || argument == "--zoom")
			{
				std::stringstream ss;
				ss << value;
				ss >> scale;
				if(scale != 0)
				{
					scale = 4.0 / scale;
				}
				else
				{
					std::cerr << "Zoom can not be 0\n";
					return ERR_ARGS;
				}
			}
		}
	}

	if(tile)
	{
		uint_fast32_t maxTile = 0;
		uint_fast32_t tmp = fractalRes;

		while(tmp > tmp % TILE)
		{
			tmp -= TILE;
			++maxTile;
		}

		if(tileX >= maxTile || tileY >= maxTile)
		{
			std::cout << "Tile out of bounds\n";
			return ERR_TOOB;
		}
	}

	if(mkdir("tiles", MODE) != 0)
	{
		if(errno != EEXIST)
		{
			std::cout << strerror(errno) << "\n";
			return ERR_DIR0;
		}
	}

	std::stringstream ss;
	ss << "tiles/" << typeStrings[type];
	std::string path = ss.str();
	if(mkdir(path.c_str(), MODE) != 0)
	{
		if(errno != EEXIST)
		{
			std::cout << strerror(errno) << "\n";
			return ERR_DIR1;
		}
	}

	ss << "/" << colorMethod;
	path = ss.str();
	if(mkdir(path.c_str(), MODE) != 0)
	{
		if(errno != EEXIST)
		{
			std::cout << strerror(errno) << "\n";
			return ERR_DIR2;
		}
	}

	if(tile)
	{
		ss << "/" << TILE;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) != 0)
		{
			if(errno != EEXIST)
			{
				std::cout << strerror(errno) << "\n";
				return ERR_DIR3;
			}
		}
		ss << "/";
		if(negative)
		{
			ss << "-";
		}
		ss << exponent;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) != 0)
		{
			if(errno != EEXIST)
			{
				std::cout << strerror(errno) << "\n";
				return ERR_DIR4;
			}
		}

		if(type == julia)
		{
			ss << "/" << juliaA << "_" << juliaB;
			path = ss.str();
			if(mkdir(path.c_str(), MODE) !=0)
			{
				if(errno != EEXIST)
				{
					std::cout << strerror(errno) << "\n";
					return ERR_DIR5;
				}
			}
		}

		ss << "/" << fractalRes;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) !=0)
		{
			if(errno != EEXIST)
			{
				std::cout << strerror(errno) << "\n";
				return ERR_DIR6;
			}
		}
		ss << "/" << iterations;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) !=0)
		{
			if(errno != EEXIST)
			{
				std::cout << strerror(errno) << "\n";
				return ERR_DIR7;
			}
		}
	}

	/*if(argc > 6)
	{
		stringstream ss_e;
		for(int arg = 6; arg < argc; ++arg)
		{
			ss_e << argv[arg];
			if(arg != argc - 1)
			{
				ss_e << " ";
			}
		}
		equation = ss_e.str();
		std::cout << "Equation: " << equation << "\n";
		path2 = equation;
		replace(path2.begin(), path2.end(), '/', '\\');
		ss << "/" << path2;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) !=0)
		{
			if(errno != EEXIST)
			{
				std::cout << strerror(errno) << "\n";
				return ERR_DIR8;
			}
		}
	}
	else
	{
		std::cout << "No equation given\n";
		return ERR_NOEQ;
	}*/

	ss.clear();
	ss.str("");

	//if(!createEquation<long double>()) return ERR_EQTN;

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	createFractal(tile);

	return 0;
}
