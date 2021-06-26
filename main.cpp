#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

struct complex
{
	double r, i;

	friend complex operator*(const complex& a, const complex& b)
	{
		double real = a.r * b.r - a.i * b.i;
		double imag = a.r * b.i + a.i * b.r;

		return complex{ real, imag };
	}

	friend complex operator+(const complex& a, const complex& b)
	{
		return complex{ a.r + b.r, a.i + b.i };
	}

	operator dvec2()
	{
		return glm::dvec2{ r, i };
	}
};

double abs(complex c)
{
	return sqrt(c.r * c.r + c.i * c.i);
}

double mag2(complex c)
{
	return c.r * c.r + c.i * c.i;
}

int mandelbrot(complex c, int max_iter)
{
	complex z{ 0, 0 };
	int n = 0;

	while (mag2(z) <= 4 && n < max_iter)
	{
		z = z*z + c;
		n += 1;
	}

	return n;
}

using namespace glm;

mat3 translate2d(dvec2 pos)
{
	return mat3(1, 0, 0,
		0, 1, 0,
		pos.x, pos.y, 1);
}

mat3 scale2d(dvec2 factor)
{
	return mat3(factor.x, 0, 0,
		0, factor.y, 0,
		0, 0, 1);
}

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		static double RE_START = -2;
		static double RE_END = 1;
		static double IM_START = -1;
		static double IM_END = 1;

		static dvec2 center{ 0, 0 };
		static dvec2 delta{ 1, 1 };

		double scrollSpeed = 0.5;

		dvec2 halfSize = dvec2(ScreenWidth() / 2.0f, ScreenHeight() / 2.0f);

		static dmat3 spaceToScreen = dmat3();
		static dmat3 screenToSpace = dmat3();

		dmat3 scale = scale2d(dvec2(delta.x / ScreenWidth(), delta.y / ScreenHeight())) * translate2d(-halfSize);
		dmat3 scaleInv = scale2d(dvec2(delta.x / ScreenWidth(), delta.y / ScreenHeight()));
		dmat3 trans = translate2d(center);
		dmat3 transInv = translate2d(-center);

		if (GetKey(olc::Key::LEFT).bHeld) center.x -= scrollSpeed * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld) center.x += scrollSpeed * fElapsedTime;

		if (GetKey(olc::Key::UP).bHeld) center.y -= scrollSpeed * fElapsedTime;
		if (GetKey(olc::Key::DOWN).bHeld) center.y += scrollSpeed * fElapsedTime;

		static dvec2 oldmouse = dvec2();
		static dvec2 oldcenter = dvec2();

		if (GetMouse(0).bPressed || GetMouse(2).bPressed)
		{
			oldmouse = dvec2(GetMousePos().x, GetMousePos().y);;

			oldcenter = center;
		}

		if (GetMouse(0).bHeld || GetMouse(2).bHeld)
		{
			dvec2 mousepos(GetMousePos().x, GetMousePos().y);

			dvec2 delta = oldmouse - mousepos;

			center = oldcenter + dvec2(scale * dvec3(delta, 0));

			printf("MOUSE: %f - %f", mousepos.x, mousepos.y);
			std::cout << "POS: " << to_string(screenToSpace * vec3(mousepos, 1.0)) << std::endl;
		}

		if (GetKey(olc::W).bHeld)
		{
			center.y -= 0.1f * fElapsedTime;
		}

		if (GetKey(olc::S).bHeld)
		{
			center.y += 0.1f * fElapsedTime;
		}
		if (GetKey(olc::D).bHeld)
		{
			center.x += 0.1f * fElapsedTime;
		}
		if (GetKey(olc::A).bHeld)
		{
			center.x -= 0.1f * fElapsedTime;
		}		

		if (GetMouse(2).bHeld)
		{
			delta.x *= 0.95f;
			delta.y *= 0.95f;
		}

		if (GetMouseWheel() > 0)
		{
			delta.x *= 1.05f;
			delta.y *= 1.05f;
		}
		if (GetMouseWheel() < 0)
		{
			delta.x *= 0.95f;
			delta.y *= 0.95f;
		}

		//std::cout << "SCALE: " << to_string(scale) << std::endl;
		//std::cout << "TRANSLATE: " << to_string(trans) << std::endl;
		//std::cout << "COMPOSED: " << to_string(screenToSpace) << std::endl;

		spaceToScreen = scaleInv * transInv;
		screenToSpace = trans * scale;

		// called once per frame
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				//c = complex(center.r - delta.r + (x / WIDTH) * (RE_END - RE_START),
				//	IM_START + (y / HEIGHT) * (IM_END - IM_START))
				// * mat3(1, 0, center.r, 0, 1, center.i, 0, 0, 1) *
				dvec2 plot = screenToSpace * vec3(x, y, 1);


				complex c{ plot.x, plot.y };

				int iter = mandelbrot(c, 80);

				int color = (iter / 80.0) * 128.0f;

				Draw(x, y, olc::Pixel(color, color, color));
			}

		RE_START *= 1.1 * fElapsedTime;
		RE_END *= 1.1 * fElapsedTime;
		IM_START *= 1.1 * fElapsedTime;
		IM_END *= 1.1 * fElapsedTime;

		return true;
	}
};


int main()
{
	Example demo;
	if (demo.Construct(100, 100, 8, 8))
		demo.Start();

	return 0;
}
