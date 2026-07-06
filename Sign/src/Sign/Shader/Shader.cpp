#include "Shader.h"



namespace Sign {
	Shader::Shader(const WCHAR* vertexSrc, const WCHAR* pixelSrc, const WCHAR* computeSrc, const PipelineSpecifications& specs)
		: m_VertexPath(vertexSrc), m_PixelPath(pixelSrc), m_ComputePath(computeSrc)
	{
		Compile();
		SetGraphicsPipeline(specs);
	}
	Shader::~Shader()
	{
		vertexShaderBlob.Reset();
		pixelShaderBlob.Reset();
		computeShaderBlob.Reset();

		m_GraphicsPipeline.reset();
	}
	void Shader::Compile()
	{
		std::filesystem::path absoluteVertexPath = std::filesystem::absolute(m_VertexPath);
		std::filesystem::path absolutePixelPath = std::filesystem::absolute(m_PixelPath);
		CompileShader(absoluteVertexPath.c_str(), "main", "vs_5_1", &vertexShaderBlob);
		CompileShader(absolutePixelPath.c_str(), "main", "ps_5_1", &pixelShaderBlob);

		if (m_ComputePath) {
			std::filesystem::path absoluteComputePath = std::filesystem::absolute(m_ComputePath);
			CompileShader(absoluteComputePath.c_str(), "main", "cs_5_1", &computeShaderBlob);
		}
	
	}
	void Shader::CompileShader(const WCHAR* path, const char* entryPoint, const char* target, ID3DBlob** blob)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		UINT compileFlags = 0;
#ifdef SIGN_DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT hr = D3DCompileFromFile(
			path,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint,
			target,
			compileFlags,
			0,
			blob,
			&errorBlob);

		if (FAILED(hr))
		{
			assert(false && "Shader Compilation Failed");
		}

	}

	void Shader::SetGraphicsPipeline(const PipelineSpecifications& spec)
	{
		PipelineSpecifications pSpecs = spec;
		pSpecs.VertexBlob = vertexShaderBlob.Get();
		pSpecs.PixelBlob = pixelShaderBlob.Get();

		m_GraphicsPipeline = std::make_unique<Sign::GraphicsPipeline>(pSpecs);
	}

	void Shader::Bind(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>& commandList) const
	{
		m_GraphicsPipeline->Bind(commandList);
	}

	std::filesystem::path Shader::GetExePath()
	{
		char exePath[MAX_PATH];
		GetModuleFileName(nullptr, exePath, MAX_PATH);
		return std::filesystem::path(exePath).parent_path();
	}

}