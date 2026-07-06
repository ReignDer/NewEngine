#include "signpch.h"
#include "FrameBuffer.h"


namespace Sign {
	FrameBuffer::FrameBuffer(const FrameBufferSpecifications& specs, ID3D12Device2* device) : m_FrameBufferSpecifications(specs), m_Device(device)
	{
		Invalidate();
	}
	FrameBuffer::~FrameBuffer()
	{

	}
	void FrameBuffer::Invalidate()
	{
		auto context = Renderer::GetContext();
		for (auto& attch : m_ColorAttachments) {
			context->Get_CBV_SRV_UAV_Allocator().Free(attch.m_SRVCpuHandle, attch.m_SRVGpuHandle, context->GetCurrentFrameFenceValue());
		}
		m_ColorAttachments.clear();

		m_RTVHeap = D3D12Utils::CreateDescriptorHeap(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, (UINT)m_FrameBufferSpecifications.m_ColorFormats.size(), false);
		UINT rtvSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();


		for (DXGI_FORMAT format : m_FrameBufferSpecifications.m_ColorFormats) {

			FrameBufferAttachment attachment;
			attachment.m_Format = format;
			attachment.m_CurrentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

			D3D12_CLEAR_VALUE clearValue = {};
			clearValue.Color[0] = clearValue.Color[1] = clearValue.Color[2] = 0.0f;
			clearValue.Color[3] = 1.0f;
			clearValue.Format = format;

			attachment.m_Resource = D3D12Utils::CreateBuffer(
				m_Device,
				m_FrameBufferSpecifications.m_Width,
				m_FrameBufferSpecifications.m_Height,
				D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				attachment.m_Format,
				D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
				attachment.m_CurrentState,
				D3D12Utils::DefaultHeapProps,
				&clearValue

			);

			m_Device->CreateRenderTargetView(attachment.m_Resource.Get(), nullptr, rtvHandle);
			attachment.m_Handle = rtvHandle;
			rtvHandle.ptr += rtvSize;

			context->Get_CBV_SRV_UAV_Allocator().Alloc(&attachment.m_SRVCpuHandle, &attachment.m_SRVGpuHandle);

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Texture2D.MipLevels = 1;

			m_Device->CreateShaderResourceView(attachment.m_Resource.Get(), &srvDesc, attachment.m_SRVCpuHandle);

			m_ColorAttachments.push_back(attachment);
		}

		if (m_FrameBufferSpecifications.HasDepth) {
			m_DSVHeap = D3D12Utils::CreateDescriptorHeap(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

			m_DepthAttachments.m_CurrentState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

			D3D12_CLEAR_VALUE depthValue = {};
			depthValue.Format = m_FrameBufferSpecifications.m_DepthFormat;
			depthValue.DepthStencil = { 1.0f, 0 };

			m_DepthAttachments.m_Resource = D3D12Utils::CreateBuffer(
				m_Device,
				m_FrameBufferSpecifications.m_Width,
				m_FrameBufferSpecifications.m_Height,
				D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				m_FrameBufferSpecifications.m_DepthFormat,
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
				m_DepthAttachments.m_CurrentState,
				D3D12Utils::DefaultHeapProps,
				&depthValue
			);

			m_Device->CreateDepthStencilView(m_DepthAttachments.m_Resource.Get(), nullptr, m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
			m_DepthAttachments.m_Handle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
		}


	}
	void FrameBuffer::Bind()
	{
		auto cmdList = Renderer::GetCommandList();
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandle;
		for (auto& attch : m_ColorAttachments) {
			rtvHandle.push_back(attch.m_Handle);
		}
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_DepthAttachments.m_Handle;

		cmdList->OMSetRenderTargets((UINT)rtvHandle.size(), rtvHandle.data(), FALSE, m_FrameBufferSpecifications.HasDepth ? &dsvHandle : nullptr);

		D3D12_VIEWPORT vp{ 0, 0, (float)m_FrameBufferSpecifications.m_Width, (float)m_FrameBufferSpecifications.m_Height, 0.0f, 1.0f };
		D3D12_RECT scissor{ 0, 0, (LONG)m_FrameBufferSpecifications.m_Width, (LONG)m_FrameBufferSpecifications.m_Height };

		cmdList->RSSetViewports(1, &vp);
		cmdList->RSSetScissorRects(1, &scissor);

	}
	void FrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		m_FrameBufferSpecifications.m_Width = width;
		m_FrameBufferSpecifications.m_Height = height;
		Renderer::GetContext()->FlushCommandQueue();
		Invalidate();
	}

	void FrameBuffer::TransitionTo(D3D12_RESOURCE_STATES newState, uint32_t attachmentIndex)
	{
		auto& attachment = m_ColorAttachments[attachmentIndex];
		if (attachment.m_CurrentState == newState) return;
		auto cmdList = Renderer::GetCommandList();
		D3D12Utils::TransitionResource(cmdList, attachment.m_Resource, attachment.m_CurrentState, newState);
		attachment.m_CurrentState = newState;
	}

	uint32_t FrameBuffer::GetTextureID(uint32_t attachmentIndex)
	{
		return m_ColorAttachments[attachmentIndex].m_SRVCpuHandle.ptr;
	}
	void FrameBuffer::ClearAttchment(const float clearColor[4])
	{
		auto* cmdList = Renderer::GetCommandList().Get();
		static const float defaultColor[4] = { 0.f,0.f,0.f,1.0f };

		const float* color = clearColor ? clearColor : defaultColor;


		for (auto& attch : m_ColorAttachments) {
			cmdList->ClearRenderTargetView(attch.m_Handle, color, 0, nullptr);
		}

		if (m_FrameBufferSpecifications.HasDepth) {
			cmdList->ClearDepthStencilView(m_DepthAttachments.m_Handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.f, 0.f, nullptr);
		}
	}
}