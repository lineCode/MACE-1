/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__UTILITY_DYNAMIC_LIBRARY_H
#define MACE__UTILITY_DYNAMIC_LIBRARY_H

#include <MACE/Core/Constants.h>

#include <string>

namespace mc {
	class DynamicLibrary {
	public:
		static DynamicLibrary getRunningProcess();

		~DynamicLibrary();
		DynamicLibrary();
		DynamicLibrary(const std::string& path);
		DynamicLibrary(const char* path);

		void init(const std::string& path);
		void init(const char* path);

		void destroy();

		void* getFunction(const std::string& name);
		void* getFunction(const char* name);

		void* operator[](const char* name) {
			return getFunction(name);
		}

		void* operator[](const std::string& name) {
			return this->operator[](name.c_str());
		}

		bool isCreated() const;

#if defined(MACE_WINAPI)&&defined(MACE_EXPOSE_WINAPI)
		void* getHandle() const {
			return dll;
		}
#elif defined(MACE_POSIX)&&defined(MACE_EXPOSE_POSIX)
		void* getDescriptor() const {
			return dll;
		}
#endif
	private:
		bool created = false;

		void* dll;
	};//DynamicLibrary
}//mc

#endif//MACE__UTILITY_DYNAMIC_LIBRARY_H
