#pragma once
#ifndef __CubeSceneNode_H__
#define __CubeSceneNode_H__

#include "SceneObject.h"
#include "MeshBuffer.h"

 
class CubeSceneNode:public SceneObject
{
public:
	virtual ISceneNode* Create(GameHost* _gameHost);
	virtual void UpdateBox(bool updateChild);
	virtual void Destroy();
	virtual void Render(bool updateChild);

};


#endif
