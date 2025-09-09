#pragma once

#include "DesignGame.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct DesignGame : DesignGameT<DesignGame>
	{
		void Init();
		void Init(uint32_t const& _height, uint32_t const& _width, IVector<IVector<bool>> const& _board);

		void UpdateSize(IInspectable const&, SizeChangedEventArgs const& e);
		void DragStart(IInspectable const&, PointerRoutedEventArgs const& e);
		void DragEnd(IInspectable const&, PointerRoutedEventArgs const& e);
		void AsWrite(IInspectable const&, TappedRoutedEventArgs const&);
		void AsReverse(IInspectable const&, TappedRoutedEventArgs const&);
		void AsErase(IInspectable const&, TappedRoutedEventArgs const&);
		void StartGame(IInspectable const&, RoutedEventArgs const&) const;

	private:
		uint32_t height = 16, width = 16;
		IVector<IVector<bool>> board = single_threaded_vector<IVector<bool>>();
		vector<vector<Border>> buttons;
		enum :int8_t { Reverse, Write, Erase } status;
		Point start;
		bool dragging;

		Border CreateButton(uint32_t const& x, uint32_t const& y, bool const& v = false);
		void ResetButton(Border const& button, uint32_t const& x, uint32_t const& y);
		Button CreateRemoveRow(uint32_t const& row);
		Button CreateRemoveColumn(uint32_t const& column);
		Button CreateAddRow();
		Button CreateAddColumn();
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct DesignGame : DesignGameT<DesignGame, implementation::DesignGame>
	{
	};
}
