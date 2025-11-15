#pragma once

#include "ReplayGraph.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct ReplayGraph : ReplayGraphT<ReplayGraph>
	{
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct ReplayGraph : ReplayGraphT<ReplayGraph, implementation::ReplayGraph>
	{
	};
}
