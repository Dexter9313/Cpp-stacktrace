/*
        Copyright (C) 2016 Florian Cabot

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

#include "../Cpp-stacktrace.hpp"
#include "Crasher.hpp"
#include <iostream>

class A
{
  public:
	void Test()
	{
		foo(true);
		bar(true);
	}

	void foo(bool crash)
	{
		if(crash)
		{
			int i = 0;
			CRITICAL("Foo crashed");
			i = 1;
		}
	}

	void bar(bool crash)
	{
		if(crash)
			CRITICAL("Bar crashed");
	}
};

int main(int argc, char* argv[])
{
	BEGIN_EXCEPTIONS
	A a;
	Crasher::generateRandomCrash();
	a.Test();
	END_EXCEPTIONS
	exit(EXIT_SUCCESS);
}
