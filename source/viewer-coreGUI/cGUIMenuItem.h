/*
 *  cGUIMenuItem.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

// A base clasee for single menu items in the GUI.

#ifndef cGUIMenuItem_h
#define cGUIMenuItem_h

class cGUIMenuItem {
protected:
  cString m_name;

public:
  cGUIMenuItem(const cString & name="") : m_name(name) { ; }
  virtual ~cGUIMenuItem() { ; }
  
  const cString & GetName() const { return m_name; }
  void SetName(const cString & name) { m_name = name; }

  virtual void Trigger() = 0;
};

#endif
