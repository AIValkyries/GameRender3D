#include "LightSceneNode.h"
#include "Light.h"
#include "GameHost.h"

ISceneNode* LightSceneNode::Create(GameHost* _gameHost)
{
	SceneObject::Create(_gameHost);

    meshBuffers.push_back(new MeshBuffer());

    meshBuffers[0]->SetRenderState(FLAT, true);

    return this;
}

// 构建box
void LightSceneNode::SetLight(Light* _light)
{
    if (_light == 0) return;

    light = _light;

    if (sourceBounds == 0) sourceBounds = new AABB();
    if (worldBounds == 0)  worldBounds = new AABB();

    // 聚光灯、点光灯
    if (light->Type == L_POINT || light->Type == L_STANDARD_SPOT)
    {
        // 有位置和半径
        sourceBounds->Set(light->Position);
        sourceBounds->Set(
            -light->Radius, -light->Radius, -light->Radius,
            light->Radius, light->Radius, light->Radius);
        localPosition += light->Position;
    }
    else if (light->Type == L_DIRECTIONAL)   //平行灯
    {
        // 没有位置，只有方向和光本身的颜色
        sourceBounds->Set(-10, -10, -10, 10, 10, 10);
    }

    UpdateBox(true);

    for (uint32 i = 0;i < meshBuffers.size();i++)
    {
        meshBuffers[i]->SetAABB(sourceBounds);
    }

}

void LightSceneNode::UpdateBox(bool updateChild)
{
    if (worldBounds && sourceBounds)
    {
        worldBounds->Set(sourceBounds);
        TransformBox(localToWorldMatrix, *worldBounds);
        //TransformBoxEx(localToWorldMatrix, *worldBounds);
    }
    SceneObject::UpdateBox(updateChild);
}

void LightSceneNode::RenderBox(bool updateChild)
{
    if (meshBuffers[0]->CheckRenderState(RENDER_BOX))
    {
        // 方向，位置， 
        // 计算起点
        if (light)
        {
            if (light->Type == L_STANDARD_SPOT)
            {
                //渲染外角
                RenderCone(light->Direction, light->OuterCone);
                // 渲染内角
                RenderCone(light->Direction, light->InnerCone);
            }

            if (light->Type == L_STANDARD_SPOT || light->Type == L_DIRECTIONAL)
            {
                // 渲染方向
                Vector3 start(0, 0, 0);
                Vector3 end = light->Direction * (light->Radius != 0 ? light->Radius : 10);

                gameHost->Draw3DLine(
                    start, end,
                    ColorF(0, 0, 1.0F, 0), 
                    meshBuffers[0], globaLocalToWorldMatrix,
                    GetShaderType());
            }
        }

    }
    
	SceneObject::RenderBox(updateChild);
}

void LightSceneNode::RenderCone(
     Vector3 direction,  float32 angle)
{
    Vector3 start(1, 1, 1);

    if (direction.x < 0)   start.x = -1;
    if (direction.y < 0)   start.y = -1;
    if (direction.z < 0)   start.z = -1;

    Matrix3x3 mat;
    BuildAxisAngle(direction, angle, mat);

    Vector3 end = mat.MultiplyVector3(start);
    end *= light->Radius;

    gameHost->Draw3DLine(
        start, end, 
        ColorF(0, 0, 0, 1.0F), 
        meshBuffers[0], 
        globaLocalToWorldMatrix,
        GetShaderType());
 
    mat.SetIdentity();
    BuildAxisAngle(direction, -angle, mat);

    Vector3 end1 = mat.MultiplyVector3(start);
    end1 *= light->Radius;

    gameHost->Draw3DLine(
        start, end1,
        ColorF(0, 0, 0, 1.0F),
        meshBuffers[0], 
        globaLocalToWorldMatrix,
        GetShaderType());
}

void LightSceneNode::Destroy()
{
    SceneObject::Destroy();
    if (light)
    {
        delete light;
        light = 0;
    }
}