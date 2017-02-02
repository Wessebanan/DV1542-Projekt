struct PS_IN {
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float3 WPos : POSITION;
	float3 Color : COLOR;
};



float4 main(PS_IN input) : SV_TARGET
{
	float3 lightPos = {50.0f, 200.0f, 50.0f};
	float3 lightVec = lightPos - input.WPos;
	float brightness = saturate(dot(normalize(lightVec), input.Nor));
	return float4(input.Color * brightness, 1.0f);
}