#pragma once

#include "MainWindow.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct MainWindow : MainWindowT<MainWindow>
	{
		void ActivatedEvent(IInspectable const&, WindowActivatedEventArgs const&);

	private:
		bool activated;
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
