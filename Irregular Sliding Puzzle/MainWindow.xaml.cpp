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
		const auto page = alive.Frame().Content().as<DesignGame>();
		page.Init(o_height, o_width, o_board);
		if (o_graph)
			page.Init(o_graph);
		title_bar.IsBackButtonVisible(false);
	}
}
