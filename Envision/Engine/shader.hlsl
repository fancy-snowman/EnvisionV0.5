// ######################################################################### //
// ######################### ROOT SIGNATURE LAYOUT ######################### //
// ######################################################################### //

/*
ROOT SIGNATURE
[i] TYPE		STAGE(S)	REGISTER	SPACE		COMMENT
-------------------------------------------------------------------------------
[0] CONSTANT	V|P			b0			0			InstanceOffset, 1 constant
[1] TABLE		V|P									Instance buffer array
[2] TABLE		P									Material buffer array
[3] CBV			V|P			b1			0			Camera buffer
-------------------------------------------------------------------------------



RANGES IN TABLE INDEX [1] (Instance buffer array)
[i] TYPE	NUM DESCS	BASE REG	SPACE	COMMENT
-------------------------------------------------------------------------------
[0] SRV		1			t0			0		
-------------------------------------------------------------------------------



RANGES IN TABLE INDEX [2] (Material buffer array)
[i] TYPE	NUM DESCS	BASE REG	SPACE	COMMENT
-------------------------------------------------------------------------------
[0] SRV		1			t1			0
-------------------------------------------------------------------------------
*/



// ######################################################################### //
// ###################### COMMON SHADER STAGE BINDINGS ##################### //
// ######################################################################### //

cbuffer RootConstants : register(b0)
{
	unsigned int InstanceOffset;
}

cbuffer CameraBuffer : register(b1)
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

struct InstanceData
{
	float3 Position;
	unsigned int ID;
	float3 ForwardDirection;
	unsigned int MaterialIndex;
	float3 UpDirection;
	float Pad;
	float4x4 WorldMatrix;
};

StructuredBuffer<InstanceData> InstanceBuffers : register (t0);



// ######################################################################### //
// ###################### PIXEL SHADER STAGE BINDINGS ###################### //
// ######################################################################### //

struct MaterialData
{
	float3 AmbientFactor;
	int AmbientMapIndex; // -1 if no map exist
	float3 DiffuseFactor;
	int DiffuseMapIndex; // -1 if no map exist
	float3 SpecularFactor;
	int SpecularMapIndex; // -1 if no map exist
	float Shininess;
	int MaterialID;
	float2 Padding;
};

StructuredBuffer<MaterialData> MaterialBuffers : register (t1);



// ######################################################################### //
// ##################### COMMON SHADER DATA STRUCTURES ##################### //
// ######################################################################### //

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
	uint InstanceIndex : INSTANCEINDEX;
};



// ######################################################################### //
// ###################### VERTEX SHADER STAGE PROGRAM ###################### //
// ######################################################################### //

VS_OUT VS_main(VS_IN input, uint instanceIndex : SV_InstanceID)
{
	VS_OUT output;
	output.InstanceIndex = InstanceOffset + instanceIndex;

	InstanceData instance = InstanceBuffers[output.InstanceIndex];

	output.Position = float4(input.Position, 1.0f);
	output.Position = mul(output.Position, instance.WorldMatrix);
	output.Position = mul(output.Position, Camera.ViewProjectionMatrix);
	output.Normal = normalize(mul(float4(input.Normal, 0.f), instance.WorldMatrix));
	output.Texcoord = input.Texcoord;

	return output;
}



// ######################################################################### //
// ####################### PIXEL SHADER STAGE PROGRAM ###################### //
// ######################################################################### //

float4 PS_main(VS_OUT input) : SV_TARGET
{
	InstanceData instance = InstanceBuffers[input.InstanceIndex];
	MaterialData material = MaterialBuffers[instance.MaterialIndex];

	float3 dirToSun = -1.f * normalize(float3(-1.0f, -2.f, 0.8f));

	float factor = dot(input.Normal, dirToSun);
	factor = clamp(factor, 0.3f, 1.0f);

	float3 color = material.DiffuseFactor;
	color *= factor;

	//return float4(material.MaterialID / 10.f, 0.0f, 0.0f, 1.0f);
	return float4(color, 1.0f);


	//return float4(input.Texcoord, 0.0f, 1.0f);

	//float4 color = float4(0.8f, 0.6f, 0.5f, 1.0f);
	//color = color * factor;
	//color.w = 1.0f;
	//return color;
}