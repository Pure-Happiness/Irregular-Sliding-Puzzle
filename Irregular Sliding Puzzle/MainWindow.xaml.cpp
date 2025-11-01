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
			const DesignGame content;
			{
				const IVector<IVector<bool>> board = single_threaded_vector<IVector<bool>>();
				for (int i{}; i < 8; ++i)
				{
					const IVector<bool> row = single_threaded_vector<bool>();
					for (int j{}; j < 8; ++j)
						row.Append(false);
					board.Append(row);
				}
				content.Init(8, 8, board, Graph(), false);
			}
			(container = root()).Child(content);
		}
	}

	void MainWindow::ReturnToDesign(TitleBar const&, IInspectable const&)
	{
		title_bar.IsBackButtonVisible(false);
		alive.Timer().Stop();
		alive = nullptr;
		const DesignGame content;
		content.Init(o_height, o_width, o_board, o_graph, o_mode);
		container.Child(content);
	}
}
