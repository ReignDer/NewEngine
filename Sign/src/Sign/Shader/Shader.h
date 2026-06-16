#pragma once
#include <directx/d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
namespace Sign {
	class Shader
	{
	public:
		Shader(const WCHAR* vertexSrc, const WCHAR* pixelSrc, const WCHAR* computeSrc);

		void Compile();

		void CompileShader(const WCHAR* path, const char* entryPoint, const char* target, ID3DBlob** blob);

		Microsoft::WRL::ComPtr<ID3DBlob> GetVertexShaderBlob() { return vertexShaderBlob; }
		Microsoft::WRL::ComPtr<ID3DBlob> GetPixelShaderBlob() { return pixelShaderBlob; }
		Microsoft::WRL::ComPtr<ID3DBlob> GetComputeShaderBlob() { return computeShaderBlob; }

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> computeShaderBlob;

		const WCHAR* m_VertexPath;
		const WCHAR* m_PixelPath;
		const WCHAR* m_ComputePath;

		DirectX::XMMATRIX m_Mat4;
	};
}

