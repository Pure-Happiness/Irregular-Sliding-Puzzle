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
			for (uint8_t i{}; i < 16; ++i)
				mum.Append(CreateRemoveRow(i));
			mum.Append(CreateAddRow());
		}
		{
			const UIElementCollection dad = father().Children();
			for (uint8_t i{}; i < 16; ++i)
				dad.Append(CreateRemoveColumn(i));
			dad.Append(CreateAddColumn());
		}
		{
			const RowDefinitionCollection rows = baby().RowDefinitions();
			for (uint8_t i{}; i < 16; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = baby().ColumnDefinitions();
			for (uint8_t i{}; i < 16; ++i)
				columns.Append(AutoColumn());
		}
		board = single_threaded_vector<IVector<bool>>();
		const UIElementCollection children = baby().Children();
		buttons.assign(16, vector<Border>(16, nullptr));
		for (uint8_t i{}; i < 16; ++i)
		{
			const IVector current = single_threaded_vector<bool>();
			board.Append(current);
			for (uint8_t j{}; j < 16; ++j)
			{
				children.Append(CreateButton(i, j));
				current.Append(false);
			}
		}
	}

	void DesignGame::Init(uint8_t const& _height, uint8_t const& _width, IVector<IVector<bool>> const& _board)
	{
		height = _height;
		width = _width;
		board = _board;
		{
			const UIElementCollection mum = mother().Children();
			mum.Clear();
			for (uint8_t i{}; i < height; ++i)
				mum.Append(CreateRemoveRow(i));
			mum.Append(CreateAddRow());
		}
		{
			const UIElementCollection dad = father().Children();
			dad.Clear();
			for (uint8_t i{}; i < width; ++i)
				dad.Append(CreateRemoveColumn(i));
			dad.Append(CreateAddColumn());
		}
		{
			const RowDefinitionCollection rows = baby().RowDefinitions();
			rows.Clear();
			for (uint8_t i{}; i < height; ++i)
				rows.Append(AutoRow());
		}
		{
			const ColumnDefinitionCollection columns = baby().ColumnDefinitions();
			columns.Clear();
			for (uint8_t i{}; i < width; ++i)
				columns.Append(AutoColumn());
		}
		const UIElementCollection children = baby().Children();
		children.Clear();
		buttons.assign(height, vector<Border>(width, nullptr));
		for (uint8_t i{}; i < height; ++i)
			for (uint8_t j{}; j < width; ++j)
				children.Append(CreateButton(i, j, board.GetAt(i).GetAt(j)));
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
			uint8_t sx = static_cast<uint32_t>(start.X) / 32, sy = static_cast<uint32_t>(start.Y) / 32, ex = static_cast<uint32_t>(end.X) / 32, ey = static_cast<uint32_t>(end.Y) / 32;
			if (sx > ex)
				swap(sx, ex);
			if (sy > ey)
				swap(sy, ey);
			switch (status)
			{
			case Reverse:
				for (; sy <= ey; ++sy)
					for (uint8_t i = sx; i <= ex; ++i)
					{
						const bool value = !board.GetAt(sy).GetAt(i);
						board.GetAt(sy).SetAt(i, value);
						buttons[sy][i].Background(value ? AccentFill() : DefaultFill());
					}
				break;
			case Write:
				for (; sy <= ey; ++sy)
					for (uint8_t i = sx; i <= ex; ++i)
					{
						board.GetAt(sy).SetAt(i, true);
						buttons[sy][i].Background(AccentFill());
					}
				break;
			case Erase:
				for (; sy <= ey; ++sy)
					for (uint8_t i = sx; i <= ex; ++i)
					{
						board.GetAt(sy).SetAt(i, false);
						buttons[sy][i].Background(DefaultFill());
					}
			}
			dragging = false;
		}
	}

	void DesignGame::ToGraphMode(IInspectable const&, RoutedEventArgs const&)
	{
		is_graph = true;
		gridBoard().Visibility(Visibility::Collapsed);
		graphBoard().Visibility(Visibility::Visible);
		graphMode().Visibility(Visibility::Collapsed);
		gridMode().Visibility(Visibility::Visible);
		write().Visibility(Visibility::Collapsed);
		vertex().Visibility(Visibility::Visible);
		reverse().Visibility(Visibility::Collapsed);
		segment().Visibility(Visibility::Visible);
		erase().Visibility(Visibility::Collapsed);
		curve().Visibility(Visibility::Visible);
	}

	void DesignGame::ToGridMode(IInspectable const&, RoutedEventArgs const&)
	{
		is_graph = false;
		gridBoard().Visibility(Visibility::Visible);
		graphBoard().Visibility(Visibility::Collapsed);
		graphMode().Visibility(Visibility::Visible);
		gridMode().Visibility(Visibility::Collapsed);
		write().Visibility(Visibility::Visible);
		vertex().Visibility(Visibility::Collapsed);
		reverse().Visibility(Visibility::Visible);
		segment().Visibility(Visibility::Collapsed);
		erase().Visibility(Visibility::Visible);
		curve().Visibility(Visibility::Collapsed);
	}

	void DesignGame::AsWrite(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Write;
		write().Background(AccentFill());
		reverse().Background(DefaultFill());
		erase().Background(DefaultFill());
	}

	void DesignGame::AddVertices(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Vertex;
		vertex().Background(AccentFill());
		segment().Background(DefaultFill());
		curve().Background(DefaultFill());
	}

	void DesignGame::AsReverse(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Reverse;
		write().Background(DefaultFill());
		reverse().Background(AccentFill());
		erase().Background(DefaultFill());
	}

	void DesignGame::AddSegments(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Segment;
		vertex().Background(DefaultFill());
		segment().Background(AccentFill());
		curve().Background(DefaultFill());
	}

	void DesignGame::AsErase(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Erase;
		write().Background(DefaultFill());
		reverse().Background(DefaultFill());
		erase().Background(AccentFill());
	}

	void DesignGame::AddCurves(IInspectable const&, TappedRoutedEventArgs const&)
	{
		status = Curve;
		vertex().Background(DefaultFill());
		segment().Background(DefaultFill());
		curve().Background(AccentFill());
	}

	void DesignGame::Set(IInspectable const&, RoutedEventArgs const&) const
	{
		const ContentDialog dialog;
		dialog.XamlRoot(XamlRoot());
		dialog.Title(box_value(ResourceLoader().GetString(L"设置")));
		dialog.Content(Settings());
		dialog.CloseButtonText(ResourceLoader().GetString(L"Back"));
		dialog.DefaultButton(ContentDialogButton::Close);
		dialog.ShowAsync();
	}

	void DesignGame::Help(IInspectable const&, RoutedEventArgs const&) const
	{
		const ContentDialog dialog;
		dialog.XamlRoot(XamlRoot());
		dialog.Title(box_value(ResourceLoader().GetString(L"用户指南")));
		{
			const StackPanel panel;
			const UIElementCollection collection = panel.Children();
			{
				const HyperlinkButton block;
				block.Content(box_value(ResourceLoader().GetString(L"GitHub")));
				block.NavigateUri(Uri(ResourceLoader().GetString(L"GitHub Link")));
				collection.Append(block);
			}
			{
				const HyperlinkButton block;
				block.Content(box_value(ResourceLoader().GetString(L"Gitee")));
				block.NavigateUri(Uri(ResourceLoader().GetString(L"Gitee Link")));
				collection.Append(block);
			}
			dialog.Content(panel);
		}
		dialog.CloseButtonText(ResourceLoader().GetString(L"Back"));
		dialog.DefaultButton(ContentDialogButton::Close);
		dialog.ShowAsync();
	}

	void DesignGame::StartGame(IInspectable const&, RoutedEventArgs const&) const
	{
		Frame().Navigate(xaml_typename<PlayGame>());
		Frame().Content().as<PlayGame>().Init(height, width, board);
	}

	fire_and_forget DesignGame::DisplayRecords(IInspectable const&, RoutedEventArgs const&)
	{
		if (!loaded)
		{
			loaded = true;
			stack<UIElement> st;
			for (StorageFile const& file : co_await ApplicationData::GetDefault().LocalFolder().GetFilesAsync())
			{
				const IBuffer buffer = co_await FileIO::ReadBufferAsync(file);
				const Grid grid;
				grid.CornerRadius({ 4, 4, 4, 4 });
				grid.Background(DataReader::FromBuffer(buffer).ReadByte() ? SuccessBrush() : CriticalBrush());
				{
					const ColumnDefinitionCollection columns = grid.ColumnDefinitions();
					columns.Append(ColumnDefinition());
					columns.Append(AutoColumn());
					columns.Append(AutoColumn());
				}
				const UIElementCollection children = grid.Children();
				{
					const TextBlock block;
					block.Margin({ 8, 8, 8, 8 });
					block.VerticalAlignment(VerticalAlignment::Center);
					const time_t time = stoull(file.Name().data());
					wostringstream sb;
					sb << put_time(localtime(&time), L"%Y-%m-%d %H:%M:%S");
					block.Text(sb.str());
					children.Append(block);
				}
				{
					const Button button;
					button.Padding({ 4, 4, 4, 4 });
					button.VerticalAlignment(VerticalAlignment::Center);
					Grid::SetColumn(button, 1);
					{
						const FontIcon icon;
						icon.Glyph(L"\uF413");
						icon.FontSize(12);
						button.Content(icon);
					}
					button.Click([this, buffer](IInspectable const&, RoutedEventArgs const&)
						{
							const DataReader reader = DataReader::FromBuffer(buffer);
							reader.ReadByte();
							const uint8_t height = reader.ReadByte(), width = reader.ReadByte();
							const IVector board = single_threaded_vector<IVector<bool>>();
							{
								const uint32_t size = height * width, len = size + 7 >> 3 << 3;
								vector<bool> flat(len);
								for (uint32_t i{}; i < len; i += 8)
								{
									const uint8_t byte = reader.ReadByte();
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
							Init(height, width, board);
						});
					children.Append(button);
				}
				{
					const Button button;
					button.Padding({ 4, 4, 4, 4 });
					button.Margin({ 8, 8, 8, 8 });
					Grid::SetColumn(button, 2);
					{
						const FontIcon icon;
						icon.Glyph(L"\uEF3B");
						icon.FontSize(12);
						button.Content(icon);
					}
					button.Click([this, buffer](IInspectable const&, RoutedEventArgs const&)
						{
							const IVector content = single_threaded_vector<uint8_t>();
							for (uint8_t* data = buffer.data(), *end = data + buffer.Length(); data < end; ++data)
								content.Append(*data);
							Frame().Navigate(xaml_typename<ReplayGame>());
							o_height = height;
							o_width = width;
							o_board = board;
							title_bar.IsBackButtonVisible(true);
							(alive = Frame().Content().as<ReplayGame>()).Init(content);
						});
					children.Append(button);
				}
				st.push(grid);
			}
			const UIElementCollection elements = records().Children();
			while (!st.empty())
			{
				elements.Append(st.top());
				st.pop();
			}
		}
	}

	Border DesignGame::CreateButton(uint8_t const& x, uint8_t const& y, bool const& v)
	{
		const Border button;
		Grid::SetRow(button, x);
		Grid::SetColumn(button, y);
		button.BorderThickness({ 1, 1, 1, 1 });
		button.BorderBrush(ControlBorder());
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
		return buttons[x][y] = button;
	}

	void DesignGame::ResetButton(Border const& button, uint8_t const& x, uint8_t const& y)
	{
		Grid::SetRow(button, x);
		Grid::SetColumn(button, y);
		buttons[x][y] = button;
	}

	Button DesignGame::CreateRemoveRow(uint8_t const& row)
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
				for (uint8_t i = row + 1; i < height; ++i)
				{
					auto& buttonsi = buttons[i];
					for (uint8_t j{}; j < width; ++j)
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

	Button DesignGame::CreateRemoveColumn(uint8_t const& column)
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
				for (uint8_t i{}; i < height; ++i)
					board.GetAt(i).RemoveAt(column);
				for (uint8_t i{}; i < height; ++i)
				{
					auto& buttonsi = buttons[i];
					for (uint8_t j = column + 1; j < width; ++j)
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
				if (height < 0xFF)
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
					for (uint8_t j{}; j < width; ++j)
					{
						children.Append(CreateButton(height, j));
						current.Append(false);
					}
					++height;
				}
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
				if (width < 0xFF)
				{
					{
						const UIElementCollection dad = father().Children();
						dad.RemoveAtEnd();
						dad.Append(CreateRemoveColumn(width));
						dad.Append(CreateAddColumn());
					}
					baby().ColumnDefinitions().Append(AutoColumn());
					const UIElementCollection children = baby().Children();
					for (uint8_t i{}; i < height; ++i)
					{
						buttons[i].push_back(nullptr);
						children.Append(CreateButton(i, width));
						board.GetAt(i).Append(false);
					}
					++width;
				}
			});
		return button;
	}
}
