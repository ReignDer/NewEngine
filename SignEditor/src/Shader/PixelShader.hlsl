struct PixelShaderInput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
    float4 LocalPosition : TEXCOORD0;
};

struct EntityID
{
    uint entityID;
};
ConstantBuffer<EntityID> EntityIDCB : register(b2);

struct PixelShaderOutput
{
    float4 Color : SV_Target0;
    int2 IDs : SV_Target1;
};

PixelShaderOutput main(PixelShaderInput IN, uint primID : SV_PrimitiveID)
{
    PixelShaderOutput OUT;
    OUT.Color = IN.Color;
    OUT.IDs = int2(EntityIDCB.entityID, (int) primID);
    return OUT;
}

/*float4 main(PixelShaderInput IN) : SV_TARGET
{
   
    return IN.Color;
}*/