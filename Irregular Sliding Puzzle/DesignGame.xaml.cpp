#include "pch.h"
#include "DesignGame.xaml.h"
#if __has_include("DesignGame.g.cpp")
#include "DesignGame.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	void DesignGame::Init()
	{
		{
			const UIElementCollection mum = mother().Children();
			for (int i{}; i < 16; ++i)
				mum.Append(CreateRemoveRow(i));
			mum.Append(CreateAddRow());
		}
		{
			const UIElementCollection dad = father().Children();
			for (int i{}; i < 16; ++i)
				dad.Append(CreateRemoveColumn(i));
			dad.Append(CreateAddColumn());
		}
		{
			const RowDefinitionCollection rows = baby().RowDefinitions();
			for (int i{}; i < 16; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = baby().ColumnDefinitions();
			for (int i{}; i < 16; ++i)
				columns.Append(AutoColumn());
		}
		const UIElementCollection children = baby().Children();
		buttons.assign(16, vector<Border>(16, nullptr));
		for (int i{}; i < 16; ++i)
		{
			const IVector current = single_threaded_vector<bool>();
			board.Append(current);
			for (int j{}; j < 16; ++j)
			{
				children.Append(CreateButton(i, j));
				current.Append(false);
			}
		}
	}

	void DesignGame::Init(uint32_t const& _height, uint32_t const& _width, IVector<IVector<bool>> const& _board)
	{
		height = _height;
		width = _width;
		{
			const UIElementCollection mum = mother().Children();
			for (uint32_t i{}; i < height; ++i)
				mum.Append(CreateRemoveRow(i));
			mum.Append(CreateAddRow());
		}
		{
			const UIElementCollection dad = father().Children();
			for (uint32_t i{}; i < width; ++i)
				dad.Append(CreateRemoveColumn(i));
			dad.Append(CreateAddColumn());
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
		const UIElementCollection children = baby().Children();
		buttons.assign(height, vector<Border>(width, nullptr));
		for (uint32_t i{}; i < height; ++i)
		{
			const IVector current = single_threaded_vector<bool>();
			board.Append(current);
			for (uint32_t j{}; j < width; ++j)
			{
				const bool v = _board.GetAt(i).GetAt(j);
				children.Append(CreateButton(i, j, v));
				current.Append(v);
			}
		}
	}

	void DesignGame::UpdateSize(IInspectable const&, SizeChangedEventArgs const& e)
	{
		daughter().Height(e.NewSize().Height);
		daughter().Width(e.NewSize().Width);
		son().Width(e.NewSize().Width - 32);
	}

	void DesignGame::DragStart(IInspectable const&, PointerRoutedEventArgs const& e)
	{
		if (const PointerPoint point = e.GetCurrentPoint(baby()); point.Properties().IsLeftButtonPressed())
		{
			start = point.Position();
			dragging = true;
		}
	}

	void DesignGame::DragEnd(IInspectable const&, PointerRoutedEventArgs const& e)
	{
		if (const PointerPoint point = e.GetCurrentPoint(baby()); point.Properties().PointerUpdateKind() == PointerUpdateKind::LeftButtonReleased && dragging)
		{
			const Point end = point.Position();
			uint32_t sx = static_cast<uint32_t>(start.X) / 32, sy = static_cast<uint32_t>(start.Y) / 32, ex = static_cast<uint32_t>(end.X) / 32, ey = static_cast<uint32_t>(end.Y) / 32;
			if (sx > ex)
				swap(sx, ex);
			if (sy > ey)
				swap(sy, ey);
			switch (status)
			{
			case Reverse:
				for (; sy <= ey; ++sy)
					for (uint32_t i = sx; i <= ex; ++i)
					{
						const bool value = !board.GetAt(sy).GetAt(i);
						board.GetAt(sy).SetAt(i, value);
						buttons[sy][i].Background(value ? AccentFill() : DefaultFill());
					}
				break;
			case Write:
				for (; sy <= ey; ++sy)
					for (uint32_t i = sx; i <= ex; ++i)
					{
						board.GetAt(sy).SetAt(i, true);
						buttons[sy][i].Background(AccentFill());
					}
				break;
			case Erase:
				for (; sy <= ey; ++sy)
					for (uint32_t i = sx; i <= ex; ++i)
					{
						board.GetAt(sy).SetAt(i, false);
						buttons[sy][i].Background(DefaultFill());
					}
			}
			dragging = false;
		}
	}

	void DesignGame::AsWrite(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Write;
		write().Background(AccentFill());
		reverse().Background(DefaultFill());
		erase().Background(DefaultFill());
	}

	void DesignGame::AsReverse(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Reverse;
		write().Background(DefaultFill());
		reverse().Background(AccentFill());
		erase().Background(DefaultFill());
		erase().Background(DefaultFill());
	}

	void DesignGame::AsErase(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Erase;
		write().Background(DefaultFill());
		reverse().Background(DefaultFill());
		erase().Background(AccentFill());
	}

	void DesignGame::StartGame(IInspectable const&, RoutedEventArgs const&) const
	{
		Frame().Navigate(xaml_typename<PlayGame>());
		Frame().Content().as<PlayGame>().Init(height, width, board);
	}

	Border DesignGame::CreateButton(uint32_t const& x, uint32_t const& y, bool const& v)
	{
		const Border button;
		Grid::SetRow(button, x);
		Grid::SetColumn(button, y);
		button.BorderThickness({ 1, 1, 1, 1 });
		button.Background(v ? AccentFill() : DefaultFill());
		button.Height(32);
		button.Width(32);
		button.PointerPressed([](IInspectable const&, PointerRoutedEventArgs const& e)
			{
				e.Handled(false);
			});
		button.PointerReleased([](IInspectable const&, PointerRoutedEventArgs const& e)
			{
				e.Handled(false);
			});
		buttons[x][y] = button;
		return button;
	}

	void DesignGame::ResetButton(Border const& button, uint32_t const& x, uint32_t const& y)
	{
		Grid::SetRow(button, x);
		Grid::SetColumn(button, y);
		buttons[x][y] = button;
	}

	Button DesignGame::CreateRemoveRow(uint32_t const& row)
	{
		const Button button;
		button.Padding({});
		button.Height(32);
		button.Width(32);
		{
			const FontIcon icon;
			icon.Glyph(L"\uECC9");
			button.Content(icon);
		}
		button.Click([this, row](IInspectable const&, RoutedEventArgs const&)
			{
				{
					const UIElementCollection mum = mother().Children();
					mum.RemoveAtEnd();
					mum.RemoveAtEnd();
					mum.Append(CreateAddRow());
				}
				baby().RowDefinitions().RemoveAtEnd();
				board.RemoveAt(row);
				for (uint32_t i = row + 1; i < height; ++i)
				{
					auto& buttonsi = buttons[i];
					for (uint32_t j{}; j < width; ++j)
						ResetButton(buttonsi[j], i - 1, j);
				}
				buttons.pop_back();
				--height;
				const UIElementCollection children = baby().Children();
				children.Clear();
				DispatcherQueue::GetForCurrentThread().TryEnqueue([this, children]()
					{
						for (auto const& i : buttons)
							for (Border const& j : i)
								children.Append(j);
					});
			});
		return button;
	}

	Button DesignGame::CreateRemoveColumn(uint32_t const& column)
	{
		const Button button;
		button.Padding({});
		button.Height(32);
		button.Width(32);
		{
			const FontIcon icon;
			icon.Glyph(L"\uECC9");
			button.Content(icon);
		}
		button.Click([this, column](IInspectable const&, RoutedEventArgs const&)
			{
				{
					const UIElementCollection dad = father().Children();
					dad.RemoveAtEnd();
					dad.RemoveAtEnd();
					dad.Append(CreateAddColumn());
				}
				baby().ColumnDefinitions().RemoveAtEnd();
				for (uint32_t i{}; i < height; ++i)
					board.GetAt(i).RemoveAt(column);
				for (uint32_t i{}; i < height; ++i)
				{
					auto& buttonsi = buttons[i];
					for (uint32_t j = column + 1; j < width; ++j)
						ResetButton(buttonsi[j], i, j - 1);
					buttonsi.pop_back();
				}
				--width;
				const UIElementCollection children = baby().Children();
				children.Clear();
				DispatcherQueue::GetForCurrentThread().TryEnqueue([this, children]()
					{
						for (auto const& i : buttons)
							for (Border const& j : i)
								children.Append(j);
					});
			});
		return button;
	}

	Button DesignGame::CreateAddRow()
	{
		const Button button;
		button.Padding({});
		button.Height(32);
		button.Width(32);
		{
			const FontIcon icon;
			icon.Glyph(L"\uECC8");
			button.Content(icon);
		}
		button.Click([this](IInspectable const&, RoutedEventArgs const&)
			{
				{
					const UIElementCollection mum = mother().Children();
					mum.RemoveAtEnd();
					mum.Append(CreateRemoveRow(height));
					mum.Append(CreateAddRow());
				}
				baby().RowDefinitions().Append(AutoRow());
				const UIElementCollection children = baby().Children();
				const IVector current = single_threaded_vector<bool>();
				board.Append(current);
				buttons.push_back(vector<Border>(width, nullptr));
				for (uint32_t j{}; j < width; ++j)
				{
					children.Append(CreateButton(height, j));
					current.Append(false);
				}
				++height;
			});
		return button;
	}

	Button DesignGame::CreateAddColumn()
	{
		const Button button;
		button.Padding({});
		button.Height(32);
		button.Width(32);
		{
			const FontIcon icon;
			icon.Glyph(L"\uECC8");
			button.Content(icon);
		}
		button.Click([this](IInspectable const&, RoutedEventArgs const&)
			{
				{
					const UIElementCollection dad = father().Children();
					dad.RemoveAtEnd();
					dad.Append(CreateRemoveColumn(width));
					dad.Append(CreateAddColumn());
				}
				baby().ColumnDefinitions().Append(AutoColumn());
				const UIElementCollection children = baby().Children();
				for (uint32_t i{}; i < height; ++i)
				{
					buttons[i].push_back(nullptr);
					children.Append(CreateButton(i, width));
					board.GetAt(i).Append(false);
				}
				++width;
			});
		return button;
	}
}
