#include "pch.h"
#include "PlayGame.xaml.h"
#if __has_include("PlayGame.g.cpp")
#include "PlayGame.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	bool PlayGame::Init(uint8_t const& _height, uint8_t const& _width, IVector<IVector<bool>> const& _board)
	{
		height = _height, width = _width, board = _board;
		numbers.assign(height, vector<uint16_t>(width));
		buttons.assign(height, vector<Button>(width, nullptr));
		CreateBackground(baby(), height, width, board, num);
		if (!num)
			return false;
		CreateTarget(target(), height, width, board, num);
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
							return false;
					}
			mt19937 r(static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count()) ^ random_device()());
			for (uint32_t i{}; i < 0x10000; ++i)
			{
				auto const& [x, y] = pos[ex][ey][r() % pos[ex][ey].size()];
				CommonMove(x, y, ex, ey, board, mem_fn(&PlayGame::MoveRaw), this);
			}
			numbers[ex][ey] = 0;
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
		SetTimer(timer, time, Timer());
		timer.Start();
		start_time = chrono::steady_clock::now();
		return true;
	}

	void PlayGame::Pause(IInspectable const&, RoutedEventArgs const&)
	{
		timer.Stop();
		pause().Visibility(Visibility::Collapsed);
		resume().Visibility(Visibility::Visible);
		baby().Visibility(Visibility::Collapsed);
	}

	void PlayGame::Resume(IInspectable const&, RoutedEventArgs const&)
	{
		pause().Visibility(Visibility::Visible);
		resume().Visibility(Visibility::Collapsed);
		baby().Visibility(Visibility::Visible);
		timer.Start();
	}

	void PlayGame::Surrender(IInspectable const&, RoutedEventArgs const&) const
	{
		timer.Stop();
		WriteRecord();
		GoBack();
	}

	void PlayGame::GoBack() const
	{
		const DesignGame content;
		content.Init(height, width, board, o_graph, false);
		container.Child(content);
	}

	Button PlayGame::CreateButton(uint8_t const& x, uint8_t const& y, uint16_t const& n)
	{
		const Button button = CommonButton(x, y, n);
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
					CommonMove(row, column, ex, ey, board, mem_fn(&PlayGame::MoveRaw), this);
				}
				uint16_t now = 1;
				for (uint8_t i{}; i < height; ++i)
					for (uint8_t j{}; j < width; ++j)
						if (board.GetAt(i).GetAt(j) && now < num && numbers[i][j] != now++)
							goto fail;
				timer.Stop();
				record.front() = 1;
				WriteRecord();
				Congratulations(time, XamlRoot(), mem_fn(&PlayGame::GoBack), this);
			fail:;
			});
		return buttons[x][y] = button;
	}

	void PlayGame::MoveRaw(uint8_t const& x, uint8_t const& y)
	{
		const Button& button = buttons[x][y];
		Grid::SetRow(button, ex);
		Grid::SetColumn(button, ey);
		buttons[ex][ey] = button;
		numbers[ex][ey] = numbers[x][y];
		ex = x, ey = y;
	}

	fire_and_forget PlayGame::WriteRecord() const
	{
		FileIO::WriteBytesAsync(co_await ApplicationData::GetDefault().LocalFolder().CreateFileAsync(to_hstring(std::time(nullptr))), record);
	}
}
