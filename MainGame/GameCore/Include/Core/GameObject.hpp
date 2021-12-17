//////////////////////////////////////////////////////////////////////////////////
///             @file   ZPrepass.hpp
///             @brief  ZPrepass Class
///             @author Toide Yutaro
///             @date   2021_07_30
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/Sprite.hpp"
#include "DirectX12/Include/Core/DirectX12BlendState.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include"GameMath/Include/GMTransform.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class Component;

//////////////////////////////////////////////////////////////////////////////////
//								Class 
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			GameObject
*************************************************************************//**
*  @class    GameObject
*  @brief    GameObject (base class in the all object class)
*****************************************************************************/
class GameObject
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	template <typename T> T* Create();
	static GameObject* Find(const std::string& name);

	Component*                          AddComponent   (Component* component);
	bool                                RemoveComponent(Component* component);
	template<typename T> T              GetComponent   () const;
	template<typename T> std::vector<T> GetComponents  () const;


	static bool Destroy(GameObject* gameObject);
	static bool DestroyImmediate(GameObject* gameObject);
	static void DestroyWithChildren(GameObject* parent);
	static void DestroyAllTagObject(const std::string& tag);
	static bool ClearAllGameObjects();
	static void ClearDestoyObjects();

	static std::vector<GameObject*> GameObjectsWithTag(const std::string& tag);
	static size_t CountGameObjects() { return _gameObjects.size(); }

	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Transform&       GetTransform()       { return _transform; }
	const gm::Transform& GetTransform() const { return _transform; }
	std::string    GetName      () const  { return _name; }
	std::string    GetTag       () const  { return _tag; }
	std::string    GetLayerName () const  { return _layerList[_layer]; }
	bool           IsActive     () const  { return _isActive; }
	int            GetChildCount() const  { return static_cast<int>(_children.size()); }
	
	GameObject* GetChild(int index) { return (index < _children.size()) ? _children[index] : nullptr; }
	static GameObject* GetGameObjectList(int index) { return _gameObjects[index]; }

	bool RemoveChild(GameObject* child);
	void ClearChildren();

	void SetName(const std::string& name) { _name = name; }
	void SetTag (const std::string& name) { _tag  = name; }
	void SetActive(bool isActive)       { _isActive   = isActive; }
	void SetParent(GameObject* parent)  { _parent     = parent; }
	void SetChild (GameObject* child)   { _children.push_back(child); }
	void SetPosition(float x, float y, float z);
	void SetPosition(const gm::Float3& position);
	void SetScale   (float x, float y, float z);
	void SetScale   (const gm::Float3& scale);
	void SetRotation(const gm::Quaternion& rotation);

	void SetLayer(const std::string& layer);


	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	GameObject();
	GameObject(const GameObject&)            = default;
	GameObject& operator=(const GameObject&) = default;
	GameObject(GameObject&&)                 = default;
	GameObject& operator=(GameObject&&)      = default;
	~GameObject();
protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	/*-------------------------------------------------------------------
	-           gameObject default info
	---------------------------------------------------------------------*/
	std::string   _name;
	std::string   _tag;
	int           _layer;

	/*-------------------------------------------------------------------
	-           gameObject state
	---------------------------------------------------------------------*/
	bool          _isActive;
	gm::Transform _transform;

	/*-------------------------------------------------------------------
	-           parent tree
	---------------------------------------------------------------------*/
	GameObject*   _parent;
	std::vector<GameObject*> _children;
	std::vector<Component*>  _components;
	static bool IsUpdating;

private:
	int GetLayerBit(const std::string& layer);

	static std::vector<GameObject*> _gameObjects;
	static std::vector<GameObject*> _destroyGameObjects;
	static std::vector<std::string> _layerList;
};

template<typename T> T* GameObject::Create()
{
	T test;
	return *test;
}

template<typename T> T GameObject::GetComponent() const 
{
	for (auto it = _components.begin(); it != _components.end(); ++it)
	{
		if (typeid(*it) == T)
		{
			return (T)(*it);
		}
	}

	return NULL;
}

template<typename T> std::vector<T> GameObject::GetComponents() const
{
	std::vector<T> componentList;
	for (auto it = _components.begin(); it != _components.end(); ++it)
	{
		if (typeid(*it) == T)
		{
			componentList.emplace_back((T)(*it));
		}
	}
	return std::move(componentList);
}
#endif