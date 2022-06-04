struct VS_IN
{
	float3 Position : POSITION;
	float3 Color : COLOR;
	float3 Normal : NORMAL;
};

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
	float3 Normal : NORMAL;
};

cbuffer CameraBuffer : register(b0)
{
	float4x4 ViewProjection;
}

cbuffer ObjectBuffer : register(b1)
{
	float4x4 World;
}

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;
	output.Position = float4(input.Position, 1.0f);
	output.Position = mul(output.Position, World);
	output.Position = mul(output.Position, ViewProjection);
	output.Color = input.Color;

	output.Normal = mul(input.Normal, World);

	return output;
}

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float3 dirToSun = -1.f * normalize(float3(-1.0f, -2.f, 0.8f));

	float factor = dot(input.Normal, dirToSun);
	factor = clamp(factor, 0.3f, 1.0f);

	return float4(input.Color * factor, 1.0f);
}