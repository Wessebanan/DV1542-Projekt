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
	float3 lightPos = { 500.0f, 400.0f, 500.0f };
	float3 lightVec = lightPos - positions.Sample(samplerState, input.texcoord).xyz;
	float brightness = saturate(dot(normalize(lightVec), normals.Sample(samplerState, input.texcoord).xyz));
	return diffuses.Sample(samplerState, input.texcoord) * brightness;
}