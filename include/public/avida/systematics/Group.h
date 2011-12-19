/*
 *  systematics/Group.h
 *  avida-core
 *
 *  Created by David on 12/14/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaSystematicsGroup_h
#define AvidaSystematicsGroup_h

#include "apto/platform.h"
#include "avida/core/Types.h"
#include "avida/systematics/Types.h"


namespace Avida {
  namespace Systematics {
    
    // GroupData
    // --------------------------------------------------------------------------------------------------------------
    
    class GroupData
    {
    public:
      LIB_EXPORT virtual ~GroupData() = 0;
      
      LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const = 0;
    };
    
    
    // Group
    // --------------------------------------------------------------------------------------------------------------
    
    class Group : virtual public Apto::MTRefCountObject {
    protected:
      GroupID m_id;
      int m_a_refs;
      int m_p_refs;
      Apto::Map<Apto::String, Apto::SmartPtr<GroupData> > m_data;
      
    public:
      LIB_EXPORT inline Group(GroupID in_id) : m_id(in_id), m_a_refs(0), m_p_refs(0) { ; }
      LIB_EXPORT virtual ~Group() = 0;
      
      LIB_EXPORT virtual RoleID Role() const = 0;
      LIB_EXPORT inline GroupID ID() const { return m_id; }
      LIB_EXPORT virtual ArbiterPtr Arbiter() const = 0;
      
      LIB_EXPORT virtual GroupPtr ClassifyNewUnit(UnitPtr u, ConstGroupMembershipPtr parent_groups) = 0;
      LIB_EXPORT virtual void HandleUnitGestation(UnitPtr u) = 0;
      LIB_EXPORT virtual void RemoveUnit(UnitPtr u) = 0;
      
      LIB_EXPORT virtual int Depth() const = 0;
      LIB_EXPORT virtual int NumUnits() const = 0;
      
      LIB_EXPORT virtual ConstPropertyIDSetPtr PropertyIDs() const = 0;
      LIB_EXPORT virtual const PropertyMap& Properties() const = 0;
      
      LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const;
      
      
      // Reference Management (Active for currently living units, Passive for all other group usage)
      LIB_EXPORT inline int ReferenceCount() const { return m_a_refs + m_p_refs; }
      LIB_EXPORT inline int ActiveReferenceCount() const { return m_a_refs; }
      LIB_EXPORT inline int PassiveReferenceCount() const { return m_p_refs; }
      
      LIB_EXPORT virtual void AddActiveReference();
      LIB_EXPORT virtual void RemoveActiveReference();
      LIB_EXPORT virtual void AddPassiveReference();
      LIB_EXPORT virtual void RemovePassiveReference();

      
      // Group Data
      template <typename T> LIB_EXPORT bool AttachData(Apto::SmartPtr<T> obj)
      {
        Apto::String type_id_str(typeid(T).name());
        if (m_data.Has(type_id_str)) return false;
        m_data.Set(type_id_str, obj);
        return true;
      }
      
      template <typename T> LIB_EXPORT Apto::SmartPtr<T> GetData()
      {
        Apto::SmartPtr<T> rtn;
        rtn.DynamicCastFrom(m_data.Get(Apto::String(typeid(T).name())));
        return rtn;
      }
    };
    
  };
};

#endif
