struct ModelViewProjection
{
    matrix MVP;
};
 
ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

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
    
    OUT.Position = mul(ModelViewProjectionCB.MVP, IN.Position);
    OUT.Color = IN.Color;
	return OUT;
}