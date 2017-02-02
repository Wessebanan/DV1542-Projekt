struct PS_IN {
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float3 WPos : POSITION;
	float3 Color : COLOR;
};



float4 main(PS_IN input) : SV_TARGET
{
	float3 lightPos = {0.0f, 4.0f, -10.0f};
	float3 lightVec = lightPos - input.WPos;
	float brightness = saturate(dot(normalize(lightVec), input.Nor));
	return float4(input.Color * brightness, 1.0f);
}