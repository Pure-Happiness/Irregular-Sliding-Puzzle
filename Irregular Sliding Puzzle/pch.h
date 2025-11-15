#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <map>
#include <random>
#include <ranges>
#include <sstream>
#include <stack>
#include <utility>
#include <vector>

#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>

// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Microsoft.Windows.Globalization.h>
#include <winrt/Microsoft.Windows.Storage.h>
#include <wil/cppwinrt_helpers.h>
#include <winrt/Irregular_Sliding_Puzzle.h>

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	using namespace std;
	using namespace winrt;
	using namespace winrt::Windows::Foundation;
	using namespace winrt::Windows::Foundation::Collections;
	using namespace winrt::Windows::Storage::Streams;
	using namespace winrt::Microsoft::UI;
	using namespace winrt::Microsoft::UI::Xaml;
	using namespace winrt::Microsoft::UI::Xaml::Controls;
	using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
	using namespace winrt::Microsoft::UI::Xaml::Media;
	using namespace winrt::Microsoft::UI::Xaml::Shapes;
	using namespace winrt::Microsoft::UI::Xaml::Input;
	using namespace winrt::Microsoft::UI::Dispatching;
	using namespace winrt::Microsoft::UI::Input;
	using namespace winrt::Microsoft::Windows::ApplicationModel::Resources;
	using namespace winrt::Microsoft::Windows::Globalization;
	using namespace winrt::Microsoft::Windows::Storage;
	using Windows::Storage::FileIO;
	using Windows::Storage::StorageFile;
	using SEllipse = Ellipse;
	using SPolyline = Polyline;
	using GraphP = Graph;

	inline Border container = nullptr;
	inline TitleBar title_bar = nullptr;
	inline ReplayGame alive = nullptr;
	inline uint8_t o_height, o_width;
	inline IVector<IVector<bool>> o_board;
	inline Graph o_graph = nullptr;
	inline bool o_mode, grid_record = true, graph_record = true;

	inline Brush AccentFill()
	{
		return Application::Current().Resources().Lookup(box_value(L"AccentFillColorDefaultBrush")).as<Brush>();
	}

	inline Brush DefaultFill()
	{
		return Application::Current().Resources().Lookup(box_value(L"ControlFillColorDefaultBrush")).as<Brush>();
	}

	inline Brush SolidFill()
	{
		return Application::Current().Resources().Lookup(box_value(L"SolidBackgroundFillColorBaseBrush")).as<Brush>();
	}

	inline Brush SuccessBrush()
	{
		return Application::Current().Resources().Lookup(box_value(L"SystemFillColorSuccessBackgroundBrush")).as<Brush>();
	}

	inline Brush CriticalBrush()
	{
		return Application::Current().Resources().Lookup(box_value(L"SystemFillColorCriticalBackgroundBrush")).as<Brush>();
	}

	inline Brush ControlBorder()
	{
		return Application::Current().Resources().Lookup(box_value(L"ControlElevationBorderBrush")).as<Brush>();
	}

	inline RowDefinition AutoRow()
	{
		const RowDefinition row;
		row.Height(GridLengthHelper::Auto());
		return row;
	}

	inline ColumnDefinition AutoColumn()
	{
		const ColumnDefinition column;
		column.Width(GridLengthHelper::Auto());
		return column;
	}

	template<typename T> static T Common(uint8_t const& x, uint8_t const& y)
	{
		const T element;
		Grid::SetRow(element, x);
		Grid::SetColumn(element, y);
		element.Height(32);
		element.Width(32);
		return element;
	}

	constexpr Border(*CommonBorder)(uint8_t const&, uint8_t const&) = Common<Border>;

	inline Button CommonButton(uint8_t const& x, uint8_t const& y, uint16_t const& val)
	{
		const Button button = Common<Button>(x, y);
		button.Content(box_value(to_hstring(val)));
		return button;
	}

	inline SEllipse CommonEllipse(IInspectable const& p)
	{
		const SEllipse ellipse;
		ellipse.Height(32);
		ellipse.Width(32);
		ellipse.Fill(SolidColorBrush(Colors::SteelBlue()));
		auto const& [X, Y] = unbox_value<Point>(p);
		Canvas::SetLeft(ellipse, X - 16);
		Canvas::SetTop(ellipse, Y - 16);
		Canvas::SetZIndex(ellipse, 1);
		return ellipse;
	}

	inline SPolyline CommonLine(IVector<Point> const& p, IInspectable const& u, IInspectable const& v)
	{
		const SPolyline pl;
		pl.StrokeThickness(4);
		pl.Stroke(SolidColorBrush(Colors::SteelBlue()));
		const PointCollection collection = pl.Points();
		collection.Append(unbox_value<Point>(u));
		for (Point const& point : p)
			collection.Append(point);
		collection.Append(unbox_value<Point>(v));
		return pl;
	}

	static Border CreateGround(uint8_t const& x, uint8_t const& y)
	{
		const Border border = CommonBorder(x, y);
		border.Background(SolidFill());
		return border;
	}

	inline void RowsColumns(Grid const& grid, uint8_t const& height, uint8_t const& width)
	{
		const RowDefinitionCollection rows = grid.RowDefinitions();
		for (uint8_t i{}; i < height; ++i)
			rows.Append(AutoRow());
		const ColumnDefinitionCollection columns = grid.ColumnDefinitions();
		for (uint8_t i{}; i < width; ++i)
			columns.Append(AutoColumn());
	}

	inline void CreateBackground(Grid const& grid, uint8_t const& height, uint8_t const& width, IVector<IVector<bool>> const& board, uint16_t& num)
	{
		RowsColumns(grid, height, width);
		const UIElementCollection children = grid.Children();
		for (uint8_t i{}; i < height; ++i)
			for (uint8_t j{}; j < width; ++j)
				if (board.GetAt(i).GetAt(j))
				{
					children.Append(CreateGround(i, j));
					++num;
				}
				else
					children.Append(CommonBorder(i, j));
	}

	inline void CreateTarget(Grid const& grid, uint8_t const& height, uint8_t const& width, IVector<IVector<bool>> const& board, uint16_t const& num)
	{
		RowsColumns(grid, height, width);
		const UIElementCollection children = grid.Children();
		uint16_t now = 1;
		for (uint8_t i{}; i < height; ++i)
			for (uint8_t j{}; j < width; ++j)
				if (board.GetAt(i).GetAt(j))
				{
					const Border border = CreateGround(i, j);
					if (now < num)
					{
						border.BorderThickness({ 1, 1, 1, 1 });
						border.BorderBrush(ControlBorder());
						border.CornerRadius({ 4, 4, 4, 4 });
						const TextBlock text;
						text.Text(to_hstring(now++));
						text.VerticalAlignment(VerticalAlignment::Center);
						text.HorizontalAlignment(HorizontalAlignment::Center);
						border.Child(text);
					}
					children.Append(border);
				}
				else
					children.Append(CommonBorder(i, j));
	}

	inline void CreateBackground(Canvas const& canvas, Graph const& graph, uint16_t& num)
	{
		num = graph.VertexCount();
		const UIElementCollection children = canvas.Children();
		graph.ForEachVertex([&children](IInspectable const& v)
			{
				children.Append(CommonEllipse(v));
			});
	}

	inline void CreateTarget(Canvas const& canvas, Graph const& graph, uint16_t const& num)
	{
		const UIElementCollection children = canvas.Children();
		uint16_t now = 1;
		graph.ForEachVertex([&children, &now, &num](IInspectable const& v)
			{
				const Grid grid;
				grid.Width(32);
				grid.Height(32);
				auto const& [X, Y] = unbox_value<Point>(v);
				Canvas::SetLeft(grid, X - 16);
				Canvas::SetTop(grid, Y - 16);
				Canvas::SetZIndex(grid, 1);
				const UIElementCollection elements = grid.Children();
				{
					const SEllipse ellipse;
					ellipse.Fill(SolidColorBrush(Colors::SteelBlue()));
					ellipse.Height(32);
					ellipse.Width(32);
					elements.Append(ellipse);
				}
				if (now < num)
				{
					const TextBlock block;
					block.Text(to_hstring(now++));
					block.VerticalAlignment(VerticalAlignment::Center);
					block.HorizontalAlignment(HorizontalAlignment::Center);
					elements.Append(block);
				}
				children.Append(grid);
			});
		graph.ForEachEdge([&children](IInspectable const& e, IInspectable const& u, IInspectable const& v)
			{
				children.Append(CommonLine(e.as<IVector<Point>>(), u, v));
			});
	}

	inline void SetTimer(DispatcherTimer const& timer, uint32_t& time, AppBarButton const& button)
	{
		timer.Interval(1s);
		timer.Tick([&time, button](IInspectable const&, IInspectable const&)
			{
				++time;
				const uint32_t minutes = time / 60, seconds = time - minutes * 60;
				button.Label((minutes < 10 ? L"0" : L"") + to_hstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + to_hstring(seconds));
			});
	}

	void CommonPause(DispatcherTimer const& timer, AppBarButton const& pause, AppBarButton const& resume, auto&& board)
	{
		timer.Stop();
		pause.Visibility(Visibility::Collapsed);
		resume.Visibility(Visibility::Visible);
		board.Visibility(Visibility::Collapsed);
	}

	void CommonResume(DispatcherTimer const& timer, AppBarButton const& pause, AppBarButton const& resume, auto&& board)
	{
		pause.Visibility(Visibility::Visible);
		resume.Visibility(Visibility::Collapsed);
		board.Visibility(Visibility::Visible);
		timer.Start();
	}

	void Congratulations(uint32_t const& time, XamlRoot const& root, auto&& th)
	{
		const ContentDialog dialog;
		dialog.Title(box_value(ResourceLoader().GetString(L"Congratulations")));
		const uint32_t minutes = time / 60, seconds = time - minutes * 60;
		dialog.Content(box_value(ResourceLoader().GetString(L"Time") + (minutes < 10 ? L"0" : L"") + to_hstring(minutes) + L":" + (seconds < 10 ? L"0" : L"") + to_hstring(seconds)));
		dialog.CloseButtonText(ResourceLoader().GetString(L"Back"));
		dialog.CloseButtonClick([th](ContentDialog const&, ContentDialogButtonClickEventArgs const&)
			{
				th->GoBack();
			});
		dialog.DefaultButton(ContentDialogButton::Close);
		dialog.XamlRoot(root);
		dialog.ShowAsync();
	}

	void CommonMove(uint8_t const& x, uint8_t const& y, uint8_t const& ex, uint8_t const& ey, IVector<IVector<bool>> const& board, auto&& th)
	{
		if (x == ex)
			if (y < ey)
			{
				for (uint8_t i = y; i < ey; ++i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint8_t i = ey; i-- > y;)
					th->MoveRaw(x, i);
			}
			else
			{
				for (uint8_t i = y; i > ey; --i)
					if (!board.GetAt(x).GetAt(i))
						return;
				for (uint8_t i = ey; i++ < y;)
					th->MoveRaw(x, i);
			}
		else if (y == ey)
			if (x < ex)
			{
				for (uint8_t i = x; i < ex; ++i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint8_t i = ex; i-- > x;)
					th->MoveRaw(i, y);
			}
			else
			{
				for (uint8_t i = x; i > ex; --i)
					if (!board.GetAt(i).GetAt(y))
						return;
				for (uint8_t i = ex; i++ < x;)
					th->MoveRaw(i, y);
			}
	}

	void CommonMove(IInspectable const& u, IInspectable const& v, IInspectable const& empty, auto&& th)
	{
		if (u == empty)
			th->MoveRaw(v);
		else if (v == empty)
			th->MoveRaw(u);
	}

	void CommonMove(IInspectable const& p, Graph const& g, IInspectable const& empty, auto&& th)
	{
		g.ForEachNeighbor2(p, [&p, &empty, th](IInspectable const&, IInspectable const& v)
			{
				if (v == empty)
				{
					th->MoveRaw(p);
					return false;
				}
				return true;
			});
	}

	inline fire_and_forget WriteRecord(vector<uint8_t> const& record, bool const& write)
	{
		if (write)
			FileIO::WriteBytesAsync(co_await ApplicationData::GetDefault().LocalFolder().CreateFileAsync(to_hstring(std::time(nullptr))), record);
	}

	inline void WriteInt(vector<uint8_t>& record, uint16_t const& val)
	{
		record.push_back(val & 0xff), record.push_back(val >> 8);
	}

	inline void WriteTime(vector<uint8_t>& record, chrono::time_point<chrono::steady_clock> const& start)
	{
		const uint32_t d = duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
		record.push_back(d & 0xFF), record.push_back(d >> 8 & 0xFF), record.push_back(d >> 16 & 0xFF), record.push_back(d >> 24);
	}
}
