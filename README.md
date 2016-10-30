# Cpp-stacktrace
Cpp-stacktrace is a small header C++ library to print stack trace on execution if an exception occurs (such as segmentation fault). It is designed to print useful information on exceptions while the program was used or tested outside of a debugger and remove the hassle to relaunch the program and pray for it to crash again in a debugger to find the problem. With a few more features it could be extended to be a useful crash-report generator to be sent by the user to the developer. The development of such a feature is not yet seriously considered but could very well be in the near future.

# Disclaimer

This library and its documentation are currently in an experimental state. It has been published fairly recently and has yet to be improved and fully tested. It is not designed to be as much portable as possible yet and has been developed and light-tested for linux-based operating systems for now. It is by far not considered to be bug free. Please feel free to report issues or requests and/or to contribute to improve it.

# Installation
Just add the *Cpp-stacktrace.hpp* header file to your project.
Make sure addr2line (atos for Mac OS) is installed on the target system (the system executing the program) for nice class/methods printing.

# Usage

`#include <Cpp-stacktrace.hpp>` in your *main.cpp* file.

Put the macros `BEGIN_EXCEPTIONS` and `END_EXCEPTIONS` around the code you wish to generate dynamic stacktraces for as so :

```c++
#include <Cpp-stacktrace.hpp>

int main(int argc, char* argv[])
{
	BEGIN_EXCEPTIONS
	//code
	END_EXCEPTIONS
	//if you don't want to generate a warning, you should put exit outside the BEGIN/END block
	exit(EXIT_SUCCESS);
}
```

Make sure main takes the `argv` parameter as it is used in the `BEGIN_EXCEPTIONS` macro.

Anywhere within this block, you can use the macro `CRITICAL` to generate a critical issue that will terminate the program and print your message along with the stacktrace and the exact location within a source file where the macro was called.
Ex:
```c++
#include <Cpp-stacktrace.hpp>

int main(int argc, char* argv[])
{
	BEGIN_EXCEPTIONS
	CRITICAL("There has been a critical error !");
	END_EXCEPTIONS
	//if you don't want to generate a warning, you should put exit outside the BEGIN/END block
	exit(EXIT_SUCCESS);
}
```
Outputs :

```
[0] 0x402247 in main at main.cpp:6 (discriminator 1)
There has been a critical error ! (in main at main.cpp:6)
```

You can read the small demo to have a better understanding on how to use this library. As stated in the disclaimer, a better documentation will come.
