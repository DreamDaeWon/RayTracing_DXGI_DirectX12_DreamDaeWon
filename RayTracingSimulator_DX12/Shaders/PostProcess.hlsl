Texture2D<float4> Accumulation : register(t0);
Texture2D<float4> RawColor : register(t1);
Texture2D<float4> ReconstructedColor : register(t2);
SamplerState LinearClamp : register(s0);
cbuffer PostConstants : register(b0) { float Exposure, Split, Mode, RRActive; };
struct VertexOutput { float4 position : SV_Position; float2 uv : TEXCOORD0; };
VertexOutput VSMain(uint id : SV_VertexID) {
    VertexOutput output;
    output.uv = float2((id << 1) & 2, id & 2);
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}
float3 ACES(float3 x) { return saturate((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14)); }
float4 PSMain(VertexOutput input) : SV_Target {
    float3 color = Accumulation.SampleLevel(LinearClamp, input.uv, 0).rgb;
    if (Mode >= 3) {
        bool rrSide = Mode == 4 || ((Mode == 3 || Mode == 6) && input.uv.x >= Split);
        if (rrSide) color = RRActive > 0 ? ReconstructedColor.SampleLevel(LinearClamp, input.uv, 0).rgb : RawColor.SampleLevel(LinearClamp, input.uv, 0).rgb;
        else if (Mode != 6) color = RawColor.SampleLevel(LinearClamp, input.uv, 0).rgb;
    }
    float3 hdr = max(0, color) * exp2(Exposure);
    return float4(pow(ACES(hdr), 1.0 / 2.2), 1);
}
