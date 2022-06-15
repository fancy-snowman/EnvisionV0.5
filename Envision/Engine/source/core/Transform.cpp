#include "envision/envpch.h"
#include "envision/core/Transform.h"

env::Transform::Transform() :
	m_matrix(Float4x4::Identity),
	m_dirty(true),
	m_position(Float3::Zero),
	m_rotation(Quaternion::Identity),
	m_scale(Float3::One)
{
	//
}

env::Transform::Transform(const Float4x4& matrix) :
	m_matrix(Float4x4::Identity),
	m_dirty(true),
	m_position(Float3::Zero),
	m_rotation(Quaternion::Identity),
	m_scale(Float3::One)
{
	using namespace DirectX;
	XMVECTOR scale;
	XMVECTOR rotation;
	XMVECTOR translation;
	DirectX::XMMatrixDecompose(&scale, &rotation, &translation, matrix);
	m_scale = scale;
	m_rotation = rotation;
	m_position = translation;
}

env::Transform::~Transform()
{
	//
}

void env::Transform::SetPosition(const Float3& position)
{
	m_position = position;
	m_dirty = true;
}

void env::Transform::Translate(const Float3& offset)
{
	m_position += offset;
	m_dirty = true;
}

void env::Transform::TranslateRight(float distance)
{
	Float3 right = GetRight();
	m_position += right * distance;
	m_dirty = true;
}

void env::Transform::TranslateUp(float distance)
{
	Float3 up = GetUp();
	m_position += up * distance;
	m_dirty = true;
}

void env::Transform::TranslateForward(float distance)
{
	Float3 forward = GetForward();
	m_position += forward * distance;
	m_dirty = true;
}

void env::Transform::TranslateDirection(const Float3& direction, float distance)
{
	Float3 directionNormalized = direction;
	directionNormalized.Normalize();
	m_position += directionNormalized * distance;
	m_dirty = true;
}

const Float3& env::Transform::GetPosition()
{
	return m_position;
}

Float4x4 env::Transform::GetTranslationMatrix()
{
	return Float4x4::CreateTranslation(m_position);
}

void env::Transform::SetRotation(const Quaternion& rotation)
{
	m_rotation = rotation;
	m_dirty = true;
}

void env::Transform::Rotate(const Quaternion& offset)
{
	m_rotation *= offset;
	m_dirty = true;
}

void env::Transform::RotateRoll(float radians)
{
	Quaternion roll = DirectX::XMQuaternionRotationAxis(GetForward(), radians);
	m_rotation *= roll;
	m_dirty = true;
}

void env::Transform::RotatePitch(float radians)
{
	Quaternion pitch = DirectX::XMQuaternionRotationAxis(GetRight(), radians);
	m_rotation *= pitch;
	m_dirty = true;
}

void env::Transform::RotateYaw(float radians)
{
	Quaternion yaw = DirectX::XMQuaternionRotationAxis(GetUp(), radians);
	m_rotation *= yaw;
	m_dirty = true;
}

void env::Transform::RotateRollPitchYaw(float roll, float pitch, float yaw)
{
	Quaternion qPitch = DirectX::XMQuaternionRotationAxis(GetRight(), pitch);
	Quaternion qYaw = DirectX::XMQuaternionRotationAxis(GetUp(), yaw);
	Quaternion qRoll = DirectX::XMQuaternionRotationAxis(GetForward(), roll);
	m_rotation *= qRoll * qYaw * qPitch;

	m_dirty = true;
}

void env::Transform::RotateAxisX(float radians)
{
	m_rotation *= DirectX::XMQuaternionRotationAxis({ 1.0f, 0.0f, 0.0f }, radians);
}

void env::Transform::RotateAxisY(float radians)
{
	m_rotation *= DirectX::XMQuaternionRotationAxis({ 0.0f, 1.0f, 0.0f }, radians);
}

void env::Transform::RotateAxisZ(float radians)
{
	m_rotation *= DirectX::XMQuaternionRotationAxis({ 0.0f, 0.0f, 1.0f }, radians);
}

void env::Transform::RotateAxisXYZ(float x, float y, float z)
{
	m_rotation *= DirectX::XMQuaternionRotationRollPitchYaw(x, y, z);
}

const Quaternion& env::Transform::GetRotation()
{
	return m_rotation;
}

const Float3 env::Transform::GetRight() const
{
	Float3 right = DirectX::XMVector3Rotate(Float3::UnitX, m_rotation);
	return right;
}

const Float3 env::Transform::GetUp() const
{
	Float3 up = DirectX::XMVector3Rotate(Float3::UnitY, m_rotation);
	return up;
}

const Float3 env::Transform::GetForward() const
{
	Float3 forward = DirectX::XMVector3Rotate(Float3::UnitZ, m_rotation);
	return forward;
}

Float4x4 env::Transform::GetRotationMatrix()
{
	Float4x4 matrix = DirectX::XMMatrixRotationQuaternion(m_rotation);
	return matrix;
}

void env::Transform::SetScale(const Float3& scale)
{
	m_scale = scale;
	m_dirty = true;
}

void env::Transform::Scale(const Float3& factors)
{
	m_scale = DirectX::XMVectorMultiply(m_scale, factors);
}

void env::Transform::Scale(float factor)
{
	m_scale *= factor;
}

const Float3& env::Transform::GetScale()
{
	return m_scale;
}

Float4x4 env::Transform::GetScaleMatrix()
{
	return Float4x4::CreateScale(m_scale);
}

const Float4x4& env::Transform::GetMatrix()
{
	if (m_dirty) {
		m_matrix = DirectX::XMMatrixTransformation(
			Float3::Zero,
			Float3::Zero,
			m_scale,
			Float3::Zero,
			m_rotation,
			m_position);
		m_dirty = false;
	}
	return m_matrix;
}

Float4x4 env::Transform::GetMatrix() const
{
	Float4x4 matrix = DirectX::XMMatrixTransformation(
		Float3::Zero,
		Float3::Zero,
		m_scale,
		Float3::Zero,
		m_rotation,
		m_position);
	return matrix;
}

Float4x4 env::Transform::GetMatrixTransposed()
{
	Float4x4 transposed = GetMatrix();
	return transposed.Transpose();
}
