SamplerState samplerState : register(s0);
Texture2D normals : register(t0);
Texture2D diffuses : register(t1);
Texture2D speculars : register(t2);
Texture2D positions : register(t3);

cbuffer camPosBuffer : register (b0)
{
	float4 camPos;
};

cbuffer lightDirBuffer : register (b1)
{
	float4 lightDir;
}

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
	float2 specular = speculars.Sample(samplerState, input.texcoord).xy;
	//------------------------------------

	float3 lightDir = { 1.0f, -1.0f, 0.0f };

	//float3 lightPos = { 500.0f, 1000.0f, 500.0f };
	//float3 lightVec = lightPos - position;
	float3 lightVec = -lightDir;

	// ---- SPECULAR CALCULATIONS ---------
	float3 reflectedLightVec = 2 * normal * dot(-normal, lightDir);
	float3 reflection = normalize(lightDir + reflectedLightVec);
	float3 pointToCamera = normalize(camPos.xyz - position);

	float specularReflection = saturate(specular.x * pow(saturate(dot(reflection, pointToCamera)), specular.y));





	//-------------------------------------






	float4 ambient = { 0.10f, 0.10f, 0.10f, 0.0f };
	float brightness =  saturate(dot(normalize(lightVec), normal));

	//float specularPart =  1.0f * dot(normals.Sample(samplerState, input.texcoord).xyz, speculars.Sample(samplerState, input.texcoord).xyz);
	float specularPart = speculars.Sample(samplerState, input.texcoord).x;
	brightness = saturate(brightness + specularReflection);
	float4 test = { 1.0f, 1.0f, 1.0f, 1.0f };

	return saturate(color * saturate(brightness + ambient));
}