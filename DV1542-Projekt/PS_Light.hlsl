SamplerState samplerState : register(s0);
//G-buffer textures as input.
Texture2D normals : register(t0);
Texture2D diffuses : register(t1);
Texture2D speculars : register(t2);
Texture2D positions : register(t3);
Texture2D lightPositions : register(t4);
//----------------------------
Texture2D shadowMap : register(t5);

cbuffer camPosBuffer : register (b0)
{
	float4 camPos;
};

cbuffer lightDirBuffer : register (b1)
{
	float4 lightDir;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main(PS_IN input) : SV_TARGET
{
	//-------Information gathering--------
	float3 normal = normals.Sample(samplerState, input.texcoord).xyz;
	float3 position = positions.Sample(samplerState, input.texcoord).xyz;
	float4 color = diffuses.Sample(samplerState, input.texcoord);
	float4 specular = speculars.Sample(samplerState, input.texcoord);
	float4 lightPos = lightPositions.Sample(samplerState, input.texcoord);
	float depth = shadowMap.Sample(samplerState, input.texcoord);
	//------------------------------------

	float3 lightVec = -lightDir.xyz;

	// ---- SPECULAR CALCULATIONS ---------
	float3 reflectedLightVec = 2 * normal * dot(-normal, lightDir.xyz);
	float3 reflection = normalize(lightDir.xyz + reflectedLightVec);
	float3 pointToCamera = normalize(camPos.xyz - position);

	float specularReflection = 0.0f;
	

	if (dot(normalize(lightDir), normal) <= 0.0f) {
		specularReflection = saturate(specular.x * pow(saturate(dot(reflection, pointToCamera)), specular.w));
	}
	//-------------------------------------

	float4 ambient = { 0.10f, 0.10f, 0.10f, 0.0f };
	float brightness =  saturate(dot(normalize(lightVec), normal));

	brightness = saturate(brightness);


	return saturate(color * saturate(brightness + ambient) + specularReflection);

	// return saturate(specularReflection); // Used for testing purposes
	// return float4(pointToCamera, 1.0f); // Used for testing purposes
}