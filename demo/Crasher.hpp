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

#ifndef CRASHER
#define CRASHER

#include <cstdlib>
#include <cassert>
#include <csignal>
#include <ctime>

enum CrashType
{
	DIV_ZERO, SEGFAULT, ASSERT, ILLEGAL_INST, STACK_OVERFLOW
};

void stack_overflow()
{
	stack_overflow();
}

class Crasher
{
	static bool created;
	public:
		static void generateRandomCrash()
		{
			if(!created)
			{
				srand(time(NULL));
				created = true;
			}

			int r = rand() % 5;

			CrashType type;
			int i,j,k,p2;
			int*p;
			switch(r)
			{
				case 0:
					type = DIV_ZERO;
					break;
				case 1:
					type = SEGFAULT;
					break;
				case 2:
					type = ASSERT;
					break;
				case 3:
					type = ILLEGAL_INST;
					break;
				case 4:
					type = STACK_OVERFLOW;
					break;
			}

			switch(type)
			{
				case DIV_ZERO:
					i = 1; j=0; k=i/j;
					break;
				case SEGFAULT:
					p = NULL; p2 = *p;
					break;
				case ASSERT:
					assert(false);
					break;
				case ILLEGAL_INST:
					raise(SIGILL);
					break;
				case STACK_OVERFLOW:
					stack_overflow();
			}
		}

	private:
		Crasher();
		virtual ~Crasher();
};

bool Crasher::created = false;


#endif

