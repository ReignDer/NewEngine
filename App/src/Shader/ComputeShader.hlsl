RWTexture2D<float4> buffer : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    int width;
    int height;
    buffer.GetDimensions(width, height);
    
    float2 uv = DTid.xy / float2(width, height);
    buffer[DTid.xy] = float4(uv.xy, 0.0f, 1.0f);
}