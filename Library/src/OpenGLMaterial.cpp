//
//  OpenGLMaterial.cpp
//  Stonefish
//
//  Created by Patryk Cieslak on 10/5/13.
//  Copyright (c) 2013 Patryk Cieslak. All rights reserved.
//

#include "OpenGLMaterial.h"
#include "OpenGLGBuffer.h"
#include "SystemUtil.h"
#include "stb_image.h"
#include "Console.h"

#define clamp(x,min,max)     (x > max ? max : (x < min ? min : x))

Look CreateOpaqueLook(glm::vec3 rgbColor, GLfloat diffuseReflectance, GLfloat roughness, GLfloat IOR, const char* textureName)
{
    Look look;
    look.color = rgbColor;
    look.data[0] = diffuseReflectance;
    look.data[1] = roughness;
    look.data[2] = IOR;
    look.data[3] = 0.f; //No reflections
    
    if(textureName != NULL)
        look.texture = LoadTexture(textureName);
    else
        look.texture = 0;
    
    return look;
}

/*
Look CreateReflectiveLook(GLfloat R, GLfloat G, GLfloat B, GLfloat hFactor, GLfloat vFactor, const char* textureName)
{
    Look newLook = CreateBasicLook(R, G, B, textureName);
    newLook.type = REFLECTIVE;
    newLook.factor[0] = clamp(hFactor, 0.f, 1.f);
    newLook.factor[1] = clamp(vFactor, 0.f, 1.f);
    return newLook;
}

Look CreateTransparentLook(GLfloat R, GLfloat G, GLfloat B, GLfloat opacity, GLfloat shininess, const char* textureName)
{
    Look newLook = CreateBasicLook(R, G, B, textureName);
    newLook.type = TRANSPARENT;
    newLook.factor[0] = clamp(opacity, 0.f, 1.f);
    newLook.factor[1] = clamp(shininess, 0.f, 1.f);
    return newLook;
}*/

void UseLook(Look l)
{
    if(l.texture != 0)
    {
        glBindTexture(GL_TEXTURE_2D, l.texture);
        OpenGLGBuffer::SetUniformIsTextured(true);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        OpenGLGBuffer::SetUniformIsTextured(false);
    }
    
    //diffuse reflectance, roughness, FO, reflection factor
    GLfloat diffuseReflectance = floor(l.data[0] * 255.f);
    GLfloat roughness = floor(l.data[1] * 255.f);
    GLfloat F0 = floor(powf((1.f - l.data[2])/(1.f + l.data[2]), 2.f) * 255.f);
    GLfloat reflection = floor(l.data[3] * 255.f);
    GLfloat materialData = diffuseReflectance
                           + (roughness * 256)
                           + (F0 * 256 * 256)
                           + (reflection * 256 * 256 * 256);
    
    glColor4f(l.color[0], l.color[1], l.color[2], 1.0); //Color + Texture mix factor
    OpenGLGBuffer::SetUniformMaterialData(materialData);
}

//Statics
GLuint LoadTexture(const char* filename)
{
    int width, height, channels;
    GLuint texture;
    
    // Allocate image; fail out on error
    cInfo("Loading texture from: %s", filename);
    
    unsigned char* dataBuffer = stbi_load(filename, &width, &height, &channels, 3);
    if(dataBuffer == NULL)
    {
        cError("Failed to load texture!");
        return -1;
    }
    
    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    
    // Allocate an OpenGL texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Upload texture to memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataBuffer);
    // Set certain properties of texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    // Wrap texture around
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Release internal buffer
    stbi_image_free(dataBuffer);
    
    return texture;
}

GLuint LoadInternalTexture(const char* filename)
{
    char path[1024];
    GetDataPath(path, 1024-32);
    strcat(path, filename);
    return LoadTexture(path);
}


