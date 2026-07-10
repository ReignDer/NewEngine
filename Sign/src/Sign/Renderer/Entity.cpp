#include "Entity.h"

namespace Sign {
	Entity::Entity(const std::string& name) 
		: m_EntityName(name), 
		m_Translation(Vector3D(0.0f,0.0f,0.0f)), 
		m_Scale(Vector3D(1.0f,1.0f,1.0f)), 
		m_Rotation(Quaternion(0.0f,0.0f,0.0f,0.0f))
	{
		if (s_DefaultShader == nullptr) {
			Sign::PipelineSpecifications pSpecs = {};
			pSpecs.InputLayout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, };
			D3D12_RT_FORMAT_ARRAY rtvFormats = {};
			rtvFormats.NumRenderTargets = 1;
			rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			pSpecs.RTVFormats = rtvFormats;
			pSpecs.DepthTest = TRUE;
			pSpecs.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

			s_DefaultShader = std::make_shared<Sign::Shader>(L"Shader/VertexShader.hlsl", L"Shader/PixelShader.hlsl", nullptr, pSpecs);
		}

		m_Shader = s_DefaultShader;
		RecalculateTransform();
	}
	Entity::~Entity()
	{
		s_DefaultShader.reset();
	}
	const Mat4& Entity::GetTransform()
	{
		return m_Transform;
	}
	void Entity::RecalculateTransform()
	{
		m_Transform = Mat4::scale(m_Scale)
			* Mat4::rotateX(m_Rotation.x)
			* Mat4::rotateY(m_Rotation.y)
			* Mat4::rotateZ(m_Rotation.z)
			* Mat4::translate(m_Translation);
	}
}