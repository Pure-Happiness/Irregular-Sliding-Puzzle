#pragma once

#include "Settings.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct Settings : SettingsT<Settings>
	{
		void InitLanguage(IInspectable const&, RoutedEventArgs const&);
		void SelectLanguage(IInspectable const&, SelectionChangedEventArgs const&);
		void InitTheme(IInspectable const&, RoutedEventArgs const&);
		void SelectTheme(IInspectable const&, SelectionChangedEventArgs const&);
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct Settings : SettingsT<Settings, implementation::Settings>
	{
	};
}
