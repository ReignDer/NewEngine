#pragma once
#include <directx/d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <print>
#include <memory>
#include <iostream>
#include <filesystem>
#include <cassert>

#include "Sign/Renderer/Pipeline.h"


namespace Sign {
	class Shader
	{
	public:
		Shader(const WCHAR* vertexSrc, const WCHAR* pixelSrc, const WCHAR* computeSrc = nullptr, const PipelineSpecifications& specs = PipelineSpecifications{});
		Shader(std::string_view name, const WCHAR* vertexSrc, const WCHAR* pixelSrc, const WCHAR* computeSrc = nullptr, const PipelineSpecifications& specs = PipelineSpecifications{});
		~Shader();
		void Compile();

		void CompileShader(const WCHAR* path, const char* entryPoint, const char* target, ID3DBlob** blob);

		void SetGraphicsPipeline(const PipelineSpecifications& spec);

		void Bind(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>& commandList) const;

		std::filesystem::path GetExePath();

		Microsoft::WRL::ComPtr<ID3DBlob> GetVertexShaderBlob() { return vertexShaderBlob; }
		Microsoft::WRL::ComPtr<ID3DBlob> GetPixelShaderBlob() { return pixelShaderBlob; }
		Microsoft::WRL::ComPtr<ID3DBlob> GetComputeShaderBlob() { return computeShaderBlob; }

		const std::string_view GetName() const { return m_Name; }

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> computeShaderBlob;

		std::unique_ptr<GraphicsPipeline> m_GraphicsPipeline;

		const WCHAR* m_VertexPath;
		const WCHAR* m_PixelPath;
		const WCHAR* m_ComputePath;

		std::string m_Name;
	};

	class ShaderLibrary {
	public:

		ShaderLibrary();

		void Add(std::string_view name, const std::shared_ptr<Shader>& shader);
		void Add(const std::shared_ptr<Shader>& shader);

		std::shared_ptr<Shader> Load(const WCHAR* vertexPath, const WCHAR* pixelPath, const PipelineSpecifications& specs);
		std::shared_ptr<Shader> Load(std::string_view name, const WCHAR* vertexPath, const WCHAR* pixelPath, const PipelineSpecifications& specs);

		std::shared_ptr<Shader> Get(std::string_view name);
		std::shared_ptr<Shader> GetDefault();

		bool Exists(std::string_view name) const;

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}

