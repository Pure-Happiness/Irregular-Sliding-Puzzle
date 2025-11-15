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
		void GridLoaded(IInspectable const&, RoutedEventArgs const&);
		void GridChecked(IInspectable const&, RoutedEventArgs const&);
		void GridUnchecked(IInspectable const&, RoutedEventArgs const&);
		void GraphLoaded(IInspectable const&, RoutedEventArgs const&);
		void GraphChecked(IInspectable const&, RoutedEventArgs const&);
		void GraphUnchecked(IInspectable const&, RoutedEventArgs const&);
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct Settings : SettingsT<Settings, implementation::Settings>
	{
	};
}
