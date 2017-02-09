SamplerState samplerState : register (s0);
Texture2D grassTex : register (t0);
Texture2D dirtTex : register (t1);
Texture2D rockTex : register (t2);

cbuffer camPosBuffer : register (c0)
{
	float4 camPos;
};

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float3 WPos : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

struct PS_OUT
{
	float4 normal : SV_Target0;
	float4 diffuse : SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
};

PS_OUT main(PS_IN input)
{
	PS_OUT output = (PS_OUT)0;
	output.normal = float4(input.Nor, 0);

	if (input.WPos.y < 13.0f)
	{
		output.diffuse = dirtTex.Sample(samplerState, input.TexCoord);
	}
	else if (input.WPos.y < 46.0f)
	{
		output.diffuse = grassTex.Sample(samplerState, input.TexCoord);
	}
	else
	{
		output.diffuse = rockTex.Sample(samplerState, input.TexCoord);
	}
	output.specular = float4(1.0f, 1.0f, 1.0f, 1.0f); //obj specularity?
	output.position = float4(input.WPos, 1);
	return output;
}