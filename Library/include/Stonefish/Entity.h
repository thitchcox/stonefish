//
//  Entity.h
//  Stonefish
//
//  Created by Patryk Cieslak on 11/28/12.
//  Copyright (c) 2012 Patryk Cieslak. All rights reserved.
//

#ifndef __Stonefish_Entity__
#define __Stonefish_Entity__

#include "UnitSystem.h"
#include "NameManager.h"
#include "OpenGLPipeline.h"

#define BIT(x) (1<<(x))

typedef enum
{
    ENTITY_STATIC, ENTITY_SOLID, ENTITY_CABLE, ENTITY_GHOST, ENTITY_FEATHERSTONE
}
EntityType;

typedef enum
{
    NONCOLLIDING = 0,
    STATIC = BIT(0),
    DEFAULT = BIT(1),
    CABLE_EVEN = BIT(2),
    CABLE_ODD = BIT(3)
}
CollisionMask;

//abstract class
class Entity
{
public:
	Entity(std::string uniqueName);
	virtual ~Entity();
    
    void setRenderable(bool render);
    bool isRenderable();
    std::string getName();
    
   	virtual EntityType getType() = 0;
    virtual void Render() = 0;
    virtual void AddToDynamicsWorld(btMultiBodyDynamicsWorld* world) = 0;
    virtual void GetAABB(btVector3& min, btVector3& max);
        
private:
    bool renderable;
    std::string name;
    
    static NameManager nameManager;
};

#endif
