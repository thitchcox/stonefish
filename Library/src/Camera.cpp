//
//  Camera.cpp
//  Stonefish
//
//  Created by Patryk Cieslak on 4/7/17.
//  Copyright (c) 2017 Patryk Cieslak. All rights reserved.
//

#include "Camera.h"
#include "MathsUtil.hpp"
#include "SimulationApp.h"

Camera::Camera(std::string uniqueName, uint32_t resX, uint32_t resY, btScalar horizFOVDeg, const btTransform& geomToSensor, SolidEntity* attachment, btScalar frequency, uint32_t spp, bool ao) : Sensor(uniqueName, frequency)
{
    g2s = UnitSystem::SetTransform(geomToSensor);
    attach = attachment;
    fovH = horizFOVDeg;
    resx = resX;
    resy = resY;
    pan = btScalar(0);
    tilt = btScalar(0);
    renderSpp = spp < 1 ? 1 : (spp > 16 ? 16 : spp);
    renderAO = ao;
    display = false;
    newDataCallback = NULL;
    imageData = new uint8_t[resx*resy*3]; //RGB color
    memset(imageData, 0, resx*resy*3);
    
    glCamera = new OpenGLCamera(glm::vec3(0,0,0), glm::vec3(0,0,1.f), glm::vec3(0,-1.f,0), 0, 0, resx, resy, (GLfloat)fovH, 1000.f, renderSpp, renderAO);
    glCamera->setCamera(this);
    UpdateTransform();
    glCamera->UpdateTransform();
    InternalUpdate(0);
    OpenGLContent::getInstance()->AddView(glCamera);
}
    
Camera::~Camera()
{
    glCamera = NULL;
    delete imageData;
}

btScalar Camera::getHorizontalFOV()
{
    return fovH;
}

void Camera::getResolution(uint32_t& x, uint32_t& y)
{
    x = resx;
    y = resy;
}

uint8_t* Camera::getDataPointer()
{
    return imageData;
}

void Camera::setDisplayOnScreen(bool onScreen)
{
    display = onScreen;
}

bool Camera::getDisplayOnScreen()
{
    return display;
}

void Camera::setPan(btScalar value)
{
    value = UnitSystem::SetAngle(value);
    pan = value;
    //SetupCamera();
}

void Camera::setTilt(btScalar value)
{
    value = UnitSystem::SetAngle(value);
    tilt = value;
    //SetupCamera();
}

btScalar Camera::getPan()
{
    return UnitSystem::GetAngle(pan);
}

btScalar Camera::getTilt()
{
    return UnitSystem::GetAngle(tilt);
}

void Camera::InstallNewDataHandler(std::function<void(Camera*)> callback)
{
    newDataCallback = callback;
}

void Camera::NewDataReady()
{
    if(newDataCallback != NULL)
        newDataCallback(this);
}

void Camera::UpdateTransform()
{
    btTransform cameraTransform = getSensorFrame();
    btVector3 eyePosition = cameraTransform.getOrigin(); //O
    btVector3 direction = cameraTransform.getBasis().getColumn(2); //Z
    btVector3 cameraUp = -cameraTransform.getBasis().getColumn(1); //-Y
    
    bool zUp = SimulationApp::getApp()->getSimulationManager()->isZAxisUp();
    
    //additional camera rotation
    /*glm::vec3 tiltAxis = glm::normalize(glm::cross(dir, up));
    glm::vec3 panAxis = glm::normalize(glm::cross(tiltAxis, dir));
    
    //rotate
	lookingDir = glm::rotate(lookingDir, tilt, tiltAxis);
	lookingDir = glm::rotate(lookingDir, pan, panAxis);
    lookingDir = glm::normalize(lookingDir);
    
    currentUp = glm::rotate(currentUp, tilt, tiltAxis);
    currentUp = glm::rotate(currentUp, pan, panAxis);
	currentUp = glm::normalize(currentUp);*/
    
    if(zUp)
    {
        glm::vec3 eye = glm::vec3((GLfloat)eyePosition.x(), (GLfloat)eyePosition.y(), (GLfloat)eyePosition.z());
        glm::vec3 dir = glm::vec3((GLfloat)direction.x(), (GLfloat)direction.y(), (GLfloat)direction.z());
        glm::vec3 up = glm::vec3((GLfloat)cameraUp.x(), (GLfloat)cameraUp.y(), (GLfloat)cameraUp.z());
        glCamera->SetupCamera(eye, dir, up);
    }
    else
    {
        btMatrix3x3 rotation;
        rotation.setEulerYPR(0,M_PI,0);
        btVector3 rotEyePosition = rotation * eyePosition;
        btVector3 rotDirection = rotation * direction;
        btVector3 rotCameraUp = rotation * cameraUp;
        glm::vec3 eye = glm::vec3((GLfloat)rotEyePosition.x(), (GLfloat)rotEyePosition.y(), (GLfloat)rotEyePosition.z());
        glm::vec3 dir = glm::vec3((GLfloat)rotDirection.x(), (GLfloat)rotDirection.y(), (GLfloat)rotDirection.z());
        glm::vec3 up = glm::vec3((GLfloat)rotCameraUp.x(), (GLfloat)rotCameraUp.y(), (GLfloat)rotCameraUp.z());
        glCamera->SetupCamera(eye, dir, up);
    }
}

void Camera::InternalUpdate(btScalar dt)
{
    glCamera->Update();
}

std::vector<Renderable> Camera::Render()
{
    std::vector<Renderable> items(0);
    
    btTransform cameraTransform;
    
    if(attach != NULL)
        cameraTransform = attach->getTransform() * attach->getGeomToCOGTransform().inverse() * g2s;
    else
        cameraTransform = g2s;
    
    Renderable item;
    item.model = glMatrixFromBtTransform(cameraTransform);
    item.type = RenderableType::SENSOR_LINES;
    
    //Create camera dummy
    GLfloat iconSize = 1.f;
    GLfloat x = iconSize*tanf(fovH/360.f*M_PI);
    GLfloat aspect = (GLfloat)resx/(GLfloat)resy;
    GLfloat y = x/aspect;
	
	item.points.push_back(glm::vec3(0,0,0));
	item.points.push_back(glm::vec3(x, -y, iconSize));
	item.points.push_back(glm::vec3(0,0,0));
	item.points.push_back(glm::vec3(x,  y, iconSize));
	item.points.push_back(glm::vec3(0,0,0));
	item.points.push_back(glm::vec3(-x, -y, iconSize));
	item.points.push_back(glm::vec3(0,0,0));
	item.points.push_back(glm::vec3(-x,  y, iconSize));
	
	item.points.push_back(glm::vec3(x, -y, iconSize));
	item.points.push_back(glm::vec3(x, y, iconSize));
	item.points.push_back(glm::vec3(x, y, iconSize));
	item.points.push_back(glm::vec3(-x, y, iconSize));
	item.points.push_back(glm::vec3(-x, y, iconSize));
	item.points.push_back(glm::vec3(-x, -y, iconSize));
	item.points.push_back(glm::vec3(-x, -y, iconSize));
	item.points.push_back(glm::vec3(x, -y, iconSize));
    
    item.points.push_back(glm::vec3(-0.5f*x, -y, iconSize));
	item.points.push_back(glm::vec3(0.f, -1.5f*y, iconSize));
	item.points.push_back(glm::vec3(0.f, -1.5f*y, iconSize));
	item.points.push_back(glm::vec3(0.5f*x, -y, iconSize));
    
    items.push_back(item);
    return items;
}

SensorType Camera::getType()
{
    return SensorType::SENSOR_CAMERA;
}

btTransform Camera::getSensorFrame()
{
    return attach->getTransform() * attach->getGeomToCOGTransform().inverse() * g2s;
}