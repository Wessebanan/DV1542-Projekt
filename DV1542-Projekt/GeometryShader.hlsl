struct GS_IN
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Color : COLOR;
};

[maxvertexcount(3)]
void main( triangle GS_IN input[3],  inout TriangleStream< GS_OUT > output)
{
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.Pos = float4(input[i].Pos, 1.0f);
		element.Color = input[i].Color;
		output.Append(element);
	}
};