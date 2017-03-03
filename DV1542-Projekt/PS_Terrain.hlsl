SamplerState samplerState	: register (s0);
Texture2D grassTex			: register (t0);
Texture2D dirtTex			: register (t1);
Texture2D rockTex			: register (t2);
Texture2D normalMap			: register (t3);

struct PS_IN
{
	float4 Pos		 : SV_POSITION;
	float3 Nor		 : NORMAL;
	float3 WPos		 : POSITION0;
	float2 TexCoord  : TEXCOORD;
	float3 Tangent	 : TANGENT;
	float3 Bitangent : BINORMAL;
	float4 lightPos  : POSITION1;
	float  testFloat : FLOAT;
};

struct PS_OUT
{	//G-buffer textures as output.
	float4 normal	: SV_Target0;
	float4 diffuse	: SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
	float4 lightPos : SV_Target4;
};	//----------------------------

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
	float grassSpecIntensity = 0.3f;

	float rockSpecPower = 4.0f;
	float rockSpecIntensity = 0.5f;

	//Determining which texture should be sampled based on the height (y).
	if (input.WPos.y < 10.0f)
	{
		output.diffuse = dirtTex.Sample(samplerState, input.TexCoord);
		specPow = dirtSpecPower;
		specIntensity = dirtSpecIntensity;
	}
	//Blending of the textures if the position is between two values.
	else if (input.WPos.y < 20.0f)
	{
		//higherTexIntensity: How much color from the standard texture above the upper limit of the blending to be used.
		//lowerTexIntensity: Takes the rest of the color from the lower texture (adds up to 1).
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

	output.specular.xyz = specIntensity;
	output.specular.w = specPow;

//	output.specular.w = input.testFloat;

	output.position = float4(input.WPos, 1);
	output.lightPos = input.lightPos;

	//The TBN matrix is constructed for coverting the sampled normal (in tangent space) to world space.
	float3x3 TBN = float3x3
		(
			input.Tangent,
			input.Bitangent,
			input.Nor
		);

	//Sampling and decompressing the normal in tangent space. Texcoords divided by 102.4 to
	//convert from [0;102.4] -> [0;1] since the normal map covers the entire terrain.
	float3 normalTS = normalMap.Sample(samplerState, input.TexCoord / 102.4f).xyz;
	normalTS = normalize((normalTS * 2.0f) - 1.0f);

	//Transforming the normal to world space with the TBN matrix and setting to output
	output.normal = normalize(float4(mul(normalTS, TBN), 0.0f));

	return output;
}