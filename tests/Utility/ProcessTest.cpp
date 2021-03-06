/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#ifdef MACE_PROC_OUT_PASS
int main() {
	//5 is a nice number to return because its unsigned and not 1 or 0
	return 5;
}
#else//MACE_PROC_OUT_PASS
#include <Catch.hpp>
#define MACE_EXPOSE_WINAPI
#define MACE_EXPOSE_POSIX
#include <MACE/Utility/Process.h>
#include <cstring>

namespace mc {
	TEST_CASE("Testing Process class", "[utility][process]") {
		Process p = Process(MACE_PROC_TEST_OUTPUT, "");

		REQUIRE(std::strcmp(p.getPath(), MACE_PROC_TEST_OUTPUT) == 0);
		REQUIRE(std::strcmp(p.getArgs(), "") == 0);

		REQUIRE_FALSE(p.isRunning());

		REQUIRE_FALSE(p.isCreated());

		p.init();

		REQUIRE(p.isRunning());

		REQUIRE(p.isCreated());

		REQUIRE(p.wait() == 5);

		REQUIRE_FALSE(p.isRunning());

		REQUIRE(p.isCreated());

		p.destroy();

		REQUIRE_FALSE(p.isCreated());
		REQUIRE_FALSE(p.isRunning());
	}
}//mc
#endif
