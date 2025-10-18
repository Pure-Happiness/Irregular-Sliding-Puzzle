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

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage::Streams;
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
using SEllipse = Shapes::Ellipse;
using GraphP = Irregular_Sliding_Puzzle::Graph;

inline TitleBar title_bar = nullptr;
inline Irregular_Sliding_Puzzle::ReplayGame alive = nullptr;
inline uint8_t o_height, o_width;
inline IVector<IVector<bool>> o_board;

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

inline Border CreateGround(uint8_t const& x, uint8_t const& y)
{
	const Border border;
	Grid::SetRow(border, x);
	Grid::SetColumn(border, y);
	border.Background(SolidFill());
	border.Height(32);
	border.Width(32);
	return border;
}

inline Border CreateWall(uint8_t const& x, uint8_t const& y)
{
	const Border border;
	Grid::SetRow(border, x);
	Grid::SetColumn(border, y);
	border.Height(32);
	border.Width(32);
	return border;
}

auto SillyPairAssign(auto& a, auto& b)
{
	return [&a, &b](IInspectable const& x, IInspectable const& y) { a = x, b = y; };
}
