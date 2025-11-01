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
		RowsColumns(baby(), 16, 16);
		board = single_threaded_vector<IVector<bool>>();
		const UIElementCollection children = baby().Children();
		cells.assign(16, vector<Border>(16, nullptr));
		for (uint8_t i{}; i < 16; ++i)
		{
			const IVector current = single_threaded_vector<bool>();
			board.Append(current);
			for (uint8_t j{}; j < 16; ++j)
			{
				children.Append(CreateCell(i, j));
				current.Append(false);
			}
		}
	}

	void DesignGame::Init(uint8_t const& _height, uint8_t const& _width, IVector<IVector<bool>> const& _board)
	{
		height = _height, width = _width, board = _board;
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
		RowsColumns(baby(), height, width);
		const UIElementCollection children = baby().Children();
		children.Clear();
		cells.assign(height, vector<Border>(width, nullptr));
		for (uint8_t i{}; i < height; ++i)
			for (uint8_t j{}; j < width; ++j)
				children.Append(CreateCell(i, j, board.GetAt(i).GetAt(j)));
	}

	void DesignGame::Init(GraphP const& _g)
	{
		g = _g;
		const UIElementCollection collection = draw().Children();
		g.ForEachVertex([this, &collection](IInspectable const& v)
			{
				collection.Append(CreateVertex(v));
			});
		g.ForEachEdge([this, &collection](IInspectable const& e, IInspectable const& u, IInspectable const& v)
			{
				collection.Append(CreateEdge(e.as<IVector<Point>>(), u, v));
			});
	}

	void DesignGame::AsGraphMode()
	{
		ToGraphMode(nullptr, nullptr);
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
			uint8_t sx = static_cast<uint32_t>(start.X) >> 5, sy = static_cast<uint32_t>(start.Y) >> 5, ex = static_cast<uint32_t>(end.X) >> 5, ey = static_cast<uint32_t>(end.Y) >> 5;
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
						cells[sy][i].Background(value ? AccentFill() : DefaultFill());
					}
				break;
			case Write:
				for (; sy <= ey; ++sy)
					for (uint8_t i = sx; i <= ex; ++i)
					{
						board.GetAt(sy).SetAt(i, true);
						cells[sy][i].Background(AccentFill());
					}
				break;
			case Erase:
				for (; sy <= ey; ++sy)
					for (uint8_t i = sx; i <= ex; ++i)
					{
						board.GetAt(sy).SetAt(i, false);
						cells[sy][i].Background(DefaultFill());
					}
			}
			dragging = false;
		}
	}

	void DesignGame::DrawPressed(IInspectable const&, PointerRoutedEventArgs const& e)
	{
		if (const PointerPoint point = e.GetCurrentPoint(draw()); point.Properties().IsLeftButtonPressed())
		{
			start = point.Position();
			dragging = true;
			if (graph_status == Vertex)
			{
				for (auto const& p : vertices | views::keys)
				{
					const auto& [cx, cy] = unbox_value<Point>(p);
					if (const float dx = cx - start.X, dy = cy - start.Y; dx * dx + dy * dy < 32 * 32)
						return;
				}
				const IInspectable bs = box_value(start);
				g.AddVertex(bs);
				draw().Children().Append(CreateVertex(bs));
			}
		}
	}

	void DesignGame::DrawMoved(IInspectable const&, PointerRoutedEventArgs const& e)
	{
		if (const PointerPoint point = e.GetCurrentPoint(draw()); point.Properties().IsLeftButtonPressed() && dragging && graph_status == Curve)
			intermediate.push_back(point.Position());
	}

	void DesignGame::DrawReleased(IInspectable const&, PointerRoutedEventArgs const& e)
	{
		if (const PointerPoint point = e.GetCurrentPoint(draw()); point.Properties().PointerUpdateKind() == PointerUpdateKind::LeftButtonReleased && dragging && graph_status != Vertex)
		{
			const auto [sx, sy] = start;
			const auto [ex, ey] = point.Position();
			IInspectable u = nullptr, v = nullptr;
			for (const auto& p : vertices | views::keys)
				if (auto const& [cx, cy] = unbox_value<Point>(p); abs(cx - sx) < 16 && abs(cy - sy) < 16)
				{
					u = p;
					goto suc1;
				}
			goto end;
		suc1:
			for (const auto& p : vertices | views::keys)
				if (auto const& [cx, cy] = unbox_value<Point>(p); abs(cx - ex) < 16 && abs(cy - ey) < 16)
				{
					v = p;
					goto suc2;
				}
			goto end;
		suc2:
			{
				const IVector<Point> im = single_threaded_vector<Point>();
				if (graph_status == Curve)
				{
					for (Point const& p : intermediate)
						im.Append(p);
				}
				g.AddEdge(im, u, v);
				draw().Children().Append(CreateEdge(im, u, v));
			}
		end:
			intermediate.clear();
			dragging = false;
		}
	}

	void DesignGame::ToGraphMode(IInspectable const&, RoutedEventArgs const&)
	{
		if (g == nullptr)
			g = Graph();
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
		if (board == nullptr)
			Init();
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
		graph_status = Vertex;
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
		graph_status = Segment;
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
		graph_status = Curve;
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
		if (is_graph)
		{
			o_height = height, o_width = width, o_board = board;
			Frame().Navigate(xaml_typename<PlayGraph>());
			Frame().Content().as<PlayGraph>().Init(g);
		}
		else
		{
			o_graph = g;
			Frame().Navigate(xaml_typename<PlayGame>());
			Frame().Content().as<PlayGame>().Init(height, width, board);
		}
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
							o_height = height, o_width = width, o_board = board, o_graph = g;
							title_bar.IsBackButtonVisible(true);
							(alive = Frame().Content().as<ReplayGame>()).Init(content.GetView());
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

	void DesignGame::AllSkip(UIElement const& element)
	{
		constexpr auto PointerSkip = [](IInspectable const&, PointerRoutedEventArgs const& e)
			{
				e.Handled(false);
			};
		element.PointerPressed(PointerSkip);
		element.PointerMoved(PointerSkip);
		element.PointerReleased(PointerSkip);
	}

	FontIcon DesignGame::RemoveIcon()
	{
		const FontIcon icon;
		icon.Glyph(L"\uECC9");
		return icon;
	}

	FontIcon DesignGame::AddIcon()
	{
		const FontIcon icon;
		icon.Glyph(L"\uECC8");
		return icon;
	}

	Button DesignGame::AddRemove(FontIcon const& icon, auto&& func)
	{
		const Button button;
		button.Padding({});
		button.Height(32);
		button.Width(32);
		button.Content(icon);
		button.Click(func);
		return button;
	}

	Border DesignGame::CreateCell(uint8_t const& x, uint8_t const& y, bool const& v)
	{
		const Border border = CommonBorder(x, y);
		border.BorderThickness({ 1, 1, 1, 1 });
		border.BorderBrush(ControlBorder());
		border.Background(v ? AccentFill() : DefaultFill());
		AllSkip(border);
		return cells[x][y] = border;
	}

	void DesignGame::ResetCell(Border const& border, uint8_t const& x, uint8_t const& y)
	{
		Grid::SetRow(border, x);
		Grid::SetColumn(border, y);
		cells[x][y] = border;
	}

	Button DesignGame::CreateRemoveRow(uint8_t const& row)
	{
		return AddRemove(RemoveIcon(), [this, row](IInspectable const&, RoutedEventArgs const&)
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
					auto& cells_i = cells[i];
					for (uint8_t j{}; j < width; ++j)
						ResetCell(cells_i[j], i - 1, j);
				}
				cells.pop_back();
				--height;
				const UIElementCollection children = baby().Children();
				children.Clear();
				DispatcherQueue::GetForCurrentThread().TryEnqueue([this, children]()
					{
						for (auto const& i : cells)
							for (Border const& j : i)
								children.Append(j);
					});
			});
	}

	Button DesignGame::CreateRemoveColumn(uint8_t const& column)
	{
		return AddRemove(RemoveIcon(), [this, column](IInspectable const&, RoutedEventArgs const&)
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
					auto& buttonsi = cells[i];
					for (uint8_t j = column + 1; j < width; ++j)
						ResetCell(buttonsi[j], i, j - 1);
					buttonsi.pop_back();
				}
				--width;
				const UIElementCollection children = baby().Children();
				children.Clear();
				DispatcherQueue::GetForCurrentThread().TryEnqueue([this, children]()
					{
						for (auto const& i : cells)
							for (Border const& j : i)
								children.Append(j);
					});
			});
	}

	Button DesignGame::CreateAddRow()
	{
		return AddRemove(AddIcon(), [this](IInspectable const&, RoutedEventArgs const&)
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
					cells.push_back(vector<Border>(width, nullptr));
					for (uint8_t j{}; j < width; ++j)
					{
						children.Append(CreateCell(height, j));
						current.Append(false);
					}
					++height;
				}
			});
	}

	Button DesignGame::CreateAddColumn()
	{
		return AddRemove(AddIcon(), [this](IInspectable const&, RoutedEventArgs const&)
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
						cells[i].push_back(nullptr);
						children.Append(CreateCell(i, width));
						board.GetAt(i).Append(false);
					}
					++width;
				}
			});
	}

	SEllipse DesignGame::CreateVertex(IInspectable const& p)
	{
		const SEllipse ellipse = CommonEllipse(p);
		AllSkip(ellipse);
		ellipse.RightTapped([this, p, ellipse](IInspectable const&, RightTappedRoutedEventArgs const&)
			{
				{
					const UIElementCollection children = draw().Children();
					g.ForEachNeighbor(p, [this, &children](IInspectable const& e, IInspectable const&)
						{
							uint32_t index;
							children.IndexOf(edges[e], index);
							children.RemoveAt(index);
						});
					uint32_t index;
					children.IndexOf(ellipse, index);
					children.RemoveAt(index);
				}
				g.EraseVertex(p);
			});
		return vertices[p] = ellipse;
	}

	SPolyline DesignGame::CreateEdge(IVector<Point> const& p, IInspectable const& u, IInspectable const& v)
	{
		const SPolyline pl = CommonLine(p, u, v);
		AllSkip(pl);
		pl.RightTapped([this, p, u, v, pl](IInspectable const&, RightTappedRoutedEventArgs const&)
			{
				{
					const UIElementCollection children = draw().Children();
					uint32_t index;
					children.IndexOf(pl, index);
					children.RemoveAt(index);
				}
				g.EraseEdge(p, u, v);
			});
		return edges[p] = pl;
	}
}
