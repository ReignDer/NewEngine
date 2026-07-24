struct PerFrame
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float time;
};
struct ModelTransform
{
    matrix MT;
};

ConstantBuffer<PerFrame> PerFrameCB : register(b0);
ConstantBuffer<ModelTransform> ModelTransformCB : register(b1);


struct VertexPosColor
{
	float4 Position : POSITION;
	float3 Normals  : NORMAL;
	float4 Color    : COLOR;
	float2 TexCoord : TEXCOORD;
    
    uint FaceID     : FACEID;
};

struct VertexShaderOutput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
    float4 LocalPosition : TEXCOORD0;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD1;
    nointerpolation uint FaceID : FACEID;
};


VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;
   
    float4 worldPos = mul(IN.Position, ModelTransformCB.MT);
    float4 viewPos = mul(worldPos, PerFrameCB.viewMatrix);
    OUT.LocalPosition = IN.Position;
    OUT.Position = mul(viewPos, PerFrameCB.projectionMatrix);
    OUT.Color = IN.Color;
    OUT.FaceID = IN.FaceID;
    
    OUT.WorldNormal = mul(IN.Normals, (float3x3)ModelTransformCB.MT);
    OUT.TexCoord = IN.TexCoord;
    return OUT;
}