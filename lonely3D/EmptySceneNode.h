#pragma once
#ifndef __EmptySceneNode_H__
#define __EmptySceneNode_H__

#include "SceneObject.h"
#include "MeshBuffer.h"

class EmptySceneNode :public SceneObject
{
public:
	virtual ISceneNode* Create(GameHost* _gameHost);
	virtual void Update(bool updateChild);
	virtual void UpdateBox(bool updateChild);
	virtual void Destroy();
};

#endif

