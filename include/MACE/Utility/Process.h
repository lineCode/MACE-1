/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__UTILITY_PROCESS_H
#define MACE__UTILITY_PROCESS_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>

#include <string>

#ifdef MACE_POSIX
#	include <sys/types.h>
#endif

namespace mc {
	class Process: public Initializable {
	public:
		Process();
		Process(const char* path, const char* args);
		Process(const std::string& path, std::string& args);
		~Process();

		void init() override;
		void destroy() override;

		int wait();

		bool isRunning() const;
		bool isCreated() const;

		void setPath(const char* p);
		const char* getPath() const;

		void setArgs(const char* a);
		const char* getArgs() const;

#if defined(MACE_WINAPI) && defined(MACE_EXPOSE_WINAPI)
		void* getProcess() const;
		void* getThread() const;
#elif defined(MACE_POSIX) && defined(MACE_EXPOSE_POSIX)
		pid_t getPID() const;
#endif

	private:
		bool created = false;

		const char* path;
		const char* args;

#ifdef MACE_WINAPI
		void* process;
		void* thread;
#elif defined(MACE_POSIX)
		pid_t process;
#endif//MACE_POSIX
	};
}

#endif//MACE__UTILITY_PROCESS_H