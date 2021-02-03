#pragma once

#ifndef __LightSceneNode_H__
#define __LightSceneNode_H__

#include "SceneObject.h"
#include "MeshBuffer.h"


class Light;
class LightSceneNode :public SceneObject
{
public:
	virtual ISceneNode* Create(GameHost* _gameHost);
	virtual void UpdateBox(bool updateChild);
	virtual void Destroy();
	virtual void RenderBox(bool updateChild);

	void SetLight(Light* _light);
	Light* GetLight()const { return light; }

private:
	Light* light;
	
	void RenderCone(Vector3 direction, float32 angle);  // ‰÷»æÕ‚Ω«ªÚƒ⁄Ω«

};

#endif

