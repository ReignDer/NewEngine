#pragma once
#include "directx/d3d12.h"
#include "wrl.h"
#include "Sign/D3D12Utils.h"

namespace Sign {
	struct PixelData {
		int32_t entityID = -1;
		int32_t faceID = -1;
	};

	struct FrameBufferAttachment {
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
		D3D12_CPU_DESCRIPTOR_HANDLE m_Handle = {};
		D3D12_CPU_DESCRIPTOR_HANDLE m_SRVCpuHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_SRVGpuHandle = {};
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		D3D12_RESOURCE_STATES m_CurrentState = D3D12_RESOURCE_STATE_COMMON;

	};
	struct FrameBufferSpecifications {
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		std::vector<DXGI_FORMAT> m_ColorFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
		DXGI_FORMAT m_DepthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		bool HasDepth = true;
		uint32_t m_Samples = 1;
	};
	class FrameBuffer
	{
	public:
		FrameBuffer(const FrameBufferSpecifications& specs, ID3D12Device2* device);
		~FrameBuffer();


		void Invalidate();

		void Bind();
		void Resize(uint32_t width, uint32_t height);
		void TransitionTo(ID3D12GraphicsCommandList2* cmdList,D3D12_RESOURCE_STATES newState, uint32_t attachmentIndex = 0);
		UINT64 GetTextureID(uint32_t attachmentIndex = 0);

		//TODO: Have mouse clicking detect if a specific entity gets clicked in the viewport
		PixelData ReadPixel(uint32_t attachmentID, int x, int y);
		void ClearAttchment(const float clearColor[4] = nullptr);

		const FrameBufferSpecifications& GetSpecifications() const { return m_FrameBufferSpecifications; }

	private:
		ID3D12Device2* m_Device;
		std::vector<FrameBufferAttachment> m_ColorAttachments;
		FrameBufferAttachment m_DepthAttachments;
		FrameBufferSpecifications m_FrameBufferSpecifications;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;

		//Read from GPU to CPU
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ReadBackBuffer;

	};
}

