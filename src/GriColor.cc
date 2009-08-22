/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2008 Daniel Kelley

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

//#define DEBUG_GRICOLOR

#include "GriColor.hh"		// from gr.hh anyway
#include "gr.hh"
#include "types.hh"
#include "errors.hh"		// for OUT_OF_MEMORY

GriColor& GriColor::operator=(const GriColor& color)
{
	t = color.get_type();
	transparency = color.getT();
	switch (t) {
	case rgb:
		a = color.getR();
		b = color.getG();
		c = color.getB();
		break;
	case hsv:
		a = color.getH();
		b = color.getS();
		c = color.getV();
		break;
	case cmyk:
		a = color.getC();
		b = color.getM();
		c = color.getY();
		d = color.getK();
		break;
	default:
		fprintf(stderr, "GriColor::operator= cannot handle this (%d) GriColor type\n", (int)t);
		exit(1);
	}
	return *this;
}

GriColor::GriColor(const GriColor& color)
{
	t = color.get_type();
	transparency = color.getT();
	switch (t) {
	case rgb:
		a = color.getR();
		b = color.getG();
		c = color.getB();
		break;
	case hsv:
		a = color.getH();
		b = color.getS();
		c = color.getV();
		break;
	case cmyk:
		a = color.getC();
		b = color.getM();
		c = color.getY();
		d = color.getK();
		break;
	default:
		fprintf(stderr, "GriColor::GriColor cannot handle this (%d) GriColor type\n", t);
		exit(1);
	}
}

GriColor::~GriColor()
{
	;
}

void
GriColor::setRGB(double R, double G, double B)
{
	a = pin0_1(R);
	b = pin0_1(G);
	c = pin0_1(B);
	transparency = 0.0;
	t = rgb;
}
void GriColor::setHSV(double H, double S, double V)
{
	a = pin0_1(H);
	b = pin0_1(S);
	c = pin0_1(V);
	transparency = 0.0;
	t = hsv;
}
void GriColor::setCMYK(double C, double M, double Y, double K)
{
	a = pin0_1(C);
	b = pin0_1(M);
	c = pin0_1(Y);
	d = pin0_1(K);
	transparency = 0.0;
	t = cmyk;
}
GriNamedColor::GriNamedColor()
{
	name = "";
	a = b = c = d = 0.0;
	transparency = 0.0;
	t = rgb;
}

GriNamedColor::GriNamedColor(const char *n, double R, double G, double B)
{
	name = n;
	a = R;
	b = G;
	c = B;
	transparency = 0.0;
	t = rgb;
}

GriNamedColor::GriNamedColor(const GriNamedColor& color) 
{
#if defined(DEBUG_GRICOLOR)
	printf("GriNamedColor(const color) ENTER (assigning from '%s')\n",color.get_name().c_str());
#endif
	name.assign(color.get_name());
	t = color.get_type();
	transparency = 0.0;
	switch (t) {
	case rgb:
		a = color.getR();
		b = color.getG();
		c = color.getB();
		break;
	case hsv:
		a = color.getH();
		b = color.getS();
		c = color.getV();
		break;
	case cmyk:
		a = color.getC();
		b = color.getM();
		c = color.getY();
		d = color.getK();
		break;
	default:
		fprintf(stderr, "GriNamedColor::GriNamedColor cannot handle this (%d) GriColor type\n", t);
		exit(1);
	}
#if defined(DEBUG_GRICOLOR)
	printf("GriNamedColor(const color) EXIT\n");
#endif
}

GriNamedColor::~GriNamedColor()
{
	;
}

GriNamedColor& GriNamedColor::operator=(const GriNamedColor& color)
{
#if defined(DEBUG_GRICOLOR)
	printf("GriNamedColor::operator= ENTER (source '%s'\n",color.get_name().c_str());
#endif
	name.assign(color.get_name());
	t = color.get_type();
	transparency = 0.0;
	switch (t) {
	case rgb:
		a = color.getR();
		b = color.getG();
		c = color.getB();
		break;
	case hsv:
		a = color.getH();
		b = color.getS();
		c = color.getV();
		break;
	case cmyk:
		a = color.getC();
		b = color.getM();
		c = color.getY();
		d = color.getK();
		break;
	default:
		fprintf(stderr, "GriNamedColor::operator= cannot handle this (%d) GriColor type\n", t);
		exit(1);
	}
#if defined(DEBUG_GRICOLOR)
	printf("GriNamedColor::operator= EXIT\n");
#endif
	return *this;
}

void GriNamedColor::setNameRGB(const char *newName, double R, double G, double B)
{
	name = newName;
	t = rgb;
	transparency = 0.0;
	a = pin0_1(R);
	b = pin0_1(G);
	c = pin0_1(B);
}
std::string
GriColor::get_hexcolor() const
{
	char hex_color[8];	// result is 7 chars long
	double r, g, b;
	getRGB(&r, &g, &b);
	sprintf(hex_color, "#%02x%02x%02x", int(r*255),int(g*255),int(b*255));
	return(std::string(hex_color));
}

void
GriColor::getRGB(double *R, double *G, double *B) const
{
	if (t == GriColor::rgb) {
		*R = a;
		*G = b;
		*B = c;
	} else if (t == GriColor::hsv) {
		gr_hsv2rgb(a, b, c, R, G, B);
	} else {
		gr_error("Internal error in GriColor::getRGB\n");
	}
}
