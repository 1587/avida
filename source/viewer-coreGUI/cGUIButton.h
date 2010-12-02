/*
 *  cGUIButton.h
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

// This is a base class for all GUI widgets that act as buttons.

#ifndef cGUIButton_h
#define cGUIButton_h

#include "cGUIWidget.h"

class cGUIButton : public cGUIWidget {
public:
  enum eButtonType { BUTTON_NORMAL, BUTTON_TOGGLE, BUTTON_RADIO, BUTTON_LIGHT,
		     BUTTON_RADIO_LIGHT, BUTTON_RETURN, BUTTON_REPEAT };

protected:
  int m_type;    // What type of button is this?
  int m_binding; // What keypress is this button bound to?

public:
  cGUIButton(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="",
	     eButtonType type=BUTTON_NORMAL)
    : cGUIWidget(parent, x, y, width, height, name), m_type(type) { ; }
  virtual ~cGUIButton() { ; }

  virtual void Press() = 0;
  virtual void BindKey(int key) = 0;

  int GetType() const { return m_type; }
  int GetBinding() const { return m_binding; }
};

#endif
