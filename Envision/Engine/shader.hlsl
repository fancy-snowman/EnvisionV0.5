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
[4] TABLE		P									Samplers
[5] TABLE		P									Dynamic diffuse textures
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



RANGES IN TABLE INDEX [4] (Samplers)
[i] TYPE	NUM DESCS	BASE REG	SPACE	COMMENT
-------------------------------------------------------------------------------
[0] SAMPLER	1			s0			0
-------------------------------------------------------------------------------



RANGES IN TABLE INDEX [5] (Dynamic diffuse textures)
[i]		TYPE	NUM DESCS	BASE REG	SPACE	COMMENT
-------------------------------------------------------------------------------
[0..N]	SRV		1			t0-tN		1		Dynamic array of up to N descriptors
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

Texture2D DiffuseTextures[50] : register (t0, space1);
SamplerState Sampler : register (s0);



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
	float3 WorldPosition : WORDPOS;
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
	output.WorldPosition = output.Position.xyz;
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
	// Retrieve indexed data structures
	InstanceData instance = InstanceBuffers[input.InstanceIndex];
	MaterialData material = MaterialBuffers[instance.MaterialIndex];

	// Illumination vectors
	float3 L_dirToLight = normalize(float3(1.0f, 2.f, 0.8f));
	float3 R_dirLightReflection = -1.0f * normalize(reflect(L_dirToLight, input.Normal));
	float3 V_dirToEye = normalize(Camera.Position - input.WorldPosition);

	float3 ambient = DiffuseTextures[material.AmbientMapIndex].Sample(Sampler, float2(input.Texcoord)).xyz;
	float3 diffuse = DiffuseTextures[material.DiffuseMapIndex].Sample(Sampler, float2(input.Texcoord)).xyz;
	float3 specular = DiffuseTextures[material.SpecularMapIndex].Sample(Sampler, float2(input.Texcoord)).xyz;

	ambient = ambient * float4(material.AmbientFactor, 1.0f);
	diffuse = diffuse * float4(material.DiffuseFactor, 1.0f);
	specular = specular * float4(material.SpecularFactor, 1.0f);

	float dot_d = dot(L_dirToLight, input.Normal); // dot_diffuse
	float dot_s = dot(R_dirLightReflection, V_dirToEye); // dot_specular

	bool dot_d_positive = dot_d > 0.0005f;
	bool dot_s_positive = dot_s > 0.0005f;

	{ // Calculate the ambient component
		ambient = ambient * float3(0.2f, 0.2f, 0.2f);
	}

	{// Calculate the diffuse component
		// If visible to the light source
		if (dot_d_positive) {
			diffuse = (diffuse * dot_d) * 1.0f;
		}

		// Else in shapow
		else {
			diffuse = float3(0.0f, 0.0f, 0.0f);
		}
	}

	{// Calculate the specular component
		// If reflection is visible to the eye
		if (dot_d_positive && dot_s_positive) {
			specular = (specular * pow(dot_s, max(material.Shininess, 1.0f))) * 1.0f;
		}

		// Else reflection is not visible to the eye
		else {
			specular = float3(0.0f, 0.0f, 0.0f);
		}
	}

	float3 illumination = ambient + diffuse * 2.0f + specular * 2.0f;
	return float4(illumination, 1.0f);
}