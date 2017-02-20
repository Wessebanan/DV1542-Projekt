SamplerState samplerState : register(s0);
Texture2D normals : register(t0);
Texture2D diffuses : register(t1);
Texture2D speculars : register(t2);
Texture2D positions : register(t3);

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
	//------------------------------------

	float3 lightPos = { 500.0f, 1000.0f, 500.0f };
	float3 lightVec = lightPos - position;
	float4 ambient = { 0.10f, 0.10f, 0.10f, 0.0f };
	float brightness =  0.5f * saturate(dot(normalize(lightVec), normal));

	//float specularPart =  1.0f * dot(normals.Sample(samplerState, input.texcoord).xyz, speculars.Sample(samplerState, input.texcoord).xyz);
	float specularPart = speculars.Sample(samplerState, input.texcoord).x;
	brightness = saturate(brightness + specularPart);

	return saturate(color * (brightness + ambient));
}