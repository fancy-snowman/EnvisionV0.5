#pragma once
#include "envision\envpch.h"

namespace env
{
	struct Timepoint;
	
	struct Duration
	{
		Duration(float seconds = 0) : m_duration(std::chrono::microseconds((int)(seconds * 1000000))) {}
		Duration(const Duration& other) : m_duration(other.m_duration) {}
		Duration(const std::chrono::high_resolution_clock::duration& duration) : m_duration(duration) {}

		Duration& operator=(const Duration& other) { m_duration = other.m_duration; return *this; }
		Duration& operator+=(const Duration& other) { m_duration += other.m_duration; return *this; }
		Duration& operator-=(const Duration& other) { m_duration -= other.m_duration; return *this; }

		bool operator<(const Duration& other) { return m_duration < other.m_duration; }
		bool operator>(const Duration& other) { return m_duration > other.m_duration; }
		bool operator<=(const Duration& other) { return m_duration <= other.m_duration; }
		bool operator>=(const Duration& other) { return m_duration >= other.m_duration; }
		bool operator==(const Duration& other) { return m_duration == other.m_duration; }

		float InSeconds() const { return m_duration.count() / 1000000.f; }

	private:
		std::chrono::high_resolution_clock::duration m_duration;
		friend struct Timepoint;
	};

	struct Timepoint
	{
		Timepoint() : m_timepoint(std::chrono::seconds(0)) {}
		Timepoint(const Timepoint& other) : m_timepoint(other.m_timepoint) {}
		Timepoint(const std::chrono::high_resolution_clock::time_point& timepoint) : m_timepoint(timepoint) {}

		Timepoint& operator=(const Timepoint& other) { m_timepoint = other.m_timepoint; return *this; }
		Timepoint& operator+=(const Timepoint& other) = delete;
		Timepoint& operator-=(const Timepoint& other) = delete;

		Timepoint& operator+=(const Duration& duration) { m_timepoint += duration.m_duration; return *this; }
		Timepoint& operator-=(const Duration& duration) { m_timepoint -= duration.m_duration; return *this; }


		Duration operator-(const Timepoint& other) { return Duration(m_timepoint - other.m_timepoint); }
		Duration operator+(const Timepoint& other) = delete;

	private:
		std::chrono::high_resolution_clock::time_point m_timepoint;
	};

	namespace Time
	{
		static Timepoint Now()
		{
			return Timepoint(std::chrono::high_resolution_clock::now());
		}
	}
}