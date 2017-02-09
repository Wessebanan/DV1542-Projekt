struct VS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

Texture2D heightMap : register(t0);
SamplerState sampAni : register(s0);

struct VS_OUT
{
	float4 Pos : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float3 s = heightMap.SampleLevel(sampAni, input.Pos.xz / 256.0f,0);
	
	output.Pos = float4(input.Pos.x, input.Pos.y + 100.0f * s.r, input.Pos.z, 1.0f);
	output.Color = input.Color;
	output.TexCoord = input.TexCoord;

	return output;
}