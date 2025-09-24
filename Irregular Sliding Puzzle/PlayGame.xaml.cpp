#include "pch.h"
#include "PlayGame.xaml.h"
#if __has_include("PlayGame.g.cpp")
#include "PlayGame.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	void PlayGame::Init(uint8_t const& _height, uint8_t const& _width, IVector<IVector<bool>> const& _board)
	{
		height = _height;
		width = _width;
		board = _board;
		numbers.assign(height, vector<uint16_t>(width));
		buttons.assign(height, vector<Button>(width, nullptr));
		{
			const RowDefinitionCollection rows = mummy().RowDefinitions();
			for (uint8_t i{}; i < height; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = mummy().ColumnDefinitions();
			for (uint8_t i{}; i < width; ++i)
				columns.Append(AutoColumn());
		}
		{
			const UIElementCollection children = mummy().Children();
			for (uint8_t i{}; i < height; ++i)
				for (uint8_t j{}; j < width; ++j)
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
			for (uint8_t i{}; i < height; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = target().ColumnDefinitions();
			for (uint8_t i{}; i < width; ++i)
				columns.Append(AutoColumn());
		}
		{
			const UIElementCollection children = target().Children();
			uint16_t now = 1;
			for (uint8_t i{}; i < height; ++i)
				for (uint8_t j{}; j < width; ++j)
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
			for (uint8_t i{}; i < height; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = baby().ColumnDefinitions();
			for (uint8_t i{}; i < width; ++i)
				columns.Append(AutoColumn());
		}
		{
			const UIElementCollection children = baby().Children();
			uint16_t now = 1;
			for (uint8_t i{}; i < height; ++i)
				for (uint8_t j{}; j < width; ++j)
					if (board.GetAt(i).GetAt(j))
						if (now < num)
							children.Append(CreateButton(i, j, numbers[i][j] = now++));
						else
							ex = i, ey = j;
					else
						children.Append(CreateWall(i, j));
		}
		{
			vector pos(height, vector<vector<pair<uint8_t, uint8_t>>>(width));
			for (uint8_t i{}; i < height; ++i)
				for (uint8_t j{}; j < width; ++j)
					if (board.GetAt(i).GetAt(j))
					{
						auto& pairs = pos[i][j];
						for (uint8_t k = j + 1; k < width && board.GetAt(i).GetAt(k); ++k)
							pairs.emplace_back(i, k);
						for (uint8_t k = j; k-- && board.GetAt(i).GetAt(k);)
							pairs.emplace_back(i, k);
						for (uint8_t k = i + 1; k < height && board.GetAt(k).GetAt(j); ++k)
							pairs.emplace_back(k, j);
						for (uint8_t k = i; k-- && board.GetAt(k).GetAt(j);)
							pairs.emplace_back(k, j);
						if (pairs.empty())
						{
							Frame().GoBack();
							Frame().Content().as<DesignGame>().Init(height, width, board);
							return;
						}
					}
			mt19937 r(static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count()) ^ random_device()());
			for (uint32_t i{}; i < 0x10000; ++i)
			{
				auto const& [x, y] = pos[ex][ey][r() % pos[ex][ey].size()];
				Move(x, y);
			}
		}
		record.push_back(0);
		record.push_back(height);
		record.push_back(width);
		{
			vector<bool> flat;
			for (auto i : board)
				flat.append_range(i);
			flat.resize(flat.size() + 7 >> 3 << 3);
			for (uint32_t i{}; i < flat.size(); i += 8)
				record.push_back(flat[i] | flat[i + 1] << 1 | flat[i + 2] << 2 | flat[i + 3] << 3 | flat[i + 4] << 4 | flat[i + 5] << 5 | flat[i + 6] << 6 | flat[i + 7] << 7);
		}
		for (uint8_t i{}; i < height; ++i)
			for (uint8_t j{}; j < width; ++j)
				if (board.GetAt(i).GetAt(j))
				{
					const uint16_t num = numbers[i][j];
					record.push_back(num & 0xFF);
					record.push_back(num >> 8);
				}
		timer.Interval(1s);
		timer.Tick([this](IInspectable const&, IInspectable const&)
			{
				++time;
				const uint32_t minutes = time / 60, seconds = time - minutes * 60;
				Timer().Label((minutes < 10 ? L"0" : L"") + to_hstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + to_hstring(seconds));
			});
		timer.Start();
		start_time = chrono::steady_clock::now();
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
		timer.Stop();
		WriteRecord();
		Frame().GoBack();
		Frame().Content().as<DesignGame>().Init(height, width, board);
	}

	Button PlayGame::CreateButton(uint8_t const& x, uint8_t const& y, uint16_t const& n)
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
				{
					const uint32_t duration = duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count();
					record.push_back(duration & 0xFF);
					record.push_back(duration >> 8 & 0xFF);
					record.push_back(duration >> 16 & 0xFF);
					record.push_back(duration >> 24);
				}
				{
					const uint8_t row = Grid::GetRow(button), column = Grid::GetColumn(button);
					record.push_back(row);
					record.push_back(column);
					Move(row, column);
				}
				if (Complete())
				{
					timer.Stop();
					record.front() = 1;
					WriteRecord();
					const ContentDialog dialog;
					dialog.Title(box_value(ResourceLoader().GetString(L"Congratulations")));
					const uint32_t minutes = time / 60, seconds = time - minutes * 60;
					dialog.Content(box_value(ResourceLoader().GetString(L"Time") + (minutes < 10 ? L"0" : L"") + to_hstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + to_hstring(seconds)));
					dialog.CloseButtonText(ResourceLoader().GetString(L"Back"));
					dialog.CloseButtonClick([this](ContentDialog const&, ContentDialogButtonClickEventArgs const&)
						{
							Frame().GoBack();
							Frame().Content().as<DesignGame>().Init(height, width, board);
						});
					dialog.DefaultButton(ContentDialogButton::Close);
					dialog.XamlRoot(XamlRoot());
					dialog.ShowAsync();
				}
			});
		return buttons[x][y] = button;
	}

	void PlayGame::ResetButton(Button const& button, uint8_t const& x, uint8_t const& y)
	{
		Grid::SetRow(button, x);
		Grid::SetColumn(button, y);
		buttons[x][y] = button;
	}

	bool PlayGame::Complete() const
	{
		uint16_t now = 1;
		for (uint8_t i{}; i < height; ++i)
			for (uint8_t j{}; j < width; ++j)
				if (board.GetAt(i).GetAt(j) && now < num && numbers[i][j] != now++)
					return false;
		return true;
	}

	void PlayGame::Move(uint8_t const& x, uint8_t const& y)
	{
		if (x == ex)
			if (y < ey)
			{
				for (uint8_t i = y; i < ey; ++i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint8_t i = ey; i-- > y;)
				{
					ResetButton(buttons[x][i], x, i + 1);
					numbers[x][i + 1] = numbers[x][i];
				}
				numbers[x][ey = y] = 0;
			}
			else
			{
				for (uint8_t i = y; i > ey; --i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint8_t i = ey; i++ < y;)
				{
					ResetButton(buttons[x][i], x, i - 1);
					numbers[x][i - 1] = numbers[x][i];
				}
				numbers[x][ey = y] = 0;
			}
		else if (y == ey)
			if (x < ex)
			{
				for (uint8_t i = x; i < ex; ++i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint8_t i = ex; i-- > x;)
				{
					ResetButton(buttons[i][y], i + 1, y);
					numbers[i + 1][y] = numbers[i][y];
				}
				numbers[ex = x][y] = 0;
			}
			else
			{
				for (uint8_t i = x; i > ex; --i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint8_t i = ex; i++ < x;)
				{
					ResetButton(buttons[i][y], i - 1, y);
					numbers[i - 1][y] = numbers[i][y];
				}
				numbers[ex = x][y] = 0;
			}
	}

	fire_and_forget PlayGame::WriteRecord() const
	{
		FileIO::WriteBytesAsync(co_await ApplicationData::GetDefault().LocalFolder().CreateFileAsync(to_hstring(std::time(nullptr))), record);
	}
}
