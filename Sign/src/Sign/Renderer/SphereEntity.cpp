#include "signpch.h"
#include "SphereEntity.h"

namespace Sign {
	SphereEntity::SphereEntity()
	{
		Sign::PipelineSpecifications pSpecs = {};
		pSpecs.InputLayout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, };
		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pSpecs.RTVFormats = rtvFormats;
		pSpecs.DepthTest = TRUE;
		pSpecs.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		m_Shader = std::make_shared<Sign::Shader>(L"Shader/VertexShader.hlsl", L"Shader/IcospherePixelShader.hlsl", nullptr, pSpecs);

		if (s_DefaultSphereMesh == nullptr) {
			s_DefaultSphereMesh = Primitive::Sphere::Create();
		}
		SetMesh(s_DefaultSphereMesh);
	}
	SphereEntity::~SphereEntity()
	{
		s_DefaultSphereMesh.reset();
	}
	void SphereEntity::OnUpdate(Timestep dt)
	{
	}
}
