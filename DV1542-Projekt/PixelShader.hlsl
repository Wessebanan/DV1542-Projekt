struct PS_IN {
	float4 Pos : SV_POSITION;
	float3 Color : COLOR;
};

Texture2D heightMap : register(t0);
SamplerState sampAni : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float3 s = heightMap.Sample(sampAni,input.Pos.xz / 27);
	return float4(s, 1.0f);
}