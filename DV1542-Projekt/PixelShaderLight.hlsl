SamplerState samplerState : register(s0);
Texture2D normals : register(t0);
Texture2D diffuses : register(t1);
Texture2D speculars : register(t2);
Texture2D positions : register(t3);

float4 main(in float4 screenPos : SV_POSITION) : SV_TARGET
{
	float4 finalColor;
	float3 lightPos = { 0.0f, 4.0f, -10.0f };
	float3 lightVec = lightPos - positions.Sample(samplerState, screenPos.xy).xyz;
	float brightness = saturate(dot(normalize(lightVec), normals.Sample(samplerState, screenPos.xy).xyz));
	finalColor = diffuses.Sample(samplerState, screenPos.xy) * brightness;
	return finalColor;
}