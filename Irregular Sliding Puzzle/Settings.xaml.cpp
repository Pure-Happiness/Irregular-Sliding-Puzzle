#include "pch.h"
#include "Settings.xaml.h"
#if __has_include("Settings.g.cpp")
#include "Settings.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	void Settings::InitLanguage(IInspectable const&, RoutedEventArgs const&)
	{
		const hstring first = *ApplicationLanguages::Languages().First();
		language().SelectedIndex(first[0] == L'z' && first[1] == L'h');
	}

	void Settings::SelectLanguage(IInspectable const&, SelectionChangedEventArgs const&)
	{
		ApplicationData::GetDefault().LocalSettings().Values().Insert(L"Language", box_value(language().SelectedIndex()));
	}

	void Settings::InitTheme(IInspectable const&, RoutedEventArgs const&)
	{
		if (const IInspectable th = ApplicationData::GetDefault().LocalSettings().Values().TryLookup(L"Theme"))
			theme().SelectedIndex(unbox_value<int32_t>(th));
		else
			theme().SelectedIndex(2);
	}

	void Settings::SelectTheme(IInspectable const&, SelectionChangedEventArgs const&)
	{
		ApplicationData::GetDefault().LocalSettings().Values().Insert(L"Theme", box_value(theme().SelectedIndex()));
	}
}
