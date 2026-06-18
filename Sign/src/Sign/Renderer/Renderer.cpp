#include "Renderer.h"



namespace Sign
{
	D3D12Context* Renderer::s_Context = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> Renderer::s_CommandList = nullptr;
	D3D12_VIEWPORT Renderer::s_Viewport = {};
	D3D12_RECT Renderer::s_ScissorsRect = {};
	std::shared_ptr<ConstantBuffer> Renderer::m_CameraConstantBuffer = nullptr;
	CameraData Renderer::m_CameraData = {};

	void Renderer::Init(D3D12Context* context)
	{
		s_Context = context;
		s_CommandList = s_Context->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();
		SetViewPort(0, 0, 1280, 720);

		m_CameraConstantBuffer = std::make_unique<ConstantBuffer>(sizeof(CameraData), 0);
	}

	void Renderer::ShutDown()
	{
		std::println("Renderer Shutdown");
		s_Context->FlushCommandQueue();
	}

	void Renderer::BeginScene(FLOAT* clearColor, const PerspectiveCamera& camera)
	{
		auto CommandQueue = s_Context->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		s_CommandList = CommandQueue->GetCommandList();
		auto rtv = s_Context->GetCurrentTargetView();
		auto dsv = s_Context->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
		auto backBuffer = s_Context->GetBackBuffer(s_Context->GetCurrentBackBuffer());
		FLOAT depth = 1.0f;
		//Clear
		{
			D3D12Utils::TransitionResource(
				s_CommandList,
				backBuffer,
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET);


			s_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
			s_CommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);

		}

		m_CameraData.viewMatrix = DirectX::XMMatrixTranspose(camera.GetViewMatrix());
		m_CameraData.projectionMatrix = DirectX::XMMatrixTranspose(camera.GetProjectionMatrix());
		m_CameraConstantBuffer->setData(&m_CameraData, sizeof(CameraData));
		
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const Pipeline& pipeline, const DirectX::XMMATRIX& translation)
	{
		pipeline.Bind(s_CommandList.Get()); 
		ID3D12DescriptorHeap* heaps[] = { s_Context->Get_CBV_SRV_UAV_DescriptorHeap().Get()};
		s_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);
		

		s_CommandList->SetGraphicsRootDescriptorTable(0, s_Context->GetGPUHandleAt(0));

		auto model = DirectX::XMMatrixTranspose(translation);
		s_CommandList->SetGraphicsRoot32BitConstants(1, sizeof(DirectX::XMMATRIX) / 4, &model, 0);

		vertexArray->Bind(s_CommandList);

		s_CommandList->RSSetViewports(1, &s_Viewport);
		s_CommandList->RSSetScissorRects(1, &s_ScissorsRect);

		auto rtv = s_Context->GetCurrentTargetView();
		auto dsv = s_Context->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
		s_CommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		s_CommandList->DrawIndexedInstanced(vertexArray->GetIndexBufferCount(), 1, 0, 0, 0);
	}

	void Renderer::EndScene()
	{
		auto backBuffer = s_Context->GetBackBuffer(s_Context->GetCurrentBackBuffer());
		{
			D3D12Utils::TransitionResource(
				s_CommandList,
				backBuffer,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT);

			uint64_t fenceValue = s_Context->GetCommandQueue()->ExecuteCommandList(s_CommandList);
			s_Context->SetFrameFenceValues(fenceValue);

		}
	}
	void Renderer::SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		s_Viewport = CD3DX12_VIEWPORT((float)x, (float)y, (float)w, (float)h);
		s_ScissorsRect = CD3DX12_RECT(x, y, x + w, y + h);
	}
	void Renderer::CPUSyncToGPU()
	{
		auto fenceValue = s_Context->GetCommandQueue()->ExecuteCommandList(s_CommandList);
		s_Context->GetCommandQueue()->WaitForFenceValue(fenceValue);
	}
	void Renderer::ResizeDepthbuffer(int width, int height)
	{
		s_Context->ResizeDepthBuffer(width, height);
	}
	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		SetViewPort(0, 0, width, height);
		ResizeDepthbuffer(width, height);
	}
}
