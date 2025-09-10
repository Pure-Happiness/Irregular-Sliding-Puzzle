#pragma once

#include <chrono>
#include <random>
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
#include <wil/cppwinrt_helpers.h>

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Xaml::Input;
using namespace winrt::Microsoft::UI::Dispatching;
using namespace winrt::Microsoft::UI::Input;
using namespace winrt::Microsoft::Windows::ApplicationModel::Resources;

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
