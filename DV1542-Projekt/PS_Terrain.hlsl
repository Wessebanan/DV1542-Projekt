SamplerState samplerState : register (s0);
Texture2D grassTex : register (t0);
Texture2D dirtTex : register (t1);
Texture2D rockTex : register (t2);
Texture2D normalMap : register (t3);

cbuffer camPosBuffer : register (b0)
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

	float3 lightPos = { 500.0f, 1000.0f, 500.0f };

	float higherTexIntensity = 0.0f;
	float lowerTexIntensity = 0.0f;

	if (input.WPos.y < 10.0f)
	{
		output.diffuse = dirtTex.Sample(samplerState, input.TexCoord);
		output.specular.w = 50;
	}
	else if (input.WPos.y < 20.0f)
	{
		higherTexIntensity = (input.WPos.y - 10.0f) / 10;
		lowerTexIntensity = 1 - higherTexIntensity;
		output.diffuse = dirtTex.Sample(samplerState, input.TexCoord) * lowerTexIntensity
			+ grassTex.Sample(samplerState, input.TexCoord) * higherTexIntensity;
		output.specular.w = 0.1f * lowerTexIntensity + 0.1f * higherTexIntensity;
	}
	else if (input.WPos.y < 45.0f)
	{
		output.diffuse = grassTex.Sample(samplerState, input.TexCoord);
		//output.diffuse = float4(1.0f, 0, 0, 0);
		output.specular.w = 32.1f;
	}
	else if (input.WPos.y < 55.0f)
	{
		higherTexIntensity = (input.WPos.y - 45.0f) / 10;
		lowerTexIntensity = 1 - higherTexIntensity;
		output.diffuse = grassTex.Sample(samplerState, input.TexCoord) * lowerTexIntensity
			+ rockTex.Sample(samplerState, input.TexCoord) * higherTexIntensity;
		output.specular.w = 2 * lowerTexIntensity + 1 * higherTexIntensity;
	}
	else if(input.WPos.y < 80.0f)
	{
		output.diffuse = rockTex.Sample(samplerState, input.TexCoord);
		output.specular.w = 1;
	}
	

	float3 lightVec = input.WPos - lightPos; // Vector from light to point
	float3 reflectedLightVec = 2 * input.Nor * dot(-input.Nor, lightVec);
	float3 reflection = normalize(lightVec + reflectedLightVec);
	float3 pointToCamera = normalize(camPos - input.WPos);
	float specularReflection = saturate(dot(reflection, pointToCamera));

	output.specular.x = specularReflection;
	output.position = float4(input.WPos, 1);

	return output;
}