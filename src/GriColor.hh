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

#ifndef _gricolor_h_
#define  _gricolor_h_

#include <string>
#include <stdio.h>
#include "types.hh"

class GriColor
{
public:
	enum type {rgb, hsv, cmyk};
	GriColor() { t = rgb; transparency = a = b = c = d = 0.0;};
	GriColor(const GriColor& c);
        ~GriColor();
	GriColor& operator=(const GriColor& c);
	void setHSV(double H, double S, double V);
	void setRGB(double R, double G, double B);
	void setCMYK(double C, double M, double Y, double K);
	bool isRGB() const {return t == rgb;}
	void set_type(type tt) {t = tt;}
	void set_transparency(double tr) {transparency = tr;}
	type get_type() const {return t;}
	void getRGB(double *R, double *G, double *B) const;
	void getCMYK(double *C, double *M, double *Y, double *K) const;
	double getT() const {return transparency;}
	void setT(double tr) { transparency = tr;}

	double getR() const {return a;}
	double getG() const {return b;}
	double getB() const {return c;}

	double getH() const {return a;}
	double getS() const {return b;}
	double getV() const {return c;}

	double getC() const {return a;}
	double getM() const {return b;}
	double getY() const {return c;}
	double getK() const {return d;}
	std::string get_hexcolor() const;
protected:
	type t;
	double transparency;	// transparency
	double a;		// red, hue, or cyan
	double b;		// green, saturation, or magenta
	double c;		// blue, brightness, or yellow
	double d;		// k=blackness
};
class GriNamedColor : public GriColor
{
public:
	GriNamedColor();
	~GriNamedColor();
	GriNamedColor(const char *n, double R, double G, double B);
	GriNamedColor(const GriNamedColor& C); 
	GriNamedColor& operator=(const GriNamedColor& C);
	void setNameRGB(const char *n, double R, double G, double B);
	const std::string get_name(void) const {return name;}
private:
	std::string name;
};
#endif
