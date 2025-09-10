#include "pch.h"
#include "PlayGame.xaml.h"
#if __has_include("PlayGame.g.cpp")
#include "PlayGame.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	void PlayGame::Init(uint32_t const& _height, uint32_t const& _width, IVector<IVector<bool>> const& _board)
	{
		height = _height;
		width = _width;
		board = _board;
		numbers.assign(height, vector<uint32_t>(width));
		buttons.assign(height, vector<Button>(width, nullptr));
		{
			const RowDefinitionCollection rows = mummy().RowDefinitions();
			for (uint32_t i{}; i < height; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = mummy().ColumnDefinitions();
			for (uint32_t i{}; i < width; ++i)
				columns.Append(AutoColumn());
		}
		{
			const UIElementCollection children = mummy().Children();
			for (uint32_t i{}; i < height; ++i)
				for (uint32_t j{}; j < width; ++j)
					if (board.GetAt(i).GetAt(j))
					{
						children.Append(CreateGround(i, j));
						++num;
					}
					else
						children.Append(CreateWall(i, j));
		}
		if (!num)
		{
			Frame().GoBack();
			Frame().Content().as<DesignGame>().Init(height, width, board);
			return;
		}
		{
			const RowDefinitionCollection rows = target().RowDefinitions();
			for (uint32_t i{}; i < height; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = target().ColumnDefinitions();
			for (uint32_t i{}; i < width; ++i)
				columns.Append(AutoColumn());
		}
		{
			const UIElementCollection children = target().Children();
			for (uint32_t i{}, now = 1; i < height; ++i)
				for (uint32_t j{}; j < width; ++j)
					if (board.GetAt(i).GetAt(j))
					{
						const Border border = CreateGround(i, j);
						if (now < num)
						{
							const TextBlock text;
							text.Text(to_hstring(now++));
							text.VerticalAlignment(VerticalAlignment::Center);
							text.HorizontalAlignment(HorizontalAlignment::Center);
							border.Child(text);
						}
						children.Append(border);
					}
					else
						children.Append(CreateWall(i, j));
		}
		{
			const RowDefinitionCollection rows = baby().RowDefinitions();
			for (uint32_t i{}; i < height; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = baby().ColumnDefinitions();
			for (uint32_t i{}; i < width; ++i)
				columns.Append(AutoColumn());
		}
		{
			const UIElementCollection children = baby().Children();
			for (uint32_t i{}, now = 1; i < height; ++i)
				for (uint32_t j{}; j < width; ++j)
					if (board.GetAt(i).GetAt(j))
						if (now < num)
							children.Append(CreateButton(i, j, numbers[i][j] = now++));
						else
							ex = i, ey = j;
					else
						children.Append(CreateWall(i, j));
		}
		vector pos(height, vector<vector<pair<uint32_t, uint32_t>>>(width));
		for (uint32_t i{}; i < height; ++i)
			for (uint32_t j{}; j < width; ++j)
				if (board.GetAt(i).GetAt(j))
				{
					auto& pairs = pos[i][j];
					for (uint32_t k{ j + 1 }; k < width && board.GetAt(i).GetAt(k); ++k)
						pairs.emplace_back(i, k);
					for (uint32_t k{ j }; k-- && board.GetAt(i).GetAt(k);)
						pairs.emplace_back(i, k);
					for (uint32_t k{ i + 1 }; k < height && board.GetAt(k).GetAt(j); ++k)
						pairs.emplace_back(k, j);
					for (uint32_t k{ i }; k-- && board.GetAt(k).GetAt(j);)
						pairs.emplace_back(k, j);
					if (pairs.empty())
					{
						Frame().GoBack();
						Frame().Content().as<DesignGame>().Init(height, width, board);
						return;
					}
				}
		mt19937 r(static_cast<unsigned>(chrono::high_resolution_clock::now().time_since_epoch().count()) ^ random_device()());
		for (uint32_t i{}; i < 0x10000; ++i)
		{
			auto const& [x, y] = pos[ex][ey][r() % pos[ex][ey].size()];
			Move(x, y);
		}
		timer.Interval(1s);
		timer.Tick([this](DispatcherQueueTimer const&, IInspectable const&)
			{
				++time;
				const uint32_t minutes = time / 60, seconds = time - minutes * 60;
				Timer().Label((minutes < 10 ? L"0" : L"") + to_hstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + to_hstring(seconds));
			});
		timer.Start();
	}

	void PlayGame::Pause(IInspectable const&, RoutedEventArgs const&)
	{
		timer.Stop();
		pause().Visibility(Visibility::Collapsed);
		resume().Visibility(Visibility::Visible);
		mummy().Visibility(Visibility::Collapsed);
		baby().Visibility(Visibility::Collapsed);
	}

	void PlayGame::Resume(IInspectable const&, RoutedEventArgs const&)
	{
		pause().Visibility(Visibility::Visible);
		resume().Visibility(Visibility::Collapsed);
		mummy().Visibility(Visibility::Visible);
		baby().Visibility(Visibility::Visible);
		timer.Start();
	}

	void PlayGame::Surrender(IInspectable const&, RoutedEventArgs const&) const
	{
		Frame().GoBack();
		Frame().Content().as<DesignGame>().Init(height, width, board);
	}

	Border PlayGame::CreateGround(uint32_t const& x, uint32_t const& y)
	{
		const Border border;
		Grid::SetRow(border, x);
		Grid::SetColumn(border, y);
		border.Background(SolidFill());
		border.Height(32);
		border.Width(32);
		return border;
	}

	Border PlayGame::CreateWall(uint32_t const& x, uint32_t const& y)
	{
		const Border border;
		Grid::SetRow(border, x);
		Grid::SetColumn(border, y);
		border.Height(32);
		border.Width(32);
		return border;
	}

	Button PlayGame::CreateButton(uint32_t const& x, uint32_t const& y, uint32_t const& n)
	{
		const Button button;
		Grid::SetRow(button, x);
		Grid::SetColumn(button, y);
		button.Height(32);
		button.Width(32);
		button.Padding({});
		button.Content(box_value(to_hstring(n)));
		button.Click([this, button](IInspectable const&, RoutedEventArgs const&)
			{
				Move(Grid::GetRow(button), Grid::GetColumn(button));
				if (Complete())
				{
					timer.Stop();
					const ContentDialog dialog;
					dialog.Title(box_value(ResourceLoader().GetString(L"Congratulations")));
					const uint32_t minutes = time / 60, seconds = time - minutes * 60;
					dialog.Content(box_value(ResourceLoader().GetString(L"Time") + (minutes < 10 ? L"0" : L"") + to_hstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + to_hstring(seconds)));
					dialog.PrimaryButtonText(ResourceLoader().GetString(L"Back"));
					dialog.PrimaryButtonClick([this](ContentDialog const&, ContentDialogButtonClickEventArgs const&)
						{
							Frame().GoBack();
							Frame().Content().as<DesignGame>().Init(height, width, board);
						});
					dialog.XamlRoot(XamlRoot());
					dialog.ShowAsync();
				}
			});
		buttons[x][y] = button;
		return button;
	}

	void PlayGame::ResetButton(Button const& button, uint32_t const& x, uint32_t const& y)
	{
		Grid::SetRow(button, x);
		Grid::SetColumn(button, y);
		buttons[x][y] = button;
	}

	bool PlayGame::Complete() const
	{
		for (uint32_t i{}, now = 1; i < height; ++i)
			for (uint32_t j{}; j < width; ++j)
				if (board.GetAt(i).GetAt(j) && now < num && numbers[i][j] != now++)
					return false;
		return true;
	}

	void PlayGame::Move(uint32_t const& x, uint32_t const& y)
	{
		if (x == ex)
			if (y < ey)
			{
				for (uint32_t i = y; i < ey; ++i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint32_t i = ey; i-- > y;)
				{
					ResetButton(buttons[x][i], x, i + 1);
					numbers[x][i + 1] = numbers[x][i];
				}
				ey = y;
			}
			else
			{
				for (uint32_t i = y; i > ey; --i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint32_t i = ey; i++ < y;)
				{
					ResetButton(buttons[x][i], x, i - 1);
					numbers[x][i - 1] = numbers[x][i];
				}
				ey = y;
			}
		else if (y == ey)
			if (x < ex)
			{
				for (uint32_t i = x; i < ex; ++i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint32_t i = ex; i-- > x;)
				{
					ResetButton(buttons[i][y], i + 1, y);
					numbers[i + 1][y] = numbers[i][y];
				}
				ex = x;
			}
			else
			{
				for (uint32_t i = x; i > ex; --i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint32_t i = ex; i++ < x;)
				{
					ResetButton(buttons[i][y], i - 1, y);
					numbers[i - 1][y] = numbers[i][y];
				}
				ex = x;
			}
	}
}
