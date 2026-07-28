#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include "Sign/Scene/ECS/Components.h"

namespace Sign {
	namespace PhysicsUtils {
		inline reactphysics3d::BodyType RigidBody3DTypeToReactType(RigidBody3D::BodyType type) {
			switch (type) {
				case RigidBody3D::BodyType::Static: return reactphysics3d::BodyType::STATIC;
				case RigidBody3D::BodyType::Dynamic: return reactphysics3d::BodyType::DYNAMIC;
				case RigidBody3D::BodyType::Kinematic: return reactphysics3d::BodyType::KINEMATIC;
			}

			return reactphysics3d::BodyType::STATIC;
		}
	}
}