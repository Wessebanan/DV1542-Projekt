SamplerState samplerState : register (s0);
Texture2D grassTex : register (t0);
Texture2D dirtTex : register (t1);
Texture2D rockTex : register (t2);
Texture2D normalMap : register (t3);

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float3 WPos : POSITION0;
	float2 TexCoord : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 Bitangent : BINORMAL;
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

PS_OUT main(PS_IN input)
{
	PS_OUT output = (PS_OUT)0;


	float higherTexIntensity = 0.0f;
	float lowerTexIntensity = 0.0f;

	float specPow = 1.0f;
	float specIntensity = 0.0f;

	float dirtSpecPower = 1.0f;
	float dirtSpecIntensity = 0.0f;

	float grassSpecPower = 1.0f;
	float grassSpecIntensity = 0.7f;

	float rockSpecPower = 7.0f;
	float rockSpecIntensity = 1.0f;

	if (input.WPos.y < 10.0f)
	{
		output.diffuse = dirtTex.Sample(samplerState, input.TexCoord);
		specPow = dirtSpecPower;
		specIntensity = dirtSpecIntensity;
	}
	else if (input.WPos.y < 20.0f)
	{
		higherTexIntensity = (input.WPos.y - 10.0f) / 10;
		lowerTexIntensity = 1 - higherTexIntensity;
		output.diffuse = dirtTex.Sample(samplerState, input.TexCoord) * lowerTexIntensity
			+ grassTex.Sample(samplerState, input.TexCoord) * higherTexIntensity;
		specPow = dirtSpecPower * lowerTexIntensity + grassSpecPower * higherTexIntensity;
		specIntensity = dirtSpecIntensity * lowerTexIntensity + grassSpecIntensity * higherTexIntensity;
	}
	else if (input.WPos.y < 45.0f)
	{
		output.diffuse = grassTex.Sample(samplerState, input.TexCoord);
		specPow = grassSpecPower;
		specIntensity = grassSpecIntensity;
	}
	else if (input.WPos.y < 55.0f)
	{
		higherTexIntensity = (input.WPos.y - 45.0f) / 10;
		lowerTexIntensity = 1 - higherTexIntensity;
		output.diffuse = grassTex.Sample(samplerState, input.TexCoord) * lowerTexIntensity
			+ rockTex.Sample(samplerState, input.TexCoord) * higherTexIntensity;
		specPow = grassSpecPower * lowerTexIntensity + rockSpecPower * higherTexIntensity;
		specIntensity = grassSpecIntensity * lowerTexIntensity + rockSpecIntensity * higherTexIntensity;
	}
	else if(input.WPos.y < 80.0f)
	{
		output.diffuse = rockTex.Sample(samplerState, input.TexCoord);
		specPow = rockSpecPower;
		specIntensity = rockSpecIntensity;
	}

	output.specular.x = specIntensity;
	output.specular.y = specPow;

	output.position = float4(input.WPos, 1);
	output.lightPos = input.lightPos;

	float3x3 TBN = float3x3
		(
			input.Tangent,
			input.Bitangent,
			input.Nor
		);

	//Sampling and decompressing the normal in tangent space
	float3 normalTS = normalMap.Sample(samplerState, input.TexCoord / 102.4f).xyz;
	normalTS = normalize((normalTS * 2.0f) - 1.0f);
	//Transforming the normal to world space using tangent frame and setting to output
	output.normal = normalize(float4(mul(normalTS, TBN), 0.0f));

	return output;
}