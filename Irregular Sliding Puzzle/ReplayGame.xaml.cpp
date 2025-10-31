#include "pch.h"
#include "ReplayGame.xaml.h"
#if __has_include("ReplayGame.g.cpp")
#include "ReplayGame.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	void ReplayGame::Init(IVector<uint8_t> const& _content)
	{
		height = (content = _content).GetAt(1);
		width = content.GetAt(2);
		buttons.assign(height, vector<Button>(width, nullptr));
		{
			const uint32_t size = height * width, len = size + 7 >> 3 << 3;
			vector<bool> flat(len);
			for (uint32_t i{}; i < len; i += 8)
			{
				const uint8_t byte = content.GetAt(read_pos++);
				flat[i] = byte & 1;
				flat[i + 1] = byte >> 1 & 1;
				flat[i + 2] = byte >> 2 & 1;
				flat[i + 3] = byte >> 3 & 1;
				flat[i + 4] = byte >> 4 & 1;
				flat[i + 5] = byte >> 5 & 1;
				flat[i + 6] = byte >> 6 & 1;
				flat[i + 7] = byte >> 7;
			}
			auto pt = flat.begin();
			for (uint8_t i{}; i < height; ++i)
			{
				const IVector current = single_threaded_vector<bool>();
				board.Append(current);
				for (uint8_t j{}; j < width; ++j, ++pt)
					current.Append(*pt);
			}
		}
		CreateBackground(baby(), height, width, board, num);
		CreateTarget(target(), height, width, board, num);
		{
			const UIElementCollection children = baby().Children();
			for (uint8_t i{}; i < height; ++i)
				for (uint8_t j{}; j < width; ++j)
					if (board.GetAt(i).GetAt(j))
						if (const uint16_t& val = content.GetAt(read_pos++) | content.GetAt(read_pos++) << 8)
							children.Append(buttons[i][j] = CommonButton(i, j, val));
						else
							children.Append(empty = CommonBorder(ex = i, ey = j));
					else
						children.Append(CommonBorder(i, j));
		}
		if (read_pos >= content.Size())
		{
			resume().IsEnabled(false);
			next().IsEnabled(false);
		}
		timer.Tick([this](IInspectable const&, IInspectable const&)
			{
				empties.emplace(ex, ey);
				const uint32_t current = content.GetAt(read_pos++) | content.GetAt(read_pos++) << 8 | content.GetAt(read_pos++) << 16 | content.GetAt(read_pos++) << 24;
				const uint8_t x = content.GetAt(read_pos++), y = content.GetAt(read_pos++);
				Move(x, y);
				if (read_pos >= content.Size())
				{
					Pause(nullptr, nullptr);
					resume().IsEnabled(false);
					next().IsEnabled(false);
				}
				else
					timer.Interval(chrono::milliseconds((long long)(((content.GetAt(read_pos) | content.GetAt(read_pos + 1) << 8 | content.GetAt(read_pos + 2) << 16 | content.GetAt(read_pos + 3) << 24) - current) / speed)));
			});
	}

	DispatcherTimer ReplayGame::Timer() const
	{
		return timer;
	}

	void ReplayGame::Resume(IInspectable const&, RoutedEventArgs const&)
	{
		resume().Visibility(Visibility::Collapsed);
		pause().Visibility(Visibility::Visible);
		previous().IsEnabled(false);
		next().IsEnabled(false);
		timer.Start();
	}

	void ReplayGame::Pause(IInspectable const&, RoutedEventArgs const&)
	{
		resume().Visibility(Visibility::Visible);
		pause().Visibility(Visibility::Collapsed);
		previous().IsEnabled(true);
		next().IsEnabled(true);
		timer.Stop();
	}

	void ReplayGame::Previous(IInspectable const&, RoutedEventArgs const&)
	{
		auto [x, y] = empties.top();
		empties.pop();
		read_pos -= 6;
		Move(x, y);
		if (empties.empty())
			previous().IsEnabled(false);
		resume().IsEnabled(true);
		next().IsEnabled(true);
	}

	void ReplayGame::Next(IInspectable const&, RoutedEventArgs const&)
	{
		empties.emplace(ex, ey);
		const uint32_t current = content.GetAt(read_pos++) | content.GetAt(read_pos++) << 8 | content.GetAt(read_pos++) << 16 | content.GetAt(read_pos++) << 24;
		const uint8_t x = content.GetAt(read_pos++), y = content.GetAt(read_pos++);
		Move(x, y);
		if (read_pos >= content.Size())
		{
			resume().IsEnabled(false);
			next().IsEnabled(false);
		}
		else
			timer.Interval(chrono::milliseconds((long long)(((content.GetAt(read_pos) | content.GetAt(read_pos + 1) << 8 | content.GetAt(read_pos + 2) << 16 | content.GetAt(read_pos + 3) << 24) - current) / speed)));
		previous().IsEnabled(true);
	}

	void ReplayGame::ChooseSpeed(IInspectable const& sender, SelectionChangedEventArgs const&)
	{
		switch (sender.as<ComboBox>().SelectedIndex())
		{
		case 0:
			speed = 0.5;
			break;
		case 1:
			speed = 1;
			break;
		case 2:
			speed = 2;
			break;
		case 3:
			speed = 3;
			break;
		case 4:
			speed = 4;
			break;
		case 5:
			speed = 8;
			break;
		default:;
		}
	}

	void ReplayGame::MoveRaw(uint8_t const& x, uint8_t const& y)
	{
		const Button& button = buttons[x][y];
		Grid::SetRow(button, ex);
		Grid::SetColumn(button, ey);
		buttons[ex][ey] = button;
		ex = x, ey = y;
	}

	void ReplayGame::Move(uint8_t const& x, uint8_t const& y)
	{
		if (x == ex)
			if (y < ey)
			{
				for (uint8_t i = y; i < ey; ++i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint8_t i = ey; i-- > y;)
					MoveRaw(x, i);
			}
			else
			{
				for (uint8_t i = y; i > ey; --i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint8_t i = ey; i++ < y;)
					MoveRaw(x, i);
			}
		else if (y == ey)
			if (x < ex)
			{
				for (uint8_t i = x; i < ex; ++i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint8_t i = ex; i-- > x;)
					MoveRaw(i, y);
			}
			else
			{
				for (uint8_t i = x; i > ex; --i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint8_t i = ex; i++ < x;)
					MoveRaw(i, y);
			}
		Grid::SetRow(empty, ex);
		Grid::SetColumn(empty, ey);
	}
}
