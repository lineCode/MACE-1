/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.hpp>
#include <MACE/Utility/Math.h>


namespace mc {
	namespace math {
		TEST_CASE("Testing isEven() and isOdd()") {
			SECTION("isEven()") {
				REQUIRE(isEven(4));
				REQUIRE(!isEven(5));
				REQUIRE(isEven(0));
				REQUIRE(!isEven(-1));
				REQUIRE(isEven(-2));
				REQUIRE(isEven(10));
			}
			SECTION("isOdd()") {
				REQUIRE(isOdd(3));
				REQUIRE(isOdd(1));
				REQUIRE(!isOdd(0));
				REQUIRE(isOdd(-3));
				REQUIRE(!isOdd(-4));
			}
		}

		TEST_CASE("Testing sqr()", "[utility][math]") {
			REQUIRE(math::sqr(2) == 4);
			REQUIRE(math::sqr(3) == 9);
			REQUIRE(math::sqr(4) == 16);
			REQUIRE(math::sqr(5) == 25);
			REQUIRE(math::sqr(6) == 36);
		}

		TEST_CASE("Testing cube()", "[utility][math]") {
			REQUIRE(math::cube(2) == 8);
			REQUIRE(math::cube(3) == 27);
			REQUIRE(math::cube(4) == 64);
			REQUIRE(math::cube(5) == 125);
			REQUIRE(math::cube(6) == 216);
		}

		TEST_CASE("Testing ceil()", "[utility][math]") {
			REQUIRE(math::ceil(5.9) == 6);
			REQUIRE(math::ceil(2.1) == 3);
			REQUIRE(math::ceil(3) == 3);
		}

		TEST_CASE("Testing abs()", "[utility][math]") {
			REQUIRE(math::abs(5) == 5);
			REQUIRE(math::abs(-5) == 5);
			REQUIRE(math::abs(4.2) == 4.2);
			REQUIRE(math::abs(-4.2) == 4.2);
		}

		TEST_CASE("Testing floor()", "[utility][math]") {
			REQUIRE(math::floor(6.9) == 6);
			REQUIRE(math::floor(1.6) == 1);
			REQUIRE(math::floor(4.7) == 4);
			REQUIRE(math::floor(5) == 5);
			REQUIRE(math::floor(-5.7) == -6);
		}

		TEST_CASE("Testing radians and degrees conversion") {
			SECTION("Radians to degrees") {
				REQUIRE(toDegrees(1.0) == Approx(57.2958));
				REQUIRE(toDegrees(2.0f) == Approx(114.592f));
				REQUIRE(toDegrees(1.5) == Approx(85.9437));
				REQUIRE(toDegrees(tau()) == Approx(360.0l));
				REQUIRE(toDegrees(6.0) == Approx(343.775));
				REQUIRE(toDegrees(-1.0f) == Approx(-57.2958));
			}
			SECTION("Degrees to radians") {
				REQUIRE(toRadians(360.0l) == Approx(tau()));
				REQUIRE(toRadians(180.0l) == Approx(pi()));
				REQUIRE(toRadians(200.0) == Approx(3.49066));
				REQUIRE(toRadians(90.0f) == Approx(1.5708f));
				REQUIRE(toRadians(0) == Approx(0));
				REQUIRE(toRadians(400.0) == Approx(6.98132));
			}
		}
	}
}