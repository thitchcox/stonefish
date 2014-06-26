//
//  ServoController.cpp
//  Stonefish
//
//  Created by Patryk Cieslak on 11/05/2014.
//  Copyright (c) 2014 Patryk Cieslak. All rights reserved.
//

#include "ServoController.h"

#pragma mark Constructors
ServoController::ServoController(std::string uniqueName, DCMotor* m, FakeRotaryEncoder* e, btScalar maxVoltage, btScalar frequency) : Controller(uniqueName, frequency)
{
    motor = m;
    encoder = e;
    maxV = maxVoltage;
    gainP = btScalar(1.);
    gainI = btScalar(0.);
    gainD = btScalar(0.);
    
    Reset();
}

#pragma mark - Destructor
ServoController::~ServoController()
{
}

#pragma mark - Controller
void ServoController::Reset()
{
    targetPos = 0.0;
    lastError = 0.0;
    integratedError = 0.0;
}

ControllerType ServoController::getType()
{
    return CONTROLLER_DCSERVO;
}

void ServoController::Tick(btScalar dt)
{
    //get measurements
    Sample encSample = encoder->getLastSample();
    
    //calculate error
    btScalar error = targetPos - encSample.getValue(0);
    integratedError += error * dt;
    btScalar derivativeError = (error - lastError)/dt;
    lastError = error;
    
    //calculate and apply control
    btScalar control = gainP * error + gainI * integratedError + gainD * derivativeError;
    control = control > maxV ? maxV : (control < -maxV ? -maxV : control);
    motor->setVoltage(control);
}

#pragma mark - Servo
void ServoController::SetPosition(btScalar pos)
{
    targetPos = UnitSystem::SetAngle(pos);
}

void ServoController::SetGains(btScalar P, btScalar I, btScalar D)
{
    gainP = P;
    gainI = I;
    gainD = D;
}
