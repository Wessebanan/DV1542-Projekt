struct VS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

Texture2D heightMap : register(t0);
//Texture2D normalMap : register(t1);
SamplerState sampAni : register(s0);

struct VS_OUT
{
	float4 Pos : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	//float3 Normal : NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float3 s = heightMap.SampleLevel(sampAni, input.Pos.xz / 1024.0f, 0).xyz;

	//float3 normal = normalMap.SampleLevel(sampAni, input.Pos.xz / 1024.0f, 0).xyz;
	
	output.Pos = float4(input.Pos.x, input.Pos.y + 100.0f * s.r, input.Pos.z, 1.0f);
	output.Color = input.Color;
	output.TexCoord = input.TexCoord;
	//output.Normal = normal;

	return output;
}