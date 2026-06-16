#pragma once
#include <directx/d3d12.h>
#include "Sign/D3D12Context.h"
#include "Sign/D3D12Utils.h"
namespace Sign {
	class Renderer
	{
	public:
		static void Init(D3D12Context* context);
		static void ShutDown();

		static void BeginScene(FLOAT* clearColor);
		static void Submit();
		static void EndScene();

		static void OnWindowResize();

	private:
		static D3D12Context* s_Context;
		static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> s_CommandList;
	};
}

