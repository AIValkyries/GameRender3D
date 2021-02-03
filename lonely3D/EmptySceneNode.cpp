#include "EmptySceneNode.h"

ISceneNode* EmptySceneNode::Create(GameHost* _gameHost)
{
	SceneObject::Create(_gameHost);
	return this;
}

void EmptySceneNode::Update(bool updateChild)
{
	if (children.size() && updateChild)
	{
		std::vector<ISceneNode*>::iterator itr;

		for (itr = children.begin();itr != children.end();itr++)
		{
			(*itr)->Update(updateChild);
		}
	}
}

void EmptySceneNode::UpdateBox(bool updateChild)
{
	if (children.size())
	{
		std::vector<ISceneNode*>::iterator itr;

		for (itr = children.begin();itr != children.end();itr++)
		{
			(*itr)->UpdateBox(updateChild);
		}
	}
}

void EmptySceneNode::Destroy()
{
	if (children.size())
	{
		std::vector<ISceneNode*>::iterator itr;

		for (itr = children.begin();itr != children.end();itr++)
		{
			(*itr)->Destroy();
			if (*itr)
			{
				delete* itr;
				(*itr) = 0;
			}
		}
		children.clear();
	}
}