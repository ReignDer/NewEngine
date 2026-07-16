#include "Pipeline.h"

#include "Sign/Renderer/Renderer.h"
namespace Sign {
	GraphicsPipeline::GraphicsPipeline(const PipelineSpecifications& specs)
	{
		CreateRootSignature(Renderer::GetContext()->GetDevice());
		CreatePSO(Renderer::GetContext()->GetDevice(), specs);
	}
	void GraphicsPipeline::Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) const
	{
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		commandList->SetPipelineState(m_PipelineState.Get());
	}

	void GraphicsPipeline::CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device2> device)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		//DENY ANYTHING THAT ISNT NEED RN
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

			//CAREFUL, ONLY ALLOW WHEN READING TEXTURES ... REMOVE ONLY ON APPLYING TEXTURES OR CHANGING LIGHTING/COLOR THROUGH CONSTANT BUFFER
			//D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_PARAMETER1 rootParameters[6];

		CD3DX12_DESCRIPTOR_RANGE1 cbvRange[1];
		cbvRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

		rootParameters[0].InitAsDescriptorTable(1, &cbvRange[0], D3D12_SHADER_VISIBILITY_VERTEX);

		rootParameters[1].InitAsConstants(sizeof(Mat4) / 4, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[2].InitAsConstants(1, 2, 0, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[3].InitAsConstants(1, 3, 0, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[4].InitAsConstants(1, 4, 0, D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_DESCRIPTOR_RANGE1 srvRange[1];
		srvRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		rootParameters[5].InitAsDescriptorTable(1, &srvRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_STATIC_SAMPLER_DESC linearSampler(
			0,                                
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, 
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, &linearSampler, rootSignatureFlags);
		//rootSignatureDescription.Init_1_1(0, nullptr, 0, nullptr, rootSignatureFlags);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, featureData.HighestVersion, &rootSignatureBlob, &errorBlob);
		device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	}
	void GraphicsPipeline::CreatePSO(Microsoft::WRL::ComPtr<ID3D12Device2> device, const PipelineSpecifications& specs)
	{
		PipelineStateStream pipelineStateStream = {};
		// If you completely disable depth testing, change your DSVFormat to UNKNOWN:

		pipelineStateStream.pRootSignature = m_RootSignature.Get();
		pipelineStateStream.InputLayout = { specs.InputLayout.data(), (UINT)specs.InputLayout.size() };
		pipelineStateStream.PrimitiveTopologyType = specs.Topology;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(specs.VertexBlob);
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(specs.PixelBlob);

		auto rasterDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
		pipelineStateStream.RasterizerState = rasterDesc;
		pipelineStateStream.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		CD3DX12_DEPTH_STENCIL_DESC depthDesc(D3D12_DEFAULT);
		depthDesc.DepthEnable = specs.DepthTest; // Turn off depth completely
		depthDesc.StencilEnable = specs.DepthTest;
		pipelineStateStream.DepthStencilState = depthDesc;

		pipelineStateStream.DSVFormat = specs.DSVFormat;
		pipelineStateStream.RTVFormats = specs.RTVFormats;


		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
		};
		device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
	}
	

	ComputePipeline::ComputePipeline(const PipelineSpecifications& specs)
	{
		CreateRootSignature(Renderer::GetContext()->GetDevice());
		CreatePSO(Renderer::GetContext()->GetDevice(), specs);
	}
	void ComputePipeline::Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) const
	{
		commandList->SetComputeRootSignature(m_RootSignature.Get());
		commandList->SetPipelineState(m_PipelineState.Get());
	}
	void ComputePipeline::CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device2> device)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		//DENY ANYTHING THAT ISNT NEED RN
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

		CD3DX12_DESCRIPTOR_RANGE1 backBufferRange[1];
		backBufferRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0);

		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsDescriptorTable(1, &backBufferRange[0]);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);
		//rootSignatureDescription.Init_1_1(0, nullptr, 0, nullptr, rootSignatureFlags);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, featureData.HighestVersion, &rootSignatureBlob, &errorBlob);
		device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	}
	void ComputePipeline::CreatePSO(Microsoft::WRL::ComPtr<ID3D12Device2> device, const PipelineSpecifications& specs)
	{
		ComputePipelineStateStream pipelineStateStream = {};

		pipelineStateStream.pRootSignature = m_RootSignature.Get();
		pipelineStateStream.CS = CD3DX12_SHADER_BYTECODE(specs.ComputeBlob);

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(ComputePipelineStateStream), &pipelineStateStream };
		device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
	}
}