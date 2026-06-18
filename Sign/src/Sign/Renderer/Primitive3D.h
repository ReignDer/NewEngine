#pragma once
#include <array>
#include <memory>
#include <vector>
#include "Sign/Renderer/Mesh.h"
#include "Sign/Renderer/MeshUtils.h"
namespace Sign {
	namespace Primitive {
		static constexpr std::array<DirectX::XMFLOAT3, 8> cubePosition = { {
			{-1.0,-1.0,-1.0} , {-1.0,1.0,-1.0 },
			{ 1.0,1.0,-1.0 } , { 1.0,-1.0,-1.0 },
			{ -1.0,-1.0,1.0 }, { -1.0,1.0,1.0 },
			{ 1.0,1.0,1.0 }  , { 1.0,-1.0,1.0 }
		} };
		static constexpr std::array<DirectX::XMFLOAT3, 4> planePosition = { {
			{-1.0,0.0,1.0 } , 
			{ 1.0,0.0,1.0 } ,
			{ 1.0,0.0,-1.0 }, 
			{ -1.0,0.0,-1.0 }
		} };
		class Cube3D
		{
		public:
			static std::shared_ptr<Mesh> Create(
				const DirectX::XMFLOAT3& translation = {0.0f,0.0f,0.0f},
				const DirectX::XMFLOAT3& scale = {1.0f,1.0f,1.0f},
				const DirectX::XMFLOAT4& rotation = {0.0f, 0.0f, 0.0f,0.0f},
				const std::array<DirectX::XMFLOAT3,8>& color = {{
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f}}}
				);
		};

		class Sphere
		{
		public:
			static std::shared_ptr<Mesh> Create(
				const DirectX::XMFLOAT3& translation = { 0.0f,0.0f,0.0f },
				const DirectX::XMFLOAT3& scale = { 1.0f,1.0f,1.0f },
				const DirectX::XMFLOAT4& rotation = { 0.0f, 0.0f, 0.0f,0.0f },
				const std::array<DirectX::XMFLOAT3, 8>& color = { {
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f}} }
					);
		};

		class Plane {
		public:
			static std::shared_ptr<Mesh> Create(
				const DirectX::XMFLOAT3& translation = { 0.0f,-5.0f,0.0f },
				const DirectX::XMFLOAT3& scale = { 10.0f,1.0f,10.0f },
				const DirectX::XMFLOAT4& rotation = { 0.0f, 0.0f, 0.0f,0.0f },
				const std::array<DirectX::XMFLOAT3, 4>& color = { {
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f},
					{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f}} }
					);
		};
	};
}

