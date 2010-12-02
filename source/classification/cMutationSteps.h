/*
 *  cMutationSteps.h
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#ifndef cMutationSteps_h
#define cMutationSteps_h

#ifndef cString_h
#include "cString.h"
#endif

#ifndef tSmartArray_h
#include "tSmartArray.h"
#endif


class cMutationSteps
{
private:
  struct sMutStep {
    enum {SUB_STEP, IN_STEP, DEL_STEP, SLIP_STEP} step_type;
    union {
      struct {
        int pos;
        char from;
        char to;
      } sub;
      struct {
        int pos;
        char inst;
      } indel;
      struct {
        int start;
        int end;
      } slip;
    };
    cString AsString() const;
  };
  
  tSmartArray<sMutStep> m_steps;
  cString m_loaded_string;
  
public:
  cMutationSteps() : m_steps(0) { ; }
  cMutationSteps(const cMutationSteps& in_ms) : m_steps(in_ms.m_steps) { ; }
  void operator=(const cMutationSteps& in_ms) { m_steps = in_ms.m_steps; }
  ~cMutationSteps() { ; }
  void Clear() { m_steps.Resize(0); }  

  void AddSubstitutionMutation(int _pos, char _from, char _to);
  void AddDeletionMutation(int _pos, char _from);
  void AddInsertionMutation(int _pos, int _to);
  void AddSlipMutation(int _start, int _end);

  void Set(const cString& in_mut_steps) { m_loaded_string = in_mut_steps; }
  // need to implement for reading, right now, we fake it.
  cString AsString() const;
};


#endif
