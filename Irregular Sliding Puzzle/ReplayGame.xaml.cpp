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
						border.BorderThickness({ 1, 1, 1, 1 });
						border.BorderBrush(ControlBorder());
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
			for (uint8_t i{}; i < height; ++i)
				for (uint8_t j{}; j < width; ++j)
					if (board.GetAt(i).GetAt(j))
						if (const uint16_t& val = content.GetAt(read_pos++) | content.GetAt(read_pos++) << 8)
						{
							const Button button;
							Grid::SetRow(button, i);
							Grid::SetColumn(button, j);
							button.Height(32);
							button.Width(32);
							button.Padding({});
							button.Content(box_value(to_hstring(val)));
							children.Append(buttons[i][j] = button);
						}
						else
							children.Append(empty = CreateWall(ex = i, ey = j));
					else
						children.Append(CreateWall(i, j));
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

	void ReplayGame::ResetButton(Button const& button, uint8_t const& x, uint8_t const& y)
	{
		Grid::SetRow(button, x);
		Grid::SetColumn(button, y);
		buttons[x][y] = button;
	}

	void ReplayGame::Move(uint8_t const& x, uint8_t const& y)
	{
		if (x == ex)
		{
			if (y < ey)
			{
				for (uint8_t i = y; i < ey; ++i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint8_t i = ey; i-- > y;)
					ResetButton(buttons[x][i], x, i + 1);
			}
			else
			{
				for (uint8_t i = y; i > ey; --i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint8_t i = ey; i++ < y;)
					ResetButton(buttons[x][i], x, i - 1);
			}
			ey = y;
		}
		else if (y == ey)
		{
			if (x < ex)
			{
				for (uint8_t i = x; i < ex; ++i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint8_t i = ex; i-- > x;)
					ResetButton(buttons[i][y], i + 1, y);
			}
			else
			{
				for (uint8_t i = x; i > ex; --i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint8_t i = ex; i++ < x;)
					ResetButton(buttons[i][y], i - 1, y);
			}
			ex = x;
		}
		Grid::SetRow(empty, ex);
		Grid::SetColumn(empty, ey);
	}
}
