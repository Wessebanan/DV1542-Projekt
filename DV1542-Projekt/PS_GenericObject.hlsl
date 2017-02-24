SamplerState samplerState : register (s0);
Texture2D objectTex : register (t0);

cbuffer MATERIAL_BUFFER : register(b0)
{
	float Kdr, Kdg, Kdb;
	float Ksr, Ksg, Ksb;
	float Kar, Kag, Kab;
	float Ns;
}

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION0;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 lightPos : POSITION1;
};

struct PS_OUT
{
	float4 normal : SV_Target0;
	float4 diffuse : SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
	float4 lightPos : SV_Target4;
};


PS_OUT main( VS_OUT input ) 
{
	PS_OUT output = (PS_OUT)0;
	float4 diffuseMaterial = float4(Kdr, Kdg, Kdb, 1.0f);
	float4 specularMaterial = float4(Ksr, Ksg, Ksb, Ns);
	
	output.normal = float4(input.Normal, 0.0f);		
	output.diffuse = diffuseMaterial * objectTex.Sample(samplerState, input.TexCoord);
	output.specular = specularMaterial;
	output.position = float4(input.WPos, 1.0f);
	output.lightPos = input.lightPos;
	return output;
}