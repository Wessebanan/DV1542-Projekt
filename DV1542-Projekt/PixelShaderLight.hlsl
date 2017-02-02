SamplerState samplerState : register(s0);
Texture2D normals : register(t0);
Texture2D diffuses : register(t1);
Texture2D speculars : register(t2);
Texture2D positions : register(t3);

float4 main() : SV_TARGET
{
	float4 finalColor = {0.0f, 0.0f, 0.0f, 0.0f};
	float4 lightPos = { 0.0f, 0.0f, -10.0f, 1.0f };

	return finalColor;
}