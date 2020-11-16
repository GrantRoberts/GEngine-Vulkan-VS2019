#pragma once
#include <optional>
#define uint uint32_t

struct QueueFamilyIndices
{
	bool IsComplete()
	{
		return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
	}

	std::optional<uint> m_GraphicsFamily;
	// (feelsbadman)
	std::optional<uint> m_PresentFamily;
};