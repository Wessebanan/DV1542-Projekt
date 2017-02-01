struct VS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

Texture2D heightMap : register(t0);
SamplerState sampAni : register(s0);

struct VS_OUT
{
	float4 Pos : POSITION;
	float3 Color : COLOR;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float3 s = heightMap.SampleLevel(sampAni, input.Pos.xz / 10.0f,0);

	
	output.Pos = float4(input.Pos.x, -5.0f + 2.0f * s.r, input.Pos.z, 1.0f);
	output.Color = input.Color;

	return output;
}