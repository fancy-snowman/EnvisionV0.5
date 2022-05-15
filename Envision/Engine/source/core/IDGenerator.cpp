#include "envision/core/IDGenerator.h"

namespace env
{
	ID IDGenerator::GenerateUnique()
	{
		return m_IDCounter++;
	}
}
