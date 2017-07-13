﻿/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__CORE_CONSTANTS_H
#define MACE__CORE_CONSTANTS_H

#include <MACE/Configure.h>

#ifndef __cplusplus
#	error A C++ compiler is required!
#endif//__cplusplus

//checking if MACE should be compiled in debug mode
#ifdef MACE_DEBUG
#	if MACE_DEBUG == 0 || MACE_DEBUG == false
#		undef MACE_DEBUG
#	endif//MACE_DEBUG == 0
#elif !defined(MACE_DEBUG)&&(defined(DEBUG) || (defined(_DEBUG)) || !defined(NDEBUG) || defined(MACE_DOXYGEN_PASS))
#	define MACE_DEBUG 1
#endif//elif

//defining keywords defining function symbol export and import
#if !defined(MACE_FUNCTION_EXPORT)&&!defined(MACE_FUNCTION_IMPORT)
#	ifdef MACE_WINDOWS
#		ifdef MACE_GNU
#			define MACE_FUNCTION_EXPORT __attribute__((dllexport))
#			define MACE_FUNCTION_EXPORT __attribute__((dllimport))
#		else
#			define MACE_FUNCTION_EXPORT __declspec(dllexport)
#			define MACE_FUNCTION_IMPORT __declspec(dllimport)
#		endif
#	else
#		define MACE_FUNCTION_EXPORT __attribute__((visibility("default")))
#		define MACE_FUNCTION_IMPORT __attribute__((visibility("hidden")))
#	endif
#endif

//checks for a C++ attribute in the form of [[attribute]]
#ifndef MACE_HAS_ATTRIBUTE
#	ifndef __has_cpp_attribute
#		define MACE_HAS_ATTRIBUTE(attr) 0
#	else
#		define MACE_HAS_ATTRIBUTE(attr) __has_cpp_attribute(attr)
#	endif
#endif

//[[fallthrough]] attribute
#ifndef MACE_FALLTHROUGH
#	if MACE_HAS_ATTRIBUTE(fallthrough)
#		define MACE_FALLTHROUGH [[fallthrough]]
#	else
#		define MACE_FALLTHROUGH
#	endif
#endif

//whether the specificed include is available
#ifndef MACE_HAS_INCLUDE
#	ifndef __has_include
#		define MACE_HAS_INCLUDE(incl) 0
#	else
#		define MACE_HAS_INCLUDE(incl) __has_include(incl)
#	endif
#endif

//whether opencv interoptibility should be built
#ifdef MACE_OPENCV
#	if MACE_OPENCV == 0 || MACE_OPENCV == false
#		undef MACE_OPENCV
#	endif
//if doxygen is running or opencv is detected, set MACE_OPENCV to 1 if it hasnt been defined already
#elif MACE_HAS_INCLUDE(<opencv2/opencv.hpp>)||defined(MACE__DOXYGEN_PASS)||(defined(CV_VERSION) && defined(CV_VERSION_MINOR) && defined(CV_VERSION_MINOR))
#	define MACE_OPENCV 1
#endif

//constexpr
#ifndef MACE_CONSTEXPR
#	if defined(__cpp_constexpr) && __cpp_constexpr >= 200704
#		define MACE_CONSTEXPR constexpr
#	else
#		define MACE_CONSTEXPR
#	endif
#endif

//static assert
//allows users to use a different static assert (such as boost's static asssert)
#ifndef MACE_STATIC_ASSERT
#	define MACE_STATIC_ASSERT(cond, message) static_assert( cond , message )
#endif

#define MACE_STRINGIFY(name) #name
#define MACE_STRINGIFY_NAME(name) "" #name
#define MACE_STRINGIFY_DEFINITION(name) "" MACE_STRINGIFY(name)

//meaning doxygen is currently parsing this file
#ifdef MACE__DOXYGEN_PASS
#	define MACE_EXPOSE_ALL 1
#endif

#ifdef MACE_EXPOSE_ALL
#	define MACE_EXPOSE_WINAPI 1
#	define MACE_EXPOSE_POSIX 1
#	define MACE_EXPOSE_OPENGL 1
#	define MACE_EXPOSE_GLFW 1
#	define MACE_EXPOSE_OPENAL 1
#endif

//for std::uint8_t
#include <cstdint>
//for std::size_t
#include <cstddef>

namespace mc {
	/**
	Primitive type guaranteed to be equal to 8 bits (1 byte). The definition here is for clarity.
	@see VoidFunctionPtr
	@see Index
	@see Enum
	@see Size
	*/
	using Byte = std::uint8_t;

	/**
	Type representing an object's size.
	<p>
	The definition here is for clarity, as it makes code more self documenting. Imagine this:
	{@code
		unsigned int getSize()
	}
	By using this definition, that code becomes:
	{@code
		Size getSize()
	}
	Not only is it shorter, but it makes it more obvious, making documentation easier.
	@see Index
	@see Enum
	@see VoidFunctionPtr
	*/
	using Size = std::size_t;

	/**
	Type representing an index in an array.
	<p>
	The definition here is for clarity, and it makes code more obvious and self-documenting. Imagine this:
	{@code
		removeObject(unsigned int)
	}
	or
	{@code
		getObject(unsigned int)
	}
	`unsigned int` is ambigious! What if the object is an `unsigned int`? With this definition, it becomes much more clear:
	{@code
		removeObject(Index)

		getObject(Index)
	}

	@see Size
	@see VoidFunctionPtr
	@see Enum
	*/
	using Index = Size;
	/**
	Type representing an int-based enum (such as GL_* constants)
	@see Index
	@see VoidFunctionPtr
	@see Size
	*/
	using Enum = unsigned int;

	/**
	Type representing a function that returns void and has no arguments.
	<p>
	Created for clarity and for use in callbacks
	@see Index
	@see Size
	@see Enum
	*/
	typedef void(*VoidFunctionPtr)();
}

#endif//MACE_CORE_CONSTANTS_H
