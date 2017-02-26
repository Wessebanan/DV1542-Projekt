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
	//------------------------------------
	float3 lightVec = -lightDir.xyz;

	//--------Shadow stuff----------
	float epsilon = 0.001f;
	lightPos.xy /= lightPos.w;
	float2 smTex = float2(0.5f * lightPos.x + 0.5f, -0.5f * lightPos.y + 0.5f);
	float depth = lightPos.z / lightPos.w;

	float dx = 1.0f / 1280.0f;
	float dy = 1.0f / 720.0f;

	//Multisampling for antialiasing, 0 if in shadow and 1 if not.
	float s0 = (shadowMap.Sample(samplerState, smTex).x + epsilon < depth) ? 0.0f : 1.0f;
	float s1 = (shadowMap.Sample(samplerState, smTex + float2(dx, 0.0f)).x + epsilon < depth) ? 0.0f : 1.0f;
	float s2 = (shadowMap.Sample(samplerState, smTex + float2(0.0f, dy)).x + epsilon < depth) ? 0.0f : 1.0f;
	float s3 = (shadowMap.Sample(samplerState, smTex + float2(dx, dy)).x + epsilon < depth) ? 0.0f : 1.0f;

	//Averaging the results.
	float shadowCoeff = (s0 + s1 + s2 + s3) / 4;
	//----------------------------------

	// ---- SPECULAR CALCULATIONS ---------
	float3 reflectedLightVec = 2 * normal * dot(-normal, lightDir.xyz);
	float3 reflection = normalize(lightDir.xyz + reflectedLightVec);
	float3 pointToCamera = normalize(camPos.xyz - position);

	float specularReflection = 0.0f;	

	if (dot(normalize(lightDir.xyz), normal) <= 0.0f) {
		specularReflection = saturate(specular.x * pow(saturate(dot(reflection, pointToCamera)), specular.w));
	}
	//-------------------------------------

	float4 ambient = { 0.10f, 0.10f, 0.10f, 0.0f };
	float brightness =  saturate(dot(normalize(lightVec), normal));

	return saturate(color * saturate(brightness * shadowCoeff + ambient) + specularReflection);

	// return saturate(specularReflection); // Used for testing purposes
	// return float4(pointToCamera, 1.0f); // Used for testing purposes
}