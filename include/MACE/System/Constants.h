/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#define MACE_INCLUDED true //this will be true if MACE is included

#ifndef __cplusplus 
	#error A C++ compiler is required!
#endif 

#if DEBUG==true || _DEBUG==true
#define MACE_ERROR_CHECK
#endif

#include <cstdint>


namespace mc {
	/**
	Primitive type guaranteed to be equal to 8 bits (1 byte). The definition here is for clarity.
	*/
	using Byte = uint8_t;

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
	*/
	using Size = unsigned int;

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
	*/
	using Index = unsigned int;

	using Enum = unsigned int;

	typedef void(*VoidFunctionPtr)();
}