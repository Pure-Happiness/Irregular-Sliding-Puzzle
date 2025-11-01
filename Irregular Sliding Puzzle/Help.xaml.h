#pragma once

#include "Help.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct Help : HelpT<Help>
	{
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct Help : HelpT<Help, implementation::Help>
	{
	};
}
