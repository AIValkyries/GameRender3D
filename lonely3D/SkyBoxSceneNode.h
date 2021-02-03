#pragma once
#ifndef __SkyBoxSceneNode_H__
#define __SkyBoxSceneNode_H__

#include "SceneObject.h"
#include "MeshBuffer.h"

class SkyBoxSceneNode :public SceneObject
{
public:
	void CreateSkyBox(
		Texture* top,
		Texture* bottom,
		Texture* left,
		Texture* right,
		Texture* front,
		Texture* back);

	virtual void Render(bool updateChild);
	virtual void Update(bool updateChild);
	virtual void Destroy();

};


#endif
