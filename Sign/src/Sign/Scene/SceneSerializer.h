#pragma once
#include "Sign/Scene/Scene.h"
#include "Sign/Scene/EntityECS.h"
namespace Sign {
	class SceneSerializer
	{
	public:
		SceneSerializer(const std::shared_ptr<Scene>& scene);

		void Serialize(std::string_view filepath);
		void SerializeRuntime(std::string_view filepath);

		bool Deserialize(std::string_view filepath);
		bool DeserializeRuntime(std::string_view filepath);

	private:
		std::shared_ptr<Scene> m_Scene;
	};
}

