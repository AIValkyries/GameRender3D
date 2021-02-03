#include "CubeSceneNode.h"
#include "Color.h"
#include "Vertex.h"
#include "GameHost.h"
#include "UtilTools.h"

/*
* 上下
   (-1,1,1)     (1,1,1)
      /7--------/6        
     /  |      / |        
    /   |     /  |         
   4----------5  |         
    |   1- - -| -2        
    |  /      |  /
    |/        | /
    0---------3/
(-1,-1,-1)

前后
   (-1,1,1)     (1,1,1)
      /18--------/19
     /  |       /|
    /   |      / |
   11---------10 |
    |   17-  -| -16
    |  /      |  /
    |/        | /
    8---------9/
(-1,-1,-1)

左右
   (-1,1,1)     (1,1,1)
      /23--------/14
     /  |       / |
    /   |      /  |
   22---------15  |
    |   20- - | -13
    |  /      |  /
    |/        | /
    21-------12/
(-1,-1,-1)

*/

ISceneNode* CubeSceneNode::Create(GameHost* _gameHost)
{
    SceneObject::Create(_gameHost);
    const int32 indexCount = 36;
    const int32 vertexCount = 24;

    MeshBuffer* mb = new MeshBuffer();
    mb->CreateIndices(INDEX_16BIT, indexCount);
    mb->CreateVertex(vertexCount);

    const uint16 u[indexCount] =
    {
          0,1,2,          2,3,0,              // 下--顺时针顺序(CW)
          4,5,6,          6,7,4,             // 上--逆时针顺序(CCW)

          8,9,10,          10,11,8,             // 前--逆时针顺序(CCW)
          12,13,14,       14,15,12,          // 右边

          16,17,18,       18,19,16,         // 后
          20,21,22,       22,23,20          // 左
    };

    for (int32 i = 0;i < indexCount;i++)
    {
        mb->SetIndices(i, u[i]);
    }

    float32 c = 0.5F;

    ColorF red(c, c, c);
    ColorF green(c, c, c);
    ColorF bule(c, c, c);
    ColorF white(c, c, c);

    static float32 edge = 10;

    // 下
    Vertex* v0 = new Vertex(-edge, -edge, -edge, 0, -1, 0, 1, 1, bule);
    Vertex* v1 = new Vertex(-edge, -edge, edge, 0, -1, 0, 1, 0, green);
    Vertex* v2 = new Vertex(edge, -edge, edge, 0, -1, 0, 0, 0, red);
    Vertex* v3 = new Vertex(edge, -edge, -edge, 0, -1, 0, 0, 1, white);

    //上
    Vertex* v4 = new Vertex(-edge, edge, -edge, 0, 1, 0, 0, 1, bule);
    Vertex* v5 = new Vertex(edge, edge, -edge, 0, 1, 0, 1, 1, green);
    Vertex* v6 = new Vertex(edge, edge, edge, 0, 1, 0, 1, 0, red);
    Vertex* v7 = new Vertex(-edge, edge, edge, 0, 1, 0, 0, 0, white);

    // 前
    Vertex* v8 = new Vertex(-edge, -edge, -edge, 0, 0, 1, 0, 1, bule);
    Vertex* v9 = new Vertex(edge, -edge, -edge, 0, 0, 1, 1, 1, green);
    Vertex* v10 = new Vertex(edge, edge, -edge, 0, 0, 1, 1, 0, red);
    Vertex* v11 = new Vertex(-edge, edge, -edge, 0, 0, 1, 0, 0, white);

    // 右
    Vertex* v12 = new Vertex(edge, -edge, -edge, 1, 0, 0, 0, 1, bule);
    Vertex* v13 = new Vertex(edge, -edge, edge, 1, 0, 0, 1, 1, green);
    Vertex* v14 = new Vertex(edge, edge, edge, 1, 0, 0, 1, 0, red);
    Vertex* v15 = new Vertex(edge, edge, -edge, 1, 0, 0, 0, 0, white);

    // 后
    Vertex* v16 = new Vertex(edge, -edge, edge, 0, 0, -1, 0, 1, bule);
    Vertex* v17 = new Vertex(-edge, -edge, edge, 0, 0, -1, 1, 1, green);
    Vertex* v18 = new Vertex(-edge, edge, edge, 0, 0, -1, 1, 0, red);
    Vertex* v19 = new Vertex(edge, edge, edge, 0, 0, -1, 0, 0, white);

    // 左
    Vertex* v20 = new Vertex(-edge, -edge, edge, -1, 0, 0, 0, 1, bule);
    Vertex* v21 = new Vertex(-edge, -edge, -edge, -1, 0, 0, 1, 1, green);
    Vertex* v22 = new Vertex(-edge, edge, -edge, -1, 0, 0, 1, 0, red);
    Vertex* v23 = new Vertex(-edge, edge, edge, -1, 0, 0, 0, 0, white);

    mb->SetSourceVertex(0, *v0);
    mb->SetSourceVertex(1, *v1);
    mb->SetSourceVertex(2, *v2);
    mb->SetSourceVertex(3, *v3);
    mb->SetSourceVertex(4, *v4);
    mb->SetSourceVertex(5, *v5);
    mb->SetSourceVertex(6, *v6);
    mb->SetSourceVertex(7, *v7);

    mb->SetSourceVertex(8, *v8);
    mb->SetSourceVertex(9, *v9);
    mb->SetSourceVertex(10, *v10);
    mb->SetSourceVertex(11, *v11);
    mb->SetSourceVertex(12, *v12);
    mb->SetSourceVertex(13, *v13);
    mb->SetSourceVertex(14, *v14);
    mb->SetSourceVertex(15, *v15);

    mb->SetSourceVertex(16, *v16);
    mb->SetSourceVertex(17, *v17);
    mb->SetSourceVertex(18, *v18);
    mb->SetSourceVertex(19, *v19);
    mb->SetSourceVertex(20, *v20);
    mb->SetSourceVertex(21, *v21);
    mb->SetSourceVertex(22, *v22);
    mb->SetSourceVertex(23, *v23);

    mb->ResetAABB(0, 0, 0);

    Vector4 tempPos(0.5f, 0.5f, 0.5f, 0);

    AABB* aabb = mb->GetAABB();

    for (int32 i = 0;i < vertexCount;i++)
    {
        Vector4 position = mb->GetSourceVertex(i)->position;
        aabb->UnionPoint(position);
    }

    worldBounds  = new AABB();
    sourceBounds = new AABB();

    worldBounds->Set(aabb);
    sourceBounds->Set(aabb);

    meshBuffers.push_back(mb);

    textureMulColor = true;

    return this;
}

void CubeSceneNode::Render(bool updateChild)
{
    SceneObject::Render(updateChild);
}


void CubeSceneNode::UpdateBox(bool updateChild)
{
    if (worldBounds && sourceBounds)
    {
        worldBounds->Set(sourceBounds);
        TransformBox(localToWorldMatrix, *worldBounds);
        //TransformBoxEx(localToWorldMatrix, *worldBounds);
    }
    SceneObject::UpdateBox(updateChild);
}


void CubeSceneNode::Destroy()
{
    Debug::LogInfo("移除Cube网格!!");

    SafeDelete(worldBounds);
    SafeDelete(sourceBounds);
    
    if (meshBuffers.size())
    {
        for (uint32 i = 0;i < meshBuffers.size();i++)
        {
            meshBuffers[i]->Destroy();
            delete meshBuffers[i];
            meshBuffers[i] = 0;
        }
        meshBuffers.clear();
    }

    SceneObject::Destroy();
}


