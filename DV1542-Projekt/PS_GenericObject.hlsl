SamplerState samplerState : register (s0);
Texture2D objectTex : register (t0);

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct PS_OUT
{
	float4 normal : SV_Target0;
	float4 diffuse : SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
};


PS_OUT main( VS_OUT input ) 
{
	PS_OUT output = (PS_OUT)0;
	output.normal = float4(input.Normal, 0.0f);
	output.diffuse = objectTex.Sample(samplerState, input.TexCoord);
	output.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.position = float4(input.WPos, 1.0f);
	return output;
}