//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>

//external
#include "physicsworld.hpp"
#include "gameobjecthandle.hpp"

//engine
#include "component.hpp"
#include "gameobject.hpp"

namespace Graphics::Components
{
	using ElypsoPhysics::PhysicsWorld;
	using ElypsoPhysics::GameObjectHandle;
	using Graphics::Shape::GameObject;

	using std::string;
	using std::shared_ptr;

	class RigidBodyComponent : public Component
	{
	public:
		RigidBodyComponent(
			shared_ptr<GameObject> owner,
			string colliderType = "BOX",
			bool isDynamic = true,
			bool useGravity = true,
			float gravityFactor = 1.0f,
			float mass = 1.0f,
			float restitution = 0.5f,
			float staticFriction = 0.5f,
			float dynamicFriction = 0.3f);

		void SetHandle(const GameObjectHandle& newHandle) { handle = newHandle; }
		void SetColliderType(const string& newColliderType);
		void SetDynamic(bool newIsDynamic) { isDynamic = newIsDynamic; }
		void EnableGravity(bool newUseGravity) { useGravity = newUseGravity; }
		void SetGravityFactor(float newGravityFactor) { gravityFactor = newGravityFactor; }
		void SetMass(float newMass) { mass = newMass; }
		void SetRestitution(float newRestitution) { restitution = newRestitution; }
		void SetStaticFriction(float newStaticFriction) { staticFriction = newStaticFriction; }
		void SetDynamicFriction(float newDynamicFriction) { dynamicFriction = newDynamicFriction; }

		const GameObjectHandle& GetHandle() const { return handle; }
		const string& GetColliderType() const { return colliderType; }
		bool IsDynamic() const { return isDynamic; }
		bool UseGravity() const { return useGravity; }
		float GetGravityFactor() const { return gravityFactor; }
		float GetMass() const { return mass; }
		float GetRestitution() const { return restitution; }
		float GetStaticFriction() const { return staticFriction; }
		float GetDynamicFriction() const { return dynamicFriction; }

		type_index GetType() const override { return typeid(RigidBodyComponent); }

		void Update(float deltaTime) override {}

	private:
		GameObjectHandle handle;
		string colliderType;
		bool isDynamic;
		bool useGravity;
		float gravityFactor;
		float mass;
		float restitution;
		float staticFriction;
		float dynamicFriction;
	};
}