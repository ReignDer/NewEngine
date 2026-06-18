#pragma once
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <wrl.h>
#include <memory>
#include "Sign/Shader/Shader.h"
namespace Sign {

	struct PipelineSpecifications {
		std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE Topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		std::shared_ptr<Shader> Shader;
		BOOL WireFrame = FALSE;
		BOOL DepthTest = TRUE;
		D3D12_RT_FORMAT_ARRAY RTVFormats= {};
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_D32_FLOAT;
		UINT NumRenderTargets = 1;

	};
	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER RasterizerState;
		CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendState;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;

	};

	struct ComputePipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_CS CS;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) const = 0;

		virtual void CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device2> device) = 0;
		virtual void CreatePSO(Microsoft::WRL::ComPtr<ID3D12Device2> device, const PipelineSpecifications& specs) = 0;

	protected:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

	};

	class GraphicsPipeline final : public Pipeline {
	public:
		GraphicsPipeline(Microsoft::WRL::ComPtr<ID3D12Device2> device, const PipelineSpecifications& specs);
		void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) const override;
		void CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device2> device) override;
		void CreatePSO(Microsoft::WRL::ComPtr<ID3D12Device2> device, const PipelineSpecifications& specs) override;
	};

	class ComputePipeline final : public Pipeline {
	public:
		ComputePipeline(Microsoft::WRL::ComPtr<ID3D12Device2> device, const PipelineSpecifications& specs);
		void Bind(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) const override;
		void CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device2> device) override;
		void CreatePSO(Microsoft::WRL::ComPtr<ID3D12Device2> device, const PipelineSpecifications& specs) override;
	};
}

