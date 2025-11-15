#include "pch.h"
#include "App.xaml.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	/// <summary>
	/// Initializes the singleton application object.  This is the first line of authored code
	/// executed, and as such is the logical equivalent of main() or WinMain().
	/// </summary>
	App::App()
	{
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
		UnhandledException([](IInspectable const&, UnhandledExceptionEventArgs const& e)
			{
				if (IsDebuggerPresent())
				{
					auto errorMessage = e.Message();
					__debugbreak();
				}
			});
#endif
		const IPropertySet settings = ApplicationData::GetDefault().LocalSettings().Values();
		if (const IInspectable theme = settings.TryLookup(L"Theme"))
			if (const auto value = unbox_value<int32_t>(theme); value < 2)
				Application::Current().RequestedTheme(static_cast<ApplicationTheme>(value));
		if (const IInspectable language = settings.TryLookup(L"Language"))
			ApplicationLanguages::PrimaryLanguageOverride(unbox_value<int32_t>(language) ? L"zh-CN" : L"en-US");
		if (const IInspectable record_grid = settings.TryLookup(L"GridRecord"))
			grid_record = unbox_value<bool>(record_grid);
		if (const IInspectable record_graph = settings.TryLookup(L"GraphRecord"))
			graph_record = unbox_value<bool>(record_graph);
	}

	/// <summary>
	/// Invoked when the application is launched.
	/// </summary>
	void App::OnLaunched(LaunchActivatedEventArgs const&)
	{
		MainWindow().Activate();
	}
}
