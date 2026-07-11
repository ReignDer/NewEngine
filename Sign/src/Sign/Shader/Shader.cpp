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
		std::filesystem::path exeDir = GetExePath();
		std::filesystem::path absoluteVertexPath = exeDir / m_VertexPath;
		std::filesystem::path absolutePixelPath = exeDir / m_PixelPath;
		CompileShader(absoluteVertexPath.c_str(), "main", "vs_5_1", &vertexShaderBlob);
		CompileShader(absolutePixelPath.c_str(), "main", "ps_5_1", &pixelShaderBlob);

		if (m_ComputePath) {
			std::filesystem::path absoluteComputePath = exeDir / m_ComputePath;
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
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				std::cerr << "HLSL Compile Error:\n" << (char*)errorBlob->GetBufferPointer() << std::endl;
			}
			else
			{
				// Sometimes D3DCompileFromFile fails because the file doesn't exist at the path
				std::wcerr << L"Shader file not found or inaccessible at: " << path << std::endl;
			}
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

	std::shared_ptr<Shader> Shader::GetDefault()
	{
		if (s_DefaultShader == nullptr) {
			Sign::PipelineSpecifications pSpecs = {};
			pSpecs.InputLayout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, };
			D3D12_RT_FORMAT_ARRAY rtvFormats = {};
			rtvFormats.NumRenderTargets = 2;
			rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvFormats.RTFormats[1] = DXGI_FORMAT_R32G32_SINT;
			pSpecs.RTVFormats = rtvFormats;
			pSpecs.DepthTest = TRUE;
			pSpecs.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

			s_DefaultShader = std::make_shared<Sign::Shader>(L"Shader/VertexShader.hlsl", L"Shader/PixelShader.hlsl", nullptr, pSpecs);
		}
		return s_DefaultShader;
	}

	std::filesystem::path Shader::GetExePath()
	{
		char exePath[MAX_PATH];
		GetModuleFileName(nullptr, exePath, MAX_PATH);
		return std::filesystem::path(exePath).parent_path();
	}

}