#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;
#include <cmath>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <cerrno>
#include <strings.h>
#include <png++/png.hpp>
#define exprtk_lean_and_mean
#include <exprtk.hpp>
#include <gmp.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

inline void clearLine(unsigned int spaces)
{
	cout << "\r";
	for(unsigned int c = 0; c < spaces; ++c)
	{
		cout << " ";
	}
	cout << flush;
}

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679

#define TILE 512

#define MODE 0777
#define ERR_ARGS  1 // not enough arguments
#define ERR_DIR0  2 // tiles
#define ERR_DIR1  3 // tiles/TYPE
#define ERR_DIR2  4 // tiles/TYPE/COLOR
#define ERR_DIR3  5 // tiles/TYPE/COLOR/TILE
#define ERR_DIR4  6 // tiles/TYPE/COLOR/TILE/EXPONENT
#define ERR_DIR5  7 // tiles/TYPE/COLOR/TILE/EXPONENT/RES
#define ERR_DIR6  8 // tiles/TYPE/COLOR/TILE/EXPONENT/RES/ITERATIONS
#define ERR_DIR7  9 // tiles/TYPE/COLOR/TILE/EQUATION
#define ERR_TOOB 10 // tile out of bounds
#define ERR_NOEQ 11 // no equation given
#define ERR_EQTN 12 // error parsing equation

enum FractalType
{
	mandelbrot,
	julia,
	burning_ship,
	triforce,
	neuron,
	experiment
};

FractalType type = mandelbrot;
string typeString = "Mandelbrot";
double juliaA = -0.8;
double juliaB = 0.156;
unsigned int colorMethod = 0;
unsigned int fractalRes = 512;
unsigned int iterations = 1024;
unsigned int tileX = 0;
unsigned int tileY = 0;
unsigned int exponent = 2;
bool negative = false;
bool smooth = false;
bool filter1 = false;
unsigned int filter1T = 512;
bool filter2 = false;
unsigned int filter2T = 512;
bool disableFancy = false;
double escapeLimit = 4;

complex<double> lepow(complex<double> base, unsigned int e)
{
	if(e == 0)
	{
		return complex<double>(1, 0);
	}
	else if(e == 1)
	{
		return base;
	}
	complex<double> out = base;
	for(unsigned int i = 1; i < e; ++i)
	{
		out *= base;
	}
	if(negative)
	{
		complex<double> one(1.0, 0.0);
		out = one/out;
	}
	return out;
}

//double Zr;
//double Zi;
double Zr2;
double Zi2;
unsigned int n;

string imgPath = "";
string path2;

unsigned int red, green, blue;
void getColor(complex<double>c, complex<double> Z)
{
	Zr2 = real(Z)*real(Z);
	Zi2 = imag(Z)*imag(Z);
	switch(colorMethod)
	{
		case 0:
		{
			if(smooth) // TODO
			{
				Z = lepow(Z, exponent) + c;
				Z = lepow(Z, exponent) + c;
				n += 2;
				double newColor = (n - (log(log(abs(Z)))) / log((double)exponent));
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
		case 1:
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
			if(smooth) // TODO
			{
				Z = lepow(Z, exponent) + c;
				Z = lepow(Z, exponent) + c;
				n += 2;
				double newColor = (n - (log(log(abs(Z)))) / log((double)exponent));
				green = floor(newColor + 0.5);
			}
			else
			{
				green = n;
			}
			if(green > 255)
			{
				unsigned int difference = green - 255;
				green = 255;
				while(difference > 255)
				{
					difference -= 255;
				}
				red = difference;
				blue = difference;
			}
			break;
		}
		case 2:
		{
			red = Zr2 * Zi2;
			green = Zr2 + Zi2;
			blue = Zr2 / Zi2;
			break;
		}
		case 3:
		{
			red = (Zr2 * Zr2 * Zr2 + 1) / Zr2;
			green = Zi2 / Zr2;
			blue = Zi2 * Zi2;
			break;
		}
		case 4: // Ben
		{
			red = green = blue = real(Z) * sin(imag(Z) + Zi2) - Zr2;
			break;
		}
		case 5: // Ben 2
		{
			red = real(Z) * (sin(imag(Z) + Zi2) + 2.0) * 100.0 - Zr2;
			//green = imag(Z) * sin(real(Z) + Zi2) * 100.0 - Zr2;
			//blue = sin(real(Z) * imag(Z) + Zi2) * 100.0 - Zr2;
			break;
		}
		default:
		{
			red = n;
			green = n;
			blue = n;
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

	/*while(red > 255)
	{
		red -= 255;
	}
	while(green > 255)
	{
		green -= 255;
	}
	while(blue > 255)
	{
		blue -= 255;
	}*/
}

/*string equation;
template<typename T> bool createEquation()
{
	png::image<png::rgb_pixel> image(TILE, TILE);
	unsigned int startY = tileY * TILE;
	unsigned int pY = startY;
	unsigned int pX = 0;

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
		unsigned int startX = tileX * TILE;
		pX = startX;
		for(; pX < startX + TILE; ++pX)
		{
			x = T(pX);
			y = T(pY);
			if(!parser.compile(equation, expression))
			{
				cout << "Parsing error: " << parser.error() << "\n";
				return false;
			}
			unsigned int color = floor(expression.value() + 0.5);
			red = color >> 16 & 0xFF;
			green = color >> 8 & 0xFF;
			blue = color & 0xFF;
			unsigned int drawX = pX;
			while(drawX >= TILE)
			{
				drawX -= TILE;
			}
			unsigned int drawY = pY;
			while(drawY >= TILE)
			{
				drawY -= TILE;
			}
			image.set_pixel(drawX, drawY, png::rgb_pixel(red, green, blue));
		}
	}
	image.write(imgPath);
	cout << "Saved tile: [" << tileX << ", " << tileY << "]\n";
	return true;
}*/

void createFractal(bool tile)
{
	unsigned int totalPoints = fractalRes * fractalRes;
	unsigned int currentPoint = 0;

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

	complex<double> c;
//#define ANIM
// There is no option for creating animations.
// I change this to whatever it needs to be if I want an animation, then ffmpeg combines the frames.
	#ifdef ANIM
	double interval = 0.1;
	escapeLimit = 0;
	double end = 512;
	/*double startA = -4.0;
	double startB = 4.0;
	double curA = startA;
	double curB = startB;
	double endA = 4.0;
	//double endB = -4.0;*/
	double multiplier = 1/interval;
	unsigned int frames = end * multiplier;
	//unsigned int frames = abs(startA) * multiplier + abs(endA) * multiplier;
	//cout << "Start A: " << startA << "\nStart B: " << startB << "\nTotal frames: " << frames << "\n";
	cout << "Interval: " << interval << "\nTotal frames: " << frames << "\n";
	for(unsigned int frame = 0; frame < frames + 1; ++frame)
	{
		string zPad = "";
		if(frame < 10)
		{
			zPad = "000";
		}
		else if(frame < 100)
		{
			zPad = "00";
		}
		else if(frame < 1000)
		{
			zPad = "0";
		}
		stringstream ss;
		ss << "tiles/" << typeString << "/" << colorMethod << "/anim/frame" << zPad << frame << ".png";
		string framePath = ss.str();
		ifstream ifile(framePath.c_str());
		if(!ifile)
		{
			//complex<double> c(curA, curB);
			//complex<double> c(-0.8, 0.156);
			ss.str("");
			ss.clear();
			ss << "Rendering frame #" << zPad << frame << " (el = " << escapeLimit << ")...";//" (c = " << real(c) << " + " << imag(c) << "i)...";
			#else
			stringstream ss;
			ss << "Rendering " << typeString << "...";
			#endif
			string startString = ss.str();
			cout << startString << flush;
			unsigned int spaces = 0;

			time_t start, end, statusT1, statusT2;
			time(&start);
			time(&statusT1);
			time(&statusT2);

			png::image<png::rgb_pixel> image(tile ? TILE : fractalRes, tile ? TILE : fractalRes);
			unsigned int startY = (tile ? tileY * TILE : 0);
			unsigned int endY = (tile ? startY + TILE : fractalRes);
			for(unsigned int pY = startY; pY < endY; ++pY)
			{
				unsigned int startX = (tile ? tileX * TILE : 0);
				unsigned int endX = (tile ? startX + TILE : fractalRes);
				for(unsigned int pX = startX; pX < endX; ++pX)
				{
					if(difftime(statusT2, statusT1) >= 1)
					{
						ss.str("");
						ss.clear();
						ss << startString << " point " << currentPoint << " of " << totalPoints;;
						string status = ss.str();
						spaces = status.length();
						clearLine(spaces);
						cout << "\r" << status << flush;
						time(&statusT1);
					}
					else
					{
						time(&statusT2);
					}

					double a = (pX / (double)fractalRes) * 4.0 - 2.0;
					double b = (pY / (double)fractalRes) * 4.0 - 2.0;
					double q = (a - 0.25)*(a - 0.25) + b*b;
					if(type == mandelbrot && exponent == 2 && escapeLimit >=4 && q * (q + (a - 0.25)) < 0.25 * (b*b)) // cardioid
					{
						mpz_add_ui(skipped, skipped, 1);
					}
					else if(type == mandelbrot && exponent == 2 && escapeLimit >= 4 && (a+1)*(a+1) + b*b < 0.0625) // bulb
					{
						mpz_add_ui(skipped, skipped, 1);
					}
					else
					{
						complex<double> Z(a, b);
						if(type == julia)
						{
							c = complex<double>(juliaA, juliaB);
						}
						else
						{
							c = complex<double>(a, b);
						}
						complex<double> pn(a, b); // previous iteration

						//stringstream pointLog;

						double maxNorm = norm(Z);
						double minNorm = norm(Z);

						bool shrinking = true;
						double lastNorm = norm(Z);

						for(n = 0; n < iterations; ++n)
						{
							if(filter1)
							{
								if(n > filter1T && minNorm > 0.02 && norm(Z) > minNorm && norm(Z) < maxNorm)
								{
									mpz_add_ui(skipped, skipped, 1);
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
										mpz_add_ui(skipped, skipped, 1);
										if(type == neuron && (colorMethod == 0 || colorMethod == 1))
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
							mpz_add_ui(run, run, 1);
							if(norm(Z) > escapeLimit)
							{
								//cout << "Point " << c2 << " escaped at iteration " << n << ": " << Z << "\n";
								mpz_add_ui(escaped, escaped, 1);
								getColor(c, Z);
								if(tile)
								{
									unsigned int drawX = pX;
									while(drawX >= TILE)
									{
										drawX -= TILE;
									}
									unsigned int drawY = pY;
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
							if(n+1 == iterations)
							{
								//cout << "Point " << c << " did not escape: " << Z << "\n" << pointLog.str() << "\n";
								mpz_add_ui(notEscaped, notEscaped, 1);
								//image.set_pixel(pX, pY, png::rgb_pixel(255, 255, 255));
								break;
							}
							if(type == mandelbrot || type == julia)
							{
								Z = lepow(Z, exponent) + c;
							}
							else if(type == burning_ship)
							{
								Z = lepow(complex<double>(abs(real(Z)), abs(imag(Z))), exponent) + c;
							}
							else if(type == triforce)
							{
								Z = lepow(complex<double>(imag(Z), real(Z)), exponent) + c;
							}
							else if(type == neuron)
							{
								Z = lepow(complex<double>(imag(Z), real(Z)), exponent) + Z;
							}
							if(pn == Z)
							{
								//cout << c2 << " is recursive at iteration " << n << ": " << pn << " == " << Z << "\n";
								mpz_add_ui(recursive, recursive, 1);
								if(type == neuron && (colorMethod == 0 || colorMethod == 1))
								{
									image.set_pixel(pX, pY, png::rgb_pixel(255, 255, 255));
								}
								break;
							}
							pn = Z;
						}
					}
					++currentPoint;
				}
			}

			time(&end);

			clearLine(spaces);
			cout << "\r" << startString << " saving..." << flush;
			#ifdef ANIM
			image.write(framePath);
			#else
			image.write(imgPath);
			#endif

			char *strE = mpz_get_str(NULL, 10, escaped);
			char *strNE = mpz_get_str(NULL, 10, notEscaped);
			char *strR = mpz_get_str(NULL, 10, recursive);
			char *strS = mpz_get_str(NULL, 10, skipped);
			char *strI = mpz_get_str(NULL, 10, run);

			double renderTime = difftime(end, start);
			cout << " done in " << renderTime << " second" << (renderTime != 1 ? "s" : "") << " (" << strE << " e, " << strNE << " ne, " << strR << " r, " << strS << " s, " << strI << " i)\n";

			free(strE);
			free(strNE);
			free(strR);
			free(strS);
			free(strI);

			mpz_set_ui(escaped, 0);
			mpz_set_ui(notEscaped, 0);
			mpz_set_ui(recursive, 0);
			mpz_set_ui(skipped, 0);
			mpz_set_ui(run, 0);
		#ifdef ANIM
		}
		//curA += interval;
		//curB -= interval;
		escapeLimit += interval;
	}
	#endif
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		cout << "No arguments given\n";
		return ERR_ARGS;
	}
	string firstArg(argv[1]);
	if(firstArg == "--help" || firstArg == "-h" || firstArg == "-?")
	{
		cout << "[s] means string, [d] means double, and [i] means integer. Options that take a value will fail without one.\n";
		cout << " -s  or --smooth          Smooth the color bands for methods 0 and 1\n";
		cout << " -df or --disableFancy    Disable fancy coloring for method 1\n";
		cout << " -t  or --type        [s] Choose the type of fractal to render. Available types:\n";
		cout << "                           mandelbrot\n";
		cout << "                           julia\n";
		cout << "                           burning_ship\n";
		cout << "                           triforce\n";
		cout << "                           neuron\n";
		cout << " -ja or --juliaA      [d] The real part of c (for julia only)\n";
		cout << " -jb or --juliaB      [d] The imaginary part of c (for julia only)\n";
		cout << " -c  or --colo[u]r    [i] The coloring method to use. Available values:\n";
		cout << "                           0 - gold (escape time)\n";
		cout << "                           1 - green (escape time) with red/blue crap\n";
		cout << "                           2 - green/orange crap with blue laser things\n";
		cout << "                           3 - red/blue crap with green laser thingies\n";
		cout << "                           4 - weird white and black crap\n";
		cout << "                           5 - similar to 4, but multicolored\n";
		cout << " -r  or --resolution  [i] Picture size (width and height)\n";
		cout << " -i  or --iterations  [i] Maximum iterations for each point\n";
		cout << " -e  or --exponent    [d] Change the exponent used. Higher = slower generating\n";
		cout << " -f1 or --filter1     [d] A crappy point filter (not reliable)\n";
		cout << " -f2 or --filter2     [d] Works best with Neuron and color method 0 (try -f2 512)\n";
		cout << " -el or --escapeLimit [d] Be careful with this\n";
		cout << "\n";
		cout << "If an invalid value is specified, the default will be used. For the filters, the value you specify is how many iterations are run before the filter starts checking points.\n";
		return 0;
	}
	bool genAll = true;
	for(int arg = 1; arg < argc; ++arg)
	{
		string argument(argv[arg]);
		if(argument == "-df" || argument == "--disableFancy")
		{
			disableFancy = true;
		}
		else if(argument == "-s" || argument == "--smooth")
		{
			smooth = true;
		}
		else
		{
			++arg;
			string value(argv[arg]);
			if(argument == "-t" || argument == "--type")
			{
				if(strcasecmp(value.c_str(), "julia") == 0)
				{
					type = julia;
					typeString = "Julia";
				}
				else if(strcasecmp(value.c_str(), "burning_ship") == 0)
				{
					type = burning_ship;
					typeString = "Burning Ship";
				}
				else if(strcasecmp(value.c_str(), "triforce") == 0)
				{
					type = triforce;
					typeString = "Triforce";
				}
				else if(strcasecmp(value.c_str(), "neuron") == 0)
				{
					type = neuron;
					typeString = "Neuron";
				}
				else if(strcasecmp(value.c_str(), "experiment") == 0)
				{
					type = experiment;
					typeString = "Experiment";
				}
				else if(strcasecmp(value.c_str(), "mandelbrot") != 0)
				{
					cout << "Unknown fractal type specified: " << value << "\n";
				}
			}
			else if(argument == "-ja" || argument == "--juliaA")
			{
				stringstream ss;
				ss << value;
				ss >> juliaA;
			}
			else if(argument == "-jb" || argument == "--juliaB")
			{
				stringstream ss;
				ss << value;
				ss >> juliaB;
			}
			else if(argument == "-c" || argument == "--color" || argument == "--colour")
			{
				stringstream ss;
				ss << value;
				ss >> colorMethod;
				if(colorMethod > 5)
				{
					colorMethod = 0;
				}
			}
			else if(argument == "-r" || argument == "--resolution")
			{
				stringstream ss;
				ss << value;
				ss >> fractalRes;
			}
			else if(argument == "-i" || argument == "--iterations")
			{
				stringstream ss;
				ss << value;
				ss >> iterations;
			}
			else if(argument == "-e" || argument == "--exponent")
			{
				int e = 0;
				stringstream ss;
				ss << value;
				ss >> e;
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
			else if(argument == "-tx" || argument == "--tileX")
			{
				genAll = false;
				stringstream ss;
				ss << value;
				ss >> tileX;
			}
			else if(argument == "-ty" || argument == "--tileY")
			{
				genAll = false;
				stringstream ss;
				ss << value;
				ss >> tileY;
			}
			else if(argument == "-f1" || argument == "--filter1")
			{
				filter1 = true;
				stringstream ss;
				ss << value;
				ss >> filter1T;
			}
			else if(argument == "-f2" || argument == "--filter2")
			{
				filter2 = true;
				stringstream ss;
				ss << value;
				ss >> filter2T;
			}
			else if(argument == "-el" || argument == "--escapeLimit")
			{
				stringstream ss;
				ss << value;
				ss >> escapeLimit;
			}
		}
	}

	if(!genAll)
	{
		unsigned int maxTile = 0;
		unsigned int tmp = fractalRes;

		while(tmp > tmp % TILE)
		{
			tmp -= TILE;
			++maxTile;
		}

		if(tileX >= maxTile || tileY >= maxTile)
		{
			cout << "Tile out of bounds\n";
			return ERR_TOOB;
		}
	}

	if(mkdir("tiles", MODE) != 0)
	{
		if(errno != EEXIST)
		{
			cout << strerror(errno) << "\n";
			return ERR_DIR0;
		}
	}

	stringstream ss;
	ss << "tiles/" << typeString;
	string path = ss.str();
	if(mkdir(path.c_str(), MODE) != 0)
	{
		if(errno != EEXIST)
		{
			cout << strerror(errno) << "\n";
			return ERR_DIR1;
		}
	}

	ss << "/" << colorMethod;
	path = ss.str();
	if(mkdir(path.c_str(), MODE) != 0)
	{
		if(errno != EEXIST)
		{
			cout << strerror(errno) << "\n";
			return ERR_DIR2;
		}
	}

	if(!genAll)
	{
		ss << "/" << TILE;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) != 0)
		{
			if(errno != EEXIST)
			{
				cout << strerror(errno) << "\n";
				return ERR_DIR3;
			}
		}
		ss << "/" << exponent;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) != 0)
		{
			if(errno != EEXIST)
			{
				cout << strerror(errno) << "\n";
				return ERR_DIR4;
			}
		}
		ss << "/" << fractalRes;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) !=0)
		{
			if(errno != EEXIST)
			{
				cout << strerror(errno) << "\n";
				return ERR_DIR5;
			}
		}
		ss << "/" << iterations;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) !=0)
		{
			if(errno != EEXIST)
			{
				cout << strerror(errno) << "\n";
				return ERR_DIR6;
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
		cout << "Equation: " << equation << "\n";
		path2 = equation;
		replace(path2.begin(), path2.end(), '/', '\\');
		ss << "/" << path2;
		path = ss.str();
		if(mkdir(path.c_str(), MODE) !=0)
		{
			if(errno != EEXIST)
			{
				cout << strerror(errno) << "\n";
				return ERR_DIR7;
			}
		}
	}
	else
	{
		cout << "No equation given\n";
		return ERR_NOEQ;
	}*/

	ss.clear();
	ss.str("");

	ss << "tiles/" << typeString << "/" << colorMethod << "/";

	//ss << TILE << "/" << path2 << "/" << tileX << "x" << tileY;

	if(genAll)
	{
		ss << "full_";
		if(negative)
		{
			ss << "-";
		}
		ss << exponent << "_";
		if(type == julia)
		{
			ss << juliaA << "_" << juliaB << "_";
		}
		ss << fractalRes << "_" << iterations;
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

		ss << "_" << escapeLimit;

		if((colorMethod == 0 || colorMethod == 1) && smooth)
		{
			ss << "_smooth";
		}
	}
	else
	{
		ss << TILE << "/" << exponent << "/" << fractalRes << "/" << iterations << "/" << tileX << "x" << tileY;
	}
	ss << ".png";
	imgPath = ss.str();

	//if(!createEquation<double>()) return ERR_EQTN;

	createFractal(!genAll);

	return 0;
}
