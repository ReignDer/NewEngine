#include "Shader.h"
namespace Sign {
	Shader::Shader(const WCHAR* vertexSrc, const WCHAR* pixelSrc, const WCHAR* computeSrc) 
		: m_VertexPath(vertexSrc), m_PixelPath(pixelSrc), m_ComputePath(computeSrc)
	{
		Compile();
	}
	void Shader::Compile()
	{
		CompileShader(m_VertexPath, "main", "vs_6_8", &vertexShaderBlob);
		CompileShader(m_PixelPath, "main", "ps_6_8", &pixelShaderBlob);
		CompileShader(m_PixelPath, "main", "cs_6_8", &pixelShaderBlob);
	
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
			assert(false && "Shader Compilation Failed");
	}
}