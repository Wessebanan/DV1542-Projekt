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
	float3 lightPos = { 500.0f, 1000.0f, 500.0f };
	float3 lightVec = lightPos - positions.Sample(samplerState, input.texcoord).xyz;
	float4 ambient = { 0.10f, 0.10f, 0.10f, 0.0f };
	float brightness =  0.5f * saturate(dot(normalize(lightVec), normals.Sample(samplerState, input.texcoord).xyz));

	float specularPart =  1.0f * dot(normals.Sample(samplerState, input.texcoord).xyz, speculars.Sample(samplerState, input.texcoord).xyz);
	brightness = saturate(brightness + specularPart);

	return saturate(diffuses.Sample(samplerState, input.texcoord) * (brightness + ambient));
}