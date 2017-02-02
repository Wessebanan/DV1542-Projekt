struct PS_IN
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float3 WPos : POSITION;
	float3 Color : COLOR;
};

struct PS_OUT
{
	float4 normal : SV_Target0;
	float4 diffuse : SV_Target1;
	float4 specular : SV_Target2;
	float4 position : SV_Target3;
};

PS_OUT main(PS_IN input)
{
	PS_OUT output = (PS_OUT)0;
	output.normal = float4(input.Nor, 0);
	output.diffuse = float4(input.Color, 1);
	output.specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	output.position = float4(input.WPos, 1);
	return output;
}