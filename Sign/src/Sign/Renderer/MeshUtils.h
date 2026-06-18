#pragma once
#include <directx/d3d12.h>
#include <DirectXMath.h>

struct VertexPosColor
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Color;
};

//Indices
static WORD trianlgeIndices[3] = { 0,1,2 };
static WORD quadIndices[6] = { 0,1,2 , 0,2,3 };
static WORD cubeIndices[36] =
{	
	0,1,2 , 0,2,3,
	4,6,5 , 4,7,6,
	4,5,1 , 4,1,0,
	3,2,6 , 3,6,7,
	1,5,6 , 1,6,2,
	4,0,3 , 4,3,7
};