struct PerFrame
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};
struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<PerFrame> PerFrameCB : register(b0);
ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b1);

struct VertexPosColor
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

struct VertexShaderOutput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;
    
    float4 worldPos = mul(IN.Position, ModelViewProjectionCB.MVP);
    float4 viewPos = mul(worldPos, PerFrameCB.viewMatrix);
    OUT.Position = mul(viewPos, PerFrameCB.projectionMatrix);
    OUT.Color = IN.Color;
    return OUT;
}