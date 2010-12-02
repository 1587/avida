/*
 *  ASCoreLib.cc
 *  Avida
 *
 *  Created by David on 3/16/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ASCoreLib.h"

#include "cASLibrary.h"

#include <iostream>


namespace ASCoreLib {
  void print(const cString& value)
  {
    std::cout << value;
  }
  
  void println(const cString& value)
  {
    std::cout << value << std::endl;
  }
};


void RegisterASCoreLib(cASLibrary* lib)
{
  lib->RegisterFunction(new tASFunction<void (const cString&)>(&ASCoreLib::print, "print"));
  lib->RegisterFunction(new tASFunction<void (const cString&)>(&ASCoreLib::println, "println"));
}
