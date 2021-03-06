/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Color.h>

namespace mc {
	namespace {
		float trimFloat(const float& color) {
			return color < 0.0f ? 0.0f : (color > 1.0f ? 1.0f : color);
		}

		Byte convertFloatToRGBA(const float& color) {
			return static_cast<Byte>(trimFloat(color)*254.0f);
		}

		float convertRGBAToFloat(const Byte& color) {
			return color / 254.0f;

		}
	}//anon namespace

	namespace Colors {
		const Color RED = Color(200, 0, 0), LIGHT_RED = Color(255, 0, 0),
			DARK_BLUE = Color(0, 0, 255), BLUE = Color(0, 0, 200), CYAN = Color(0, 255, 255),
			LIGHT_BLUE = Color(50, 200, 255), DARK_RED = Color(150, 0, 0),
			DARK_GREEN = Color(0, 50, 0), GREEN = Color(0, 150, 0),
			LIGHT_GREEN = Color(0, 250, 0), PURPLE = Color(100, 0, 255),
			MAGENTA = Color(255, 0, 255), WHITE = Color(255, 255, 255),
			BLACK = Color(0, 0, 0), YELLOW = Color(255, 255, 0),
			DARK_GRAY = Color(100, 100, 100), LIGHT_GRAY = Color(200, 200, 200),
			ORANGE = Color(255, 125, 0), GRAY = Color(150, 150, 150),
			DARK_ORANGE = Color(255, 100, 0), LIGHT_ORANGE = Color(255, 150, 0),
			INVISIBLE = Color(0, 0, 0, 0);
	}

	Color Color::lighten() const {
		return Color(trimFloat(r + 0.05f), trimFloat(g + 0.05f), trimFloat(b + 0.05f), a);
	}

	Byte Color::getRed() const {
		return convertFloatToRGBA(this->r);
	}

	Byte Color::getGreen() const {
		return convertFloatToRGBA(this->g);
	}

	Byte Color::getBlue() const {
		return convertFloatToRGBA(this->b);
	}

	Byte Color::getAlpha() const {
		return convertFloatToRGBA(this->a);
	}

	void Color::setRed(const Byte& red) {
		this->r = convertRGBAToFloat(red);
	}

	void Color::setGreen(const Byte& green) {
		this->g = convertRGBAToFloat(green);

	}

	void Color::setBlue(const Byte& blue) {
		this->b = convertRGBAToFloat(blue);

	}

	void Color::setAlpha(const Byte& alpha) {
		this->a = convertRGBAToFloat(alpha);
	}

	Color::Color(const int red, const int green, const int blue, const int alpha) noexcept: Color(static_cast<Byte>(red), static_cast<Byte>(green), static_cast<Byte>(blue), static_cast<Byte>(alpha)) {}

	Color::Color(const Byte red, const Byte green, const Byte blue, const Byte alpha) noexcept {
		setRed(red);
		setGreen(green);
		setBlue(blue);
		setAlpha(alpha);
	}

	Color::Color(const float red, const float green, const float blue, const float alpha) noexcept:  r(red), g(green), b(blue), a(alpha) {}

	Color::Color(const std::array<float, 4>& rgba) {
		this->setValues(rgba);
	}

	Color::Color(const Color & copy) noexcept : Color(copy.r, copy.g, copy.b, copy.a) {}

	Color::Color() noexcept : Color(0.0f, 0.0f, 0.0f, 1.0f) {}

	Color Color::darken() const {
		return Color(trimFloat(r - 0.05f), trimFloat(g - 0.05f), trimFloat(b - 0.05f), a);
	}

	bool Color::operator==(const Color & other) const {
		return r == other.r&&g == other.g&&b == other.b&&a == other.a;
	}

	bool Color::operator!=(const Color & other) const {
		return !operator==(other);
	}

	void Color::setValues(const std::array<float, 3>& rgb) {
		this->r = rgb[0];
		this->g = rgb[1];
		this->b = rgb[2];
	}

	void Color::setValues(const std::array<float, 4>& rgba) {
		this->r = rgba[0];
		this->g = rgba[1];
		this->b = rgba[2];
		this->a = rgba[3];
	}

	std::array<float, 4> Color::getValues() const {
		return { { r,g,b,a } };
	}

	const float * Color::flatten(float arr[4]) const {
		arr[0] = r;
		arr[1] = g;
		arr[2] = b;
		arr[3] = a;

		return arr;
	}

	float * Color::begin() {
		return &r;
	}

	const float * Color::begin() const {
		return &r;
	}

	float * Color::end() {
		return &a + sizeof(float);
	}

	const float * Color::end() const {
		return &a + sizeof(float);
	}

	std::ostream & operator<<(std::ostream & output, const Color & v) {
		output << "Color(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a << ")";
		return output;
	}

	bool Color::operator<(const Color& other) const {
		//the real g right here
		return other.r < r && other.g < g && other.b < b && other.a < a;
	}

	bool Color::operator<=(const Color& other) const {
		return operator<(other) || operator==(other);
	}

	bool Color::operator>(const Color& other) const {
		//the real g right here
		return !operator<=(other);
	}

	bool Color::operator>=(const Color& other) const {
		//the real g right here
		return !operator<(other);
	}
}//mc
