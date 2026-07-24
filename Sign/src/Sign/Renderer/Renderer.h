#pragma once
#include <directx/d3d12.h>
#include <print>
#include <memory>	
#include "Sign/D3D12Context.h"
#include "Sign/D3D12Utils.h"
#include "Sign/Shader/Shader.h"
#include "Sign/Buffers/VertexArray.h"
#include "Sign/Buffers/ConstantBuffer.h"
#include "Sign/Renderer/Mesh.h"
#include "PerspectiveCamera.h"
#include "Sign/Math/SignMath.h"
#include "Sign/Buffers/FrameBuffer.h"
#include "Texture.h"
namespace Sign {
	struct alignas(256) CameraData {
		Mat4 viewMatrix;
		Mat4 projectionMatrix;
		float time;
	};
	class Renderer
	{
	public:
		static void								Init(D3D12Context* context);
		static void								ShutDown();
		static void								BeginInitializationCommand();
		static void								EndInitializationCommand();
		static void								SetCommandlist(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		static void								BeginFrame();
		static void								RenderClearCommand(FLOAT* clearColor);
		static void								BeginScene(const PerspectiveCamera& camera);
		static void								Submit(const std::shared_ptr<VertexArray>& vertexArray, const Shader& shader, const Mat4& transform);
		static void								Submit(const std::shared_ptr<VertexArray>& vertexArray, const Shader& shader, const Mat4& transform, const Texture2D& texture);
		static void								Submit(const std::shared_ptr<VertexArray>& vertexArray, const Shader& shader, const Mat4& transform, uint32_t entity, uint32_t selectedEntityID, uint32_t selectedFace);
		static void								EndScene();

		static void								OnWindowResize(uint32_t width, uint32_t height);
		static void								SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
		static void								CPUSyncToGPU();
		static void								Resizebuffers(int width, int height);
		static void								RegisterFrameBuffers(std::string_view name, std::shared_ptr<FrameBuffer> fb);
		static std::shared_ptr<FrameBuffer>		GetFrameBuffer(std::string_view name);


		static const std::unordered_map<std::string, std::shared_ptr<FrameBuffer>> GetAllFrameBuffers();

		static D3D12Context*										GetContext();
		static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>	GetCommandList();
		static const D3D12_VIEWPORT const							GetViewPort();
		static std::shared_ptr<Texture2D>							GetWhiteTexture();
	};
}

