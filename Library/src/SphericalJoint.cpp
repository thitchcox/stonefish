//
//  SphericalJoint.cpp
//  Stonefish
//
//  Created by Patryk Cieslak on 2/3/13.
//  Copyright (c) 2013 Patryk Cieslak. All rights reserved.
//

#include "SphericalJoint.h"

SphericalJoint::SphericalJoint(std::string uniqueName, SolidEntity* solidA, SolidEntity* solidB, const btVector3& pivot, bool collideLinkedEntities) : Joint(uniqueName, collideLinkedEntities)
{
    btRigidBody* bodyA = solidA->getRigidBody();
    btRigidBody* bodyB = solidB->getRigidBody();
    btVector3 pivotInA = bodyA->getCenterOfMassTransform().inverse()*UnitSystem::SetPosition(pivot);
    btVector3 pivotInB = bodyB->getCenterOfMassTransform().inverse()*UnitSystem::SetPosition(pivot);
    
    btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*bodyA, *bodyB, pivotInA, pivotInB);
    setConstraint(p2p);
    
    sigDamping = btVector3(0.,0.,0.);
    velDamping = btVector3(0.,0.,0.);
}

SphericalJoint::~SphericalJoint()
{
}

void SphericalJoint::setDamping(btVector3 constantFactor, btVector3 viscousFactor)
{
    for(int i = 0; i < 3; i++)
    {
        sigDamping[i] = constantFactor[i] > btScalar(0.) ? UnitSystem::SetTorque(btVector3(constantFactor[i],0.0,0.0)).x() : btScalar(0.);
        velDamping[i] = viscousFactor[i] > btScalar(0.) ? viscousFactor[i] : btScalar(0.);
    }
}

JointType SphericalJoint::getType()
{
    return SPHERICAL;
}

void SphericalJoint::ApplyTorque(btVector3 T)
{
    btRigidBody& bodyA = getConstraint()->getRigidBodyA();
    btRigidBody& bodyB = getConstraint()->getRigidBodyB();
    btVector3 torque = UnitSystem::SetTorque(T);
    bodyA.applyTorque(torque);
    bodyB.applyTorque(-torque);
}

void SphericalJoint::ApplyDamping()
{
    if(sigDamping.length2() > btScalar(0.) || velDamping.length2() > btScalar(0.))
    {
        btRigidBody& bodyA = getConstraint()->getRigidBodyA();
        btRigidBody& bodyB = getConstraint()->getRigidBodyB();
        btVector3 relativeAV = bodyA.getAngularVelocity() - bodyB.getAngularVelocity();
        btVector3 torque(0.,0.,0.);
        
        if(relativeAV.x() != btScalar(0.))
            torque[0] = -(sigDamping.x() * relativeAV.x()/fabs(relativeAV.x())) - velDamping.x() * relativeAV.x();
        if(relativeAV.y() != btScalar(0.))
            torque[1] = -(sigDamping.y() * relativeAV.y()/fabs(relativeAV.y())) - velDamping.y() * relativeAV.y();
        if(relativeAV.z() != btScalar(0.))
            torque[2] = -(sigDamping.z() * relativeAV.z()/fabs(relativeAV.z())) - velDamping.z() * relativeAV.z();
        
        bodyA.applyTorque(torque);
        bodyB.applyTorque(-torque);
    }
}

btVector3 SphericalJoint::Render()
{
    btPoint2PointConstraint* p2p = (btPoint2PointConstraint*)getConstraint();
    btVector3 pivot = p2p->getRigidBodyA().getCenterOfMassTransform()(p2p->getPivotInA());
    btVector3 A = p2p->getRigidBodyA().getCenterOfMassPosition();
    btVector3 B = p2p->getRigidBodyB().getCenterOfMassPosition();
    
    glDummyColor();
    glBegin(GL_LINES);
    glBulletVertex(A);
    glBulletVertex(pivot);
    glBulletVertex(B);
    glBulletVertex(pivot);
    glEnd();
    
    return pivot;
}
