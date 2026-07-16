#pragma once
#include <directx/d3d12.h>
#include <wrl.h>
#include <DirectXTex.h>
#include <filesystem>
#include "Renderer.h"

namespace Sign {
	enum class ImageFormat {
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F
	};
	struct TextureSpecifications {
		uint32_t Width;
		uint32_t Height;
		ImageFormat Format = ImageFormat::None;
		bool GenerateMips = true;

	};
	class Texture
	{
	public:
		virtual ~Texture() = default;


		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual const std::string& GetPath() const = 0;

		virtual void Bind(uint32_t slot = 5) const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const Texture& other) const = 0;

	};

	class Texture2D : public Texture {
	public:
		Texture2D(const TextureSpecifications& specs);
		Texture2D(std::string_view path);

		virtual ~Texture2D() = default;

		// Inherited via Texture
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		const std::string& GetPath() const override { return m_Path; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return m_SRVGpuHandle; }
		void Bind(uint32_t slot) const override;
		void SetData(void* data, uint32_t size) override;
		bool IsLoaded() const override;
		bool operator==(const Texture& other) const override;
	private:
		TextureSpecifications m_Specifications;
		std::string m_Path;
		uint32_t m_Width, m_Height;
		bool m_IsLoaded = false;
		DXGI_FORMAT m_InternalFormat, m_DataFormat;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadBuffer;

		D3D12_CPU_DESCRIPTOR_HANDLE m_SRVCpuHandle{};
		D3D12_GPU_DESCRIPTOR_HANDLE m_SRVGpuHandle{};
		
	};
}

