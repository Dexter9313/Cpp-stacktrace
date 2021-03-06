/*
    Copyright (C) 2017 Florian Cabot

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef EXCEPTIONS
#define EXCEPTIONS

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <execinfo.h>
#include <iostream>
#include <sstream>
#include <stdint.h>

/*! \ingroup exceptions
 * Prints stack trace and throws a critical exception.
 *
 * This exception will be automatically catched within the BEGIN_EXCEPTIONS and
 * END_EXCEPTIONS macros. These macros will ensure the displaying of a helpful
 * debug message and will exit the program with the error code EXIT_FAILURE.
 * The debug message contains three automatically-filled informations and a
 * custom string. These informations are : the function name of the function
 * being called, the file name which contains the function and the line number
 * within the file where Critical has been called.
 * \param str The custom string to be displayed for debugging.
 */
#define CRITICAL(str)                                                \
	{                                                                \
		print_stacktrace(0);                                         \
		throw(CriticalException(str, __func__, __FILE__, __LINE__)); \
	}

#define MAX_BACKTRACE_LINES 64

/*! \ingroup exceptions
 * Initializes the critical exceptions handling.
 *
 * This exceptions system prints automatically stack traces whenever a critical
 * signal is sent, whether it is from the system (SIG_SEGV for example) or by
 * the program itself through the #CRITICAL macro.
 *
 * Because of the way it handles system signals, it is also currently not
 * possible to use the signal() function to redefine system signal handlers. If
 * you wish to do specific things on system signal capture, please edit the
 * signal handler defined in the Exceptions.hpp file to do it there. It is
 * expected that some additional functions or macros will be written to allow
 * system signals handling anywhere else in the code.
 *
 * This macro has to be put within the main function together with the
 * END_EXCEPTIONS macro and has to be called only once in the whole program. It
 * will assume the argv parameter is passed to main and use it automatically.
 * To be more precise, it will start a try{} block which has to be closed with
 * the END_EXCEPTIONS macro, which holds the corresponding closing bracket and
 * the catch{} block.
 */
#define BEGIN_EXCEPTIONS                                           \
	init_exceptions(argv[0]); /*NOLINT complaining about argv[0]*/ \
	try                                                            \
	{
/*! \ingroup exceptions
 * Closes the critical exceptions handling.
 *
 * This exceptions system prints automatically stack traces whenever a critical
 * signal is sent, whether it is from the system (SIG_SEGV for example) or by
 * the program itself through the #CRITICAL macro.
 *
 * Because of the way it handles system signals, it is also currently not
 * possible to use the signal() function to redefine system signal handlers. If
 * you wish to do specific things on system signal capture, please edit the
 * signal handler defined in the Exceptions.hpp file to do it there. It is
 * expected that some additional functions or macros will be written to allow
 * system signals handling anywhere else in the code.
 *
 * This macro has to be put within the main function together with the
 * END_EXCEPTIONS macro and has to be called only once in the whole program. To
 * be more precise, it will close the try{} block opened by the BEGIN_EXCEPTIONS
 * macro.
 */
#define END_EXCEPTIONS                       \
	}                                        \
	catch(CriticalException & exception)     \
	{                                        \
		std::cerr << exception << std::endl; \
		exit(EXIT_FAILURE);                  \
	}

class Exceptions
{
  public:
	static char*& getProgramName()
	{
		static char* _programName;
		return _programName;
	}
};

void print_stacktrace(int calledFromSigInt);
void posix_signal_handler(int sig);
void set_signal_handler(sig_t handler);
void init_exceptions(char* programName);
int addr2line(char const* const program_name, void const* const addr,
              int lineNb);

// prints formated stack trace with most information as possible
// parameter indicates if the function is called by the signal handler or not
//(to hide the call to the signal handler)
inline void print_stacktrace(int calledFromSigInt)
{
	void* buffer[MAX_BACKTRACE_LINES];
	char** strings;

	int nptrs = backtrace(buffer, MAX_BACKTRACE_LINES);
	strings   = backtrace_symbols(buffer, nptrs);

	if(strings == NULL)
	{
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}

	int i = 1;

	if(calledFromSigInt != 0)
		++i;

	for(; i < nptrs - 2; ++i)
	{
		// if addr2line failed, print what we can
		if(addr2line(Exceptions::getProgramName(), buffer[i], nptrs - 2 - i - 1)
		   != 0)
			std::cerr << "[" << nptrs - 2 - i - 1 << "] " << strings[i]
			          << std::endl;
	}

	free(strings);
}

inline void posix_signal_handler(int sig)
{
	print_stacktrace(1);

	switch(sig)
	{
		case SIGABRT:
			std::cerr
			    << "Caught SIGABRT: usually caused by an abort() or assert()"
			    << std::endl;
			break;

		case SIGFPE:
			std::cerr
			    << "Caught SIGFPE: arithmetic exception, such as divide by zero"
			    << std::endl;
			;
			break;

		case SIGILL:
			std::cerr << "Caught SIGILL: illegal instruction" << std::endl;
			;
			break;

		case SIGINT:
			std::cerr << "Caught SIGINT: interactive attention signal, "
			             "probably a ctrl+c"
			          << std::endl;
			;
			break;

		case SIGSEGV:
			std::cerr << "Caught SIGSEGV: segfault" << std::endl;
			;
			break;

		case SIGTERM:
		default:
			std::cerr << "Caught SIGTERM: a termination request was sent to "
			             "the program"
			          << std::endl;
			;
			break;
	}

	_Exit(EXIT_FAILURE);
}

inline void set_signal_handler(sig_t handler)
{
	signal(SIGABRT, handler);
	signal(SIGFPE, handler);
	signal(SIGILL, handler);
	signal(SIGINT, handler);
	signal(SIGSEGV, handler);
	signal(SIGTERM, handler);
}

// lib activation, first thing to do in main
// programName should be argv[0]
inline void init_exceptions(char* programName)
{
	set_signal_handler(posix_signal_handler);
	Exceptions::getProgramName() = programName;
}

/* Resolve symbol name and source location given the path to the executable
   and an address */
// returns 0 if address has been resolved and a message has been printed; else
// returns 1
inline int addr2line(char const* const program_name, void const* const addr,
                     int lineNb)
{
	char addr2line_cmd[512] = {0};

/* have addr2line map the address to the relent line in the code */
#ifdef __APPLE__
	/* apple does things differently... */
	sprintf(addr2line_cmd, "atos -o %.256s %p", program_name, addr);
#else
	sprintf(addr2line_cmd, "addr2line -C -f -e %.256s %p", program_name, addr);
#endif

	/* This will print a nicely formatted string specifying the
	function and source line of the address */

	FILE* fp;
	char outLine1[1035];
	char outLine2[1035];

	/* Open the command for reading. */
	fp = popen(addr2line_cmd, "r");

	if(fp == NULL)
		return 1;

	while(fgets(outLine1, sizeof(outLine1) - 1, fp) != NULL)
	{
		// if we have a pair of lines
		if(fgets(outLine2, sizeof(outLine2) - 1, fp) != NULL)
		{
			// if symbols are readable
			if(outLine2[0] != '?')
			{
				// only let func name in outLine1
				int i;

				for(i = 0; i < 1035; ++i)
				{
					if(outLine1[i] == '\r' || outLine1[i] == '\n')
					{
						outLine1[i] = '\0';
						break;
					}
				}

				// don't display the whole path
				int lastSlashPos = 0;

				for(i = 0; i < 1035 && outLine2[i] != '\0'; ++i)
				{
					if(outLine2[i] == '/')
						lastSlashPos = i + 1;
				}

				std::cerr << "[" << lineNb << "] " << addr << " in " << outLine1
				          << " at " << outLine2 + lastSlashPos << std::flush;
			}
			else
			{
				pclose(fp);
				return 1;
			}
		}
		else
		{
			pclose(fp);
			return 1;
		}
	}

	/* close */
	pclose(fp);

	return 0;
}

/*! Exception to be thrown by the CRITICAL macro
 *
 * It is not intended to be thrown by the user, even if he could in theory. The
 * prefered way to use it
 * is through the CRITICAL macro.
 * It contains an error message, the name of the function that threw it, the
 * name of the file that contains
 * the throw statement and the line at which this throw statement can be found.
 */
class CriticalException
{
  private:
	std::string message;
	std::string funcName;
	std::string file;
	int line;

	static std::string itos(int i)
	{
		std::ostringstream oss;
		oss << i;
		return oss.str();
	}

  public:
	/*! Constructor
	 *
	 * \param message Custom message that should be displayed.
	 * \param
	 */
	CriticalException(std::string message, std::string funcName,
	                  std::string file, int line)
	    : message(message)
	    , funcName(funcName)
	    , file(file)
	    , line(line)
	{
	}
	std::string toStr() const

	{
		return message + " (in " + funcName + " at " + file + ":" + itos(line)
		       + ")";
	}
};

inline std::ostream& operator<<(std::ostream& stream,
                                CriticalException const& exception)
{
	return stream << exception.toStr();
}

#endif
