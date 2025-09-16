#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	void MainWindow::ActivatedEvent(IInspectable const&, WindowActivatedEventArgs const&)
	{
		if (!activated)
		{
			activated = true;
			Title(ResourceLoader().GetString(L"Title"));
			ExtendsContentIntoTitleBar(true);
			SetTitleBar(title_bar = title());
			root().Navigate(xaml_typename<DesignGame>());
			root().Content().as<DesignGame>().Init();
		}
	}

	void MainWindow::ReturnToDesign(TitleBar const&, IInspectable const&)
	{
		alive.Timer().Stop();
		alive.Frame().GoBack();
		alive.Frame().Content().as<DesignGame>().Init(o_height, o_width, o_board);
		title_bar.IsBackButtonVisible(false);
	}
}
