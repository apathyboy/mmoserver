/*
---------------------------------------------------------------------------------------
This source file is part of SWG:ANH (Star Wars Galaxies - A New Hope - Server Emulator)

For more information, visit http://www.swganh.com

Copyright (c) 2006 - 2010 The SWG:ANH Team
---------------------------------------------------------------------------------------
Use of this source code is governed by the GPL v3 license that can be found
in the COPYING file or at http://www.gnu.org/licenses/gpl-3.0.html

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
---------------------------------------------------------------------------------------
*/

#ifndef ANH_ZONESERVER_TANGIBLE_CONTAINER_H
#define ANH_ZONESERVER_TANGIBLE_CONTAINER_H

#include "TangibleObject.h"
#include "ObjectFactoryCallback.h"

class CreatureObject;

//=============================================================================

class Container : public TangibleObject
{
    friend class ContainerObjectFactory;

public:

    Container();
    ~Container();

    CreatureObject* getParent() {
        return mParent;
    }
    void		setParent(CreatureObject* creature) {
        mParent = creature;
    }
    uint32		getObjectLoadCounter() {
        return mObjectLoadCounter;
    }
    void		setObjectLoadCounter(uint32 count) {
        mObjectLoadCounter = count;
    }

    ObjectList*	getObjects() {
        return &mObjects;
    }

    void		addObject(Object* object) {
        mObjects.push_back(object);
    }
    void		removeObject(Object* object);
    void		deleteObject(Object* object);

private:
    uint32				mObjectLoadCounter;
    CreatureObject*		mParent;
    ObjectList			mObjects;
};

//=============================================================================

#endif

