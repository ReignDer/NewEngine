#include "Renderer.h"


namespace Sign
{
	D3D12Context* Renderer::s_Context = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> Renderer::s_CommandList = nullptr;

	void Renderer::Init(D3D12Context* context)
	{
		s_Context = context;
		s_CommandList = s_Context->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)->GetCommandList();
	}

	void Renderer::ShutDown()
	{
		
	}

	void Renderer::BeginScene(FLOAT* clearColor)
	{
		auto backBuffer = s_Context->GetBackBuffer(s_Context->GetCurrentBackBuffer());
		//Clear
		{
			D3D12Utils::TransitionResource(
				s_CommandList,
				backBuffer,
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET);

			auto rtv = s_Context->GetCurrentTargetView();
			s_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

		}
	}

	void Renderer::Submit()
	{
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
	void Renderer::OnWindowResize()
	{
	}
}
