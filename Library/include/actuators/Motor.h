//
//  Motor.h
//  Stonefish
//
//  Created by Patryk Cieslak on 15/09/2015.
//  Copyright (c) 2015-2018 Patryk Cieslak. All rights reserved.
//

#ifndef __Stonefish_Motor__
#define __Stonefish_Motor__

#include "actuators/JointActuator.h"

namespace sf
{
    //! A class representing a motor.
    class Motor : public JointActuator
    {
    public:
        //! A constructor.
        /*!
         \param uniqueName a name for the motor
         */
        Motor(std::string uniqueName);
        
        //! A method used to update the internal state of the actuator.
        /*!
         \param dt the time step of the simulation [s]
         */
        virtual void Update(Scalar dt);
        
        //! A method to set the motor torque.
        /*!
         \param tau a value of the motor torque [Nm]
         */
        virtual void setIntensity(Scalar tau);
        
        //! A method returning the torque generated by the motor.
        virtual Scalar getTorque();
        
        //! A method returning the angular position of the motor.
        virtual Scalar getAngle();
        
        //! A method returning the angular velocity of the motor.
        virtual Scalar getAngularVelocity();
        
    protected:
        Scalar torque;
    };
}

#endif