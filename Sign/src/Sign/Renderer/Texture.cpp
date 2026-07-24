#include "signpch.h"
#include "Texture.h"
#include "Renderer.h"
namespace Sign {
	namespace Utils {
		static DXGI_FORMAT ToDXGI(ImageFormat format) {
			switch (format) {
			case ImageFormat::R8: return DXGI_FORMAT_R8_UNORM;
			case ImageFormat::RGB8:
			case ImageFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			default: return DXGI_FORMAT_UNKNOWN;
			}
		}
	}
	Texture2D::Texture2D(const TextureSpecifications& specs) 
		: m_Specifications(specs), m_Width(m_Specifications.Width), m_Height(m_Specifications.Height)
	{
		m_DataFormat = Utils::ToDXGI(m_Specifications.Format);
		m_InternalFormat = m_DataFormat;

		auto device = Renderer::GetContext()->GetDevice();

		m_Resource = D3D12Utils::CreateBuffer(device.Get(), 
			m_Width, 
			m_Height, 
			D3D12_RESOURCE_DIMENSION_TEXTURE2D, 
			m_DataFormat, 
			D3D12_RESOURCE_FLAG_NONE, 
			D3D12_RESOURCE_STATE_COPY_DEST, 
			D3D12Utils::DefaultHeapProps);

		Renderer::GetContext()->Get_CBV_SRV_UAV_Allocator().Alloc(&m_SRVCpuHandle, &m_SRVGpuHandle);

		D3D12Utils::CreateShaderResourceView(device.Get(), m_Resource.Get(), m_InternalFormat, &m_SRVCpuHandle);

		m_IsLoaded = true;
	}
	Texture2D::Texture2D(std::string_view path) : m_Path(path)
	{
		std::wstring widePath = std::filesystem::path(path).wstring();

		DirectX::ScratchImage srcImage;

		HRESULT hr;

		if (path.ends_with(".dds")) {
			hr = DirectX::LoadFromDDSFile(widePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, srcImage);
		}
		else if(path.ends_with(".tga"))
			hr = DirectX::LoadFromTGAFile(widePath.c_str(), DirectX::TGA_FLAGS_NONE, nullptr, srcImage);
		else
			hr = DirectX::LoadFromWICFile(widePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, srcImage);
		
		if (FAILED(hr)) {
			m_IsLoaded = false;
			return;
		}
		
		DirectX::ScratchImage timage;
		const DirectX::TexMetadata* metaData = &srcImage.GetMetadata();

		if (m_Specifications.GenerateMips && metaData->mipLevels == 1) {
			hr = DirectX::GenerateMipMaps(*srcImage.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, timage);
			if (SUCCEEDED(hr))
			{
				srcImage = std::move(timage);
				metaData = &srcImage.GetMetadata();
				
			}
		}
		m_Width = static_cast<uint32_t>(metaData->width);
		m_Height = static_cast<uint32_t>(metaData->height);
		m_DataFormat = metaData->format;
		m_InternalFormat = metaData->format;
		
		hr = DirectX::CreateTexture(Renderer::GetContext()->GetDevice().Get(), *metaData, &m_Resource);
		
		if (FAILED(hr)) {
			m_IsLoaded = false;
			return;
		}
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		hr = DirectX::PrepareUpload(Renderer::GetContext()->GetDevice().Get(), srcImage.GetImages(), srcImage.GetImageCount(), *metaData, subresources);

		if (FAILED(hr)) {
			m_IsLoaded = false;
			return;
		}
		UINT64 uploadBufferSize = GetRequiredIntermediateSize(
			m_Resource.Get(), 0, static_cast<UINT>(subresources.size()));
		CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		hr = Renderer::GetContext()->GetDevice()->CreateCommittedResource(
			&D3D12Utils::UploadHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&uploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer));
		if (FAILED(hr)) {
			m_IsLoaded = false;
			return;
		}
		auto cmdList = Renderer::GetCommandList();
		UpdateSubresources(
			cmdList.Get(),
			m_Resource.Get(),
			m_UploadBuffer.Get(),
			0, 0,
			static_cast<UINT>(subresources.size()),
			subresources.data());

		D3D12Utils::TransitionResource(cmdList, m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


		Renderer::GetContext()->Get_CBV_SRV_UAV_Allocator().Alloc(&m_SRVCpuHandle, &m_SRVGpuHandle);

		D3D12Utils::CreateShaderResourceView(Renderer::GetContext()->GetDevice().Get(), m_Resource.Get(), m_InternalFormat, &m_SRVCpuHandle);

		m_IsLoaded = true;
	}

	void Texture2D::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t slot) const
	{
		cmdList->SetGraphicsRootDescriptorTable(slot, m_SRVGpuHandle);
	}
	void Texture2D::SetData(void* data, uint32_t size)
	{
		if (!m_Resource) return;
		UINT64 uploadBufferSize = GetRequiredIntermediateSize(
			m_Resource.Get(), 0, 1);
		CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		HRESULT hr = Renderer::GetContext()->GetDevice()->CreateCommittedResource(
			&D3D12Utils::UploadHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&uploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer));
		if (FAILED(hr)) {
			m_IsLoaded = false;
			return;
		}


		uint32_t bytesPerPixel = size / (m_Width * m_Height);
		uint32_t rowPitch = m_Width * bytesPerPixel;

		D3D12_SUBRESOURCE_DATA subresource = {};
		subresource.pData = data;
		subresource.RowPitch = rowPitch;
		subresource.SlicePitch = rowPitch * m_Height;

		auto cmdList = Renderer::GetCommandList();
		UpdateSubresources(
			cmdList.Get(),
			m_Resource.Get(),
			m_UploadBuffer.Get(),
			0, 0,
			1,
			&subresource);

		D3D12Utils::TransitionResource(cmdList, m_Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
	bool Texture2D::IsLoaded() const
	{
		return m_IsLoaded;
	}
	bool Texture2D::operator==(const Texture& other) const
	{
		return false;
	}
}