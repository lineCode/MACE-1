/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.h>
#include <MACE/Utility/DynamicLibrary.h>

namespace mc {
	extern "C" MACE_FUNCTION_EXPORT int exportTest(int returnValue) {
		return returnValue;
	}

	typedef int(*ExportTestPtr)(int);

	//this macro is defined when the file is compiled as a dynamic library
#ifndef MACE_DLL_OUT_PASS
	TEST_CASE("Testing loading dynamic libraries from file system", "[utility][dynamiclibrary]") {
		//for this demo, this source file is compiled twice: one as part as the testing executable, and one
		//as the platform specific shared library. cmake handles all of that. MACE_DLL_TEST_OUTPUT is defined
		//by cmake to wherever the library is compiled to.
		DynamicLibrary dllTest = DynamicLibrary(MACE_DLL_TEST_OUTPUT);

		REQUIRE(dllTest.isCreated());

		ExportTestPtr exportTestFunc = reinterpret_cast<ExportTestPtr>(dllTest.getFunction("exportTest"));

		REQUIRE(exportTestFunc(42) == 42);

		REQUIRE(dllTest.isCreated());

		dllTest.destroy();

		REQUIRE(!dllTest.isCreated());
	}
#endif//MACE_DLL_OUT_PASS
}//mc
