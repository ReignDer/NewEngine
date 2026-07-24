#pragma once
#include <directx/d3d12.h>
#include "Sign/Math/SignMath.h"

struct VertexPosColor
{
	Sign::Vector3D Position;
	Sign::Vector3D Normals;
	Sign::Vector3D Color;
	Sign::Vector2D TexCoord;
	uint32_t FaceID;

};

//Indices
static uint32_t trianlgeIndices[3] = { 0,1,2 };
static uint32_t quadIndices[6] = { 0,1,2 , 0,2,3 };
static uint32_t cubeIndices[36] =
{	
	0,1,2 , 0,2,3,
	4,6,5 , 4,7,6,
	4,5,1 , 4,1,0,
	3,2,6 , 3,6,7,
	1,5,6 , 1,6,2,
	4,0,3 , 4,3,7
};