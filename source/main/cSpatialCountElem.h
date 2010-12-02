/*
 *  cSpatialCountElem.h
 *  Avida
 *
 *  Called "spatial_count_elem.hh" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#ifndef cSpatialCountElem_h
#define cSpatialCountElem_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cSpatialCountElem
{
private:
  mutable double amount, delta, initial;
  tArray<int> elempt, xdist, ydist;
  tArray<double> dist;
  
public:
  cSpatialCountElem();
  cSpatialCountElem(double initamount);
  
  void Rate(double ratein) const { delta += ratein; }
  void State() { amount += delta; delta = 0.0; }
  double GetAmount() const { return amount; }
  void SetAmount(double res) const { amount = res; }
  void SetPtr(int innum, int inelempt, int inxdist, int  inydist, double indist);
  int GetElemPtr(int innum) { return elempt[innum]; }
  int GetPtrXdist(int innum) { return xdist[innum]; }
  int GetPtrYdist(int innum) { return ydist[innum]; }
  double GetPtrDist(int innum) { return dist[innum]; }
  friend void FlowMatter(cSpatialCountElem&, cSpatialCountElem&, double, double, double, double,
                         int, int, double);
  void SetInitial(double init) { initial = init; }
  double GetInitial() { return initial; }
  
  inline void ResetResourceCount(double res_initial) { amount = res_initial + initial; }
};

#endif
