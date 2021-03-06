/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Utility/Signal.h>

#include <iostream>
#include <csignal>

namespace mc {
	namespace os {
		namespace {
			void signalHandle[[noreturn]](int sig) {
				//the following code is pretty boring and self explanatory.
				//plus signals are literally the devil to debug, especially SIGSEGV
				//so to lighten up the mood for developers that encounter these signals,
				//i've written a joke about every single one
				try{
#ifdef MACE_POSIX
					if (sig == SIGHUP) {
						//no YOU hangup first!
						throw SignalHangupError("SIGHUP: Hangup detected on controlling terminal or death of controlling process");
					} else if (sig == SIGKILL) {
						//RIP program. may it forever live on in /dev/null
						throw SignalKillError("SIGKILL: Program was killed");
					} else if (sig == SIGSTOP) {
						//stop, hammertime
						throw SignalStopError("SIGSTOP: Program was stopped");
					} else if (sig == SIGALRM) {
						//the terminal's form of birth control
						throw SignalAlarmError("SIGALRM: Abort signal from alarm()");
					} else if (sig == SIGTSTP) {
						//ive got a terminal illness
						throw SignalTerminalStopError("SIGTSTP: Stop was typed in the terminal");
					} else if (sig == SIGTTIN) {
						//the adventures of TTIN TTIN
						throw SignalTerminalInputError("SIGTTIN: Terminal input for background process");
					} else if (sig == SIGTTOU) {
						//
						throw SignalTerminalOutputError("SIGTTOU: Terminal output for background process");
					}
#endif//MACE_POSIX

					if (sig == SIGABRT
#ifdef MACE_WINDOWS
						//SIBABRT_COMPAT is the same as SIGABRT on Windows platforms.
						//It exists for compatibilty with POSIX platforms
						|| sig == SIGABRT_COMPAT
#endif//MACE_WINDOWS
						) {
						//conservatives hate SIGABRT
						throw SignalAbortError("SIGABRT: Program was aborted");
					} else if (sig == SIGFPE) {
						//i was just trying to make a point
						throw SignalFloatingPointError("SIGFPE: A floating point error occured");
					} else if (sig == SIGILL) {
						//oh dear, the program seems quite ill
						throw SignalIllegalInstructionError("SIGILL: An illegal instruction occured");
					} else if (sig == SIGINT) {
						//its rude to interuppt
						throw SignalInterruptError("SIGINT: Program was interrupted from keyboard");
					} else if (sig == SIGSEGV) {
						//now to whip out the ol' gdb and empty the next 3 days from the calendar
						throw SignalSegmentFaultError("SIGSEGV: Invalid memory reference (segmentation fault)");
					} else if (sig == SIGTERM) {
						//hasta la vista baby
						throw SignalTerminateError("SIGTERM: Program was terminated");
					} else {
						//we are getting mixed signals here
						throw SignalError("Program recieved signal " + std::to_string(sig));
					}
				}catch(const Error& err){
					Error::handleError(err);
				}
			}

			void onUnexpected[[noreturn]]() {
				try {
					Error::handleError(MACE__GET_ERROR_NAME(Unknown) ("An unexpected error occured", __LINE__, __FILE__));
				} catch (...) {
					std::cerr << "onUnexpected(): An unexpected error occured trying to show an unexpected error" << std::endl;
					//this function should never throw an error, ignore all errors and exit anyways
				}
				std::exit(EXIT_FAILURE);
			}

			void onTerminate[[noreturn]](){
				try {
					Error::handleError(MACE__GET_ERROR_NAME(Unknown) ("An exception was thrown somewhere and not caught appropriately", __LINE__, __FILE__));
				} catch (...) {
					std::cerr << "onTerminate(): An unexpected error occured trying to show an error" << std::endl;
					//this function should never throw an error, ignore all errors and exit anyways
				}
				std::exit(EXIT_FAILURE);
			}
		}

		void SignalModule::init() {
			std::signal(SIGABRT, &signalHandle);
			std::signal(SIGFPE, &signalHandle);
			std::signal(SIGILL, &signalHandle);
			std::signal(SIGINT, &signalHandle);
			std::signal(SIGSEGV, &signalHandle);
			std::signal(SIGTERM, &signalHandle);

#ifdef MACE_WINDOWS
			std::signal(SIGABRT_COMPAT, &signalHandle);
#elif defined(MACE_POSIX)
			std::signal(SIGHUP, &signalHandle);
			std::signal(SIGKILL, &signalHandle);
			std::signal(SIGSTOP, &signalHandle);
			std::signal(SIGALRM, &signalHandle);
			std::signal(SIGTSTP, &signalHandle);
			std::signal(SIGTTIN, &signalHandle);
			std::signal(SIGTTOU, &signalHandle);
#endif//MACE_POSIX
		}

		void SignalModule::update() {}

		void SignalModule::destroy() {
			std::signal(SIGABRT, SIG_DFL);
			std::signal(SIGFPE, SIG_DFL);
			std::signal(SIGILL, SIG_DFL);
			std::signal(SIGINT, SIG_DFL);
			std::signal(SIGSEGV, SIG_DFL);
			std::signal(SIGTERM, SIG_DFL);
#ifdef MACE_POSIX
			std::signal(SIGHUP, SIG_DFL);
			std::signal(SIGKILL, SIG_DFL);
			std::signal(SIGSTOP, SIG_DFL);
			std::signal(SIGALRM, SIG_DFL);
			std::signal(SIGTSTP, SIG_DFL);
			std::signal(SIGTTIN, SIG_DFL);
			std::signal(SIGTTOU, SIG_DFL);
#endif//MACE_POSIX
		}

		std::string SignalModule::getName() const {
			return "MACE/Signal";
		}

		void ErrorModule::init() {
			oldTerminate = std::set_terminate(&onTerminate);
			oldUnexpected = std::set_unexpected(&onUnexpected);

			SignalModule::init();
		}

		void ErrorModule::update() {
			SignalModule::update();
		}

		void ErrorModule::destroy() {
			SignalModule::destroy();

			std::set_terminate(oldTerminate);
			std::set_unexpected(oldUnexpected);
		}

		std::string ErrorModule::getName() const {
			return "MACE/Error";
		}
	}//os
}//mc
