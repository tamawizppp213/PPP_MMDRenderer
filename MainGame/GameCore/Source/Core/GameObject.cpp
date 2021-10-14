/////////////////////////////////////////////////////////////////////////////////
//              Title:  GameObject.hpp
//            Content:  GameObject.hpp
//             Author:  Toide Yutaro
//             Create:  2021_07_30
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/GameObject.hpp"
#include "GameCore/Include/Core/GameComponent.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
#define INVALID_VALUE (-1)

//////////////////////////////////////////////////////////////////////////////////
//                   Implement
//////////////////////////////////////////////////////////////////////////////////
std::vector<std::unique_ptr<GameObject>> GameObject::_gameObjects;
std::vector<std::unique_ptr<GameObject>> GameObject::_destroyGameObjects;
std::vector<std::string> GameObject::_layerList;
bool GameObject::IsUpdating = false;


GameObject::GameObject()
{
	_gameObjects.emplace_back(this);
	_tag      = "";
	_name     = "";
	_isActive = true;
	_parent   = nullptr;
	
}


/****************************************************************************
*                          Find
*************************************************************************//**
*  @fn        GameObject* GameObject::Find(const std::string& name)
*  @brief     This function returns the gameObject with the same name as the assign name.
*  @param[in] std::string name
*  @return 　　GameObject* 
*****************************************************************************/
GameObject* GameObject::Find(const std::string& name)
{
	for (auto it = _gameObjects.begin(); it != _gameObjects.end(); ++it)
	{
		
		if ((*it)->_name == name)
		{
			return it->get();
		}
	}
	return nullptr;
}

#pragma region Component Management
Component* GameObject::AddComponent(Component* component)
{
	if (component->_gameObject == nullptr)
	{
		_components.emplace_back(component);
		component->_gameObject = this;
	}
	return component;
}

bool GameObject::RemoveComponent(Component* component)
{
	for (auto it = _components.begin(); it != _components.end(); ++it)
	{
		if ((*it) == component)
		{
			_components.erase(it);
			return true;
		}
	}
	return false;
}
#pragma endregion Component Management
/****************************************************************************
*                          GameObjectsWithTag
*************************************************************************//**
*  @fn        std::vector<GameObject*> GameObject::GameObjectsWithTag(const std::string& tag)
*  @brief     This function returns the gameObject list with the same tag as the assign tag.
*  @param[in] std::string tag
*  @return 　　std::vector<GameObject*> 
*****************************************************************************/
std::vector<GameObject*> GameObject::GameObjectsWithTag(const std::string& tag)
{
	std::vector<GameObject*> gameObjects;
	for (auto it = _gameObjects.begin(); it != _gameObjects.end(); ++it)
	{
		if ((*it)->GetTag() == tag)
		{
			gameObjects.emplace_back(it->get());
		}
	}
	return gameObjects;
}

#pragma region Destroy
/****************************************************************************
*                          Destroy
*************************************************************************//**
*  @fn        GameObject* GameObject::Destroy(GameObject* gameObject)
*  @brief     This function return the gameObject with the same name as the assign name.
*  @param[in] std::string name
*  @return 　　GameObject*
*****************************************************************************/
bool GameObject::Destroy(GameObject* gameObject)
{
	if (gameObject == nullptr) { return false; }

	if (IsUpdating)
	{
		_destroyGameObjects.emplace_back(gameObject);
		gameObject->SetActive(false);
	}
	else
	{
		for (auto it = _gameObjects.begin(); it != _gameObjects.end(); ++it)
		{
			if (it->get() == gameObject)
			{
				std::swap(*it, _gameObjects.back());
				_gameObjects.pop_back(); 
				_gameObjects.shrink_to_fit();         // memory free
				//delete gameObject;                    // delete object
				return true;
			}
		}
	}

	return false;
}

/****************************************************************************
*                          DestroyImmediate
*************************************************************************//**
*  @fn        GameObject* GameObject::DestroyImmediate(GameObject* gameObject)
*  @brief     This function return the gameObject with the same name as the assign name.
*  @param[in] std::string name
*  @return 　　GameObject*
*****************************************************************************/
bool GameObject::DestroyImmediate(GameObject* gameObject)
{
	if (gameObject == nullptr) { return false; }

	for (auto it = _gameObjects.begin(); it != _gameObjects.end(); ++it)
	{
		if (it->get() == gameObject)
		{
			std::swap(*it, _gameObjects.back());
			_gameObjects.pop_back();
			_gameObjects.shrink_to_fit();         // memory free
			//delete gameObject;                    // delete object
			return true;
		}
	}

	return false;
}

/****************************************************************************
*                          DestroyAllTagObject
*************************************************************************//**
*  @fn        GameObject* GameObject::DestroyAllTagObject(const std::string& tag)
*  @brief     This function destroys all objects with the tag
*  @param[in] std::string tag
*  @return 　　void
*****************************************************************************/
void GameObject::DestroyAllTagObject(const std::string& tag)
{

	for (auto it = _gameObjects.begin(); it != _gameObjects.end(); ++it)
	{
		if (it->get()->GetTag() == tag)
		{
			if (IsUpdating)
			{
				_destroyGameObjects.emplace_back(std::move(*it));
				it->get()->SetActive(false);
			}
			else
			{
				_gameObjects.erase(it);
				//delete *it;
			}
		}
	}
	_gameObjects.shrink_to_fit();
}

/****************************************************************************
*                          DestroyWithChildren
*************************************************************************//**
*  @fn        void GameObject::DestroyWithChildren(GameObject* gameObject)
*  @brief     This function destroys gameObjects including child objects.
*  @param[in] GameObject* child
*  @return 　　GameObject*
*****************************************************************************/
void GameObject::DestroyWithChildren(GameObject* gameObject)
{
	if (gameObject == nullptr) { return; }
	for (int i = gameObject->GetChildCount() - 1; i >= 0; --i)
	{
		GameObject* child = gameObject->GetChild(i);
		DestroyWithChildren(child);
	}
	Destroy(gameObject);
}

void GameObject::ClearDestoyObjects()
{
	for (auto it = _destroyGameObjects.begin(); it != _destroyGameObjects.end(); ++it)
	{
		Destroy(it->get());
	}
	_destroyGameObjects.clear();
	_destroyGameObjects.shrink_to_fit();
}
#pragma endregion Destroy

/****************************************************************************
*                          RemoveChild
*************************************************************************//**
*  @fn        bool GameObject::RemoveChild(GameObject* child)
*  @brief     This function removes child gameObject
*  @param[in] GameObject* child
*  @return 　　GameObject*
*****************************************************************************/
bool GameObject::RemoveChild(GameObject* child)
{
	for (auto it = _children.begin(); it != _children.end(); ++it)
	{
		if (*it == child)
		{
			_children.erase(it);
			return true;
		}
	}
	return false;
}

/****************************************************************************
*                          ClearChildren
*************************************************************************//**
*  @fn        bool GameObject::ClearChildren()
*  @brief     This function clears children object.
*  @param[in] void 
*  @return 　　void
*****************************************************************************/
void GameObject::ClearChildren()
{
	_children.clear();
	std::vector<GameObject*>().swap(_children); // memory free

}

bool GameObject::ClearAllGameObjects()
{
	_gameObjects.clear();
	_gameObjects.shrink_to_fit();
	return true;
}

void GameObject::SetLayer(const std::string& layer)
{
	int bit = GetLayerBit(layer);
	if (bit >= 0)
	{
		_layer = (1 << bit);
	}
}

void GameObject::SetPosition(float x, float y, float z)
{
	_transform.LocalPosition = gm::Vector3(x, y, z);
}

void GameObject::SetPosition(const gm::Float3& position)
{
	_transform.LocalPosition = position;
}

void GameObject::SetScale(float x, float y, float z)
{
	_transform.LocalScale = gm::Vector3(x, y, z);\
}

void GameObject::SetScale(const gm::Float3& scale)
{
	_transform.LocalScale = scale;
}

void GameObject::SetRotation(const gm::Quaternion& rotation)
{
	_transform.LocalRotation = rotation;
}

#pragma region Private Function
int GameObject::GetLayerBit(const std::string& layer)
{
	if (layer == "") { return INVALID_VALUE; }
	for (int i = 0; i < 31; ++i) // 4byte
	{
		if (_layerList[i] == layer)
		{
			return i;
		}
	} 
	return INVALID_VALUE;
}

#pragma endregion Private Function