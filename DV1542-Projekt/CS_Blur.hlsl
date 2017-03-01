Texture2D<float> origSM : register (t0);
RWTexture2D<float> blurSM : register (u0);

static const float gaussianNumbers[7][7] =
{	//Calculated in a different program.
	0.000840725, 0.00301024, 0.00647097, 0.00835139, 0.00647097, 0.00301024, 0.000840725,
	0.00301024, 0.0107783, 0.0231695, 0.0299024, 0.0231695, 0.0107783, 0.00301024,
	0.00647097, 0.0231695, 0.0498063, 0.0642797, 0.0498063, 0.0231695, 0.00647097,
	0.00835139, 0.0299024, 0.0642797, 0.082959, 0.0642797, 0.0299024, 0.00835139,
	0.00647097, 0.0231695, 0.0498063, 0.0642797, 0.0498063, 0.0231695, 0.00647097,
	0.00301024, 0.0107783, 0.0231695, 0.0299024, 0.0231695, 0.0107783, 0.00301024,
	0.000840725, 0.00301024, 0.00647097, 0.00835139, 0.00647097, 0.00301024, 0.000840725
};

[numthreads(16, 16, 1)]
void main(uint3 dThreadID : SV_DispatchThreadID)
{
	float blurredDepth = 0.0f;
	//Starting at (-3,-3) and finishing at (3,3)
	//doing one column at a time.
	for (int i = -3; i < 4; i++)
	{
		for (int j = -3; j < 4; j++)
		{	//Summing the depths of a 7x7 pixel square from the original depth map with a gaussian filter (sum = 1).
			blurredDepth += origSM.Load(dThreadID + int3(i, j, 0)) * gaussianNumbers[i + 3][j + 3];
		}
	}
	blurSM[dThreadID.xy] = blurredDepth;
}