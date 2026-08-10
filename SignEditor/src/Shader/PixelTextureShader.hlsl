
struct Light
{
    float3 Position;
    float3 Direction;
    float3 Color;
    
    float Type;
    float Range;
    float Intensity;
    float InnerConeCos;
    float OuterConeCos;
    float2 _pad;
};

StructuredBuffer<Light> Lights : register(t1);
cbuffer LightcountCB : register(b5)
{
    uint LightCount;
}

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
    float4 texColor = AlbedoTexture.Sample(SampleTexture, IN.TexCoord);
    
    float3 N = normalize(IN.WorldNormal);
    float3 lighting = float3(0.05, 0.05, 0.05);
    
    for (uint i = 0; i < LightCount; i++)
    {
        Light L = Lights[i];
        float3 toLight;
        float attenuation = 1.0;
        
        
        if(L.Type < 0.5)
        {
            toLight = -normalize(L.Direction);
        }
        else
        {
            float3 delta = L.Position - IN.LocalPosition.xyz;
            float dist = length(delta);
            toLight = delta / max(dist, 0.0001);
            attenuation = saturate(1.0 - (dist / L.Range));
            attenuation *= attenuation;
            
            if (L.Type < 1.5)
            {
                float cosAngle = dot(-toLight, normalize(L.Direction));
                float spotFactor = saturate((cosAngle - L.OuterConeCos) / max(L.InnerConeCos - L.OuterConeCos, 0.0001));
                attenuation *= spotFactor;
            }

        }
        float NdotL = saturate(dot(N, toLight));
        lighting += L.Color * L.Intensity * NdotL * attenuation;
    }
    OUT.Color = baseColor * texColor * float4(lighting, 1.0);
    return OUT;
}

/*float4 main(PixelShaderInput IN) : SV_TARGET
{
   
    return IN.Color;
}*/