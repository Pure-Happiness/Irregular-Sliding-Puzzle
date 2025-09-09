#pragma once

#include "App.xaml.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct App : AppT<App>
	{
		App();

		static void OnLaunched(LaunchActivatedEventArgs const&);
	};
}
