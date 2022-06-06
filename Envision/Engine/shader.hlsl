struct VS_IN
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

cbuffer CameraBuffer : register(b0)
{
	struct {
		float3 Position;
		float FieldOfView;
		float3 ForwardDirection;
		float DistanceNearPlane;
		float3 UpDirection;
		float DistanceFarPlane;
		float4x4 ViewMatrix;
		float4x4 ProjectionMatrix;
		float4x4 ViewProjectionMatrix;
	} Camera;
}

cbuffer ObjectBuffer : register(b1)
{
	struct {
		float3 Position;
		float ID;
		float3 ForwardDirection;
		float MaterialID;
		float3 UpDirection;
		float Pad;

		float4x4 WorldMatrix;
	} Object;
}

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;
	output.Position = float4(input.Position, 1.0f);
	output.Position = mul(output.Position, Object.WorldMatrix);
	output.Position = mul(output.Position, Camera.ViewProjectionMatrix);

	output.Normal = normalize(mul(float4(input.Normal, 0.f), Object.WorldMatrix));

	output.Texcoord = input.Texcoord;

	return output;
}

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float3 dirToSun = -1.f * normalize(float3(-1.0f, -2.f, 0.8f));

	float factor = dot(input.Normal, dirToSun);
	factor = clamp(factor, 0.3f, 1.0f);

	return float4(input.Normal, 1.0f);
	//return float4(input.Texcoord, 0.0f, 1.0f);

	//float4 color = float4(0.8f, 0.6f, 0.5f, 1.0f);
	//color = color * factor;
	//color.w = 1.0f;
	//return color;
}