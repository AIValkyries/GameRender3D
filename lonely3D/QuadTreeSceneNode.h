#pragma once
#ifndef __QuadTreeSceneNode_H__
#define __QuadTreeSceneNode_H__

#include "SceneObject.h"
#include "MeshBuffer.h"
#include "QuadTree.h"

class QuadTreeSceneNode :public SceneObject
{
public:
	virtual ISceneNode* Create(GameHost* _gameHost);
	virtual void UpdateBox(bool updateChild);
	virtual void Destroy();
	virtual void Render(bool updateChild);
	
	QuadTree* GetQuadTree()const { return quadTree; }

private:
	QuadTree* quadTree;
};


#endif
