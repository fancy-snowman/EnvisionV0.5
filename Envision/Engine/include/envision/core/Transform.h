#pragma once
#include "envision/envpch.h"

namespace env
{
	class Transform
	{
	private:

		Float4x4 m_matrix;
		bool m_dirty;

		Float3 m_position;
		Quaternion m_rotation;
		Float3 m_scale;

	public:

		Transform();
		~Transform();

		// Position

		void SetPosition(const Float3& position);
		void Translate(const Float3& offset);
		void TranslateRight(float distance);
		void TranslateUp(float distance);
		void TranslateForward(float distance);
		void TranslateDirection(const Float3& direction, float distance);

		const Float3& GetPosition();
		Float4x4 GetTranslationMatrix();

		// Rotation

		void SetRotation(const Quaternion& rotation);
		void Rotate(const Quaternion& offset);
		void RotateRoll(float radians);
		void RotatePitch(float radians);
		void RotateYaw(float radians);
		void RotateRollPitchYaw(float roll, float pitch, float yaw);
		void RotateAxisX(float radians);
		void RotateAxisY(float radians);
		void RotateAxisZ(float radians);
		void RotateAxisXYZ(float x, float y, float z);

		const Quaternion& GetRotation();
		const Float3 GetRight();
		const Float3 GetUp();
		const Float3 GetForward();
		Float4x4 GetRotationMatrix();

		// Scale

		void SetScale(const Float3& scale);
		void Scale(const Float3& factors);
		void Scale(float factor);

		const Float3& GetScale();
		Float4x4 GetScaleMatrix();

		// Transform
	
		const Float4x4& GetMatrix();
		Float4x4 GetMatrixTransposed();
	};
}