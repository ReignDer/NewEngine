

struct PixelShaderInput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
    float4 LocalPosition : TEXCOORD0;
    float3 WorldNormal : NORMAL;
    float2 TexCoord : TEXCOORD1;
    nointerpolation uint FaceID : FACEID;

};

Texture2D AlbedoTexture : register(t0);
SamplerState SampleTexture : register(s0);

struct PixelShaderOutput
{
    float4 Color : SV_Target0;
};


PixelShaderOutput main(PixelShaderInput IN)
{
    PixelShaderOutput OUT;
    float4 baseColor = IN.Color;
    float4 texColor = AlbedoTexture.Sample(SampleTexture, IN.TexCoord * 0.5);
    OUT.Color = baseColor * texColor;
    return OUT;
}

/*float4 main(PixelShaderInput IN) : SV_TARGET
{
   
    return IN.Color;
}*/