/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__MACE_H
#define MACE__MACE_H

#include <MACE/Core/Core.h>
#include <MACE/Utility/Utility.h>
#include <MACE/Graphics/Graphics.h>

/**
Namespace for everything in MACE. This includes constants, typedefs, tests, classes, and variables.
<p>
All macros used by MACE are prefixed with `MACE_`. Internal macros which should not be used are prefixed with `MACE__`
<p>
No symbol in `mc` conflict directly with symbols found in `std` (C++11) or any dependency in MACE.
<p>
It is usually safe to type `using namespace mc`. However, be weary that this may create name conflicts with external libraries.

@note No MACE header file will bring all of `mc` into the global namespace.
@todo Add a proper logger
*/
namespace mc {

	//the following functions are for use with runtime link libraries to verify the MACE version
	//they have a strange naming convention because name mangling is disabled and there has to be an
	//easy conflict-free way to access these functions
	extern "C" {
		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR unsigned int getMACEVersionMajor() {
			return MACE_VERSION_MAJOR;
		}

		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR unsigned int getMACEVersionMinor() {
			return MACE_VERSION_MINOR;
		}

		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR unsigned int getMACEVersionPatch() {
			return MACE_VERSION_PATCH;
		}

		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR const char* getMACEVersionString() {
			return MACE_STRINGIFY(MACE_VERSION);
		}

		MACE_FUNCTION_EXPORT inline MACE_CONSTEXPR bool getMACEDebugBuild() {
#ifdef MACE_DEBUG
			return true;
#else
			return false;
#endif
		}
	}
}

#endif