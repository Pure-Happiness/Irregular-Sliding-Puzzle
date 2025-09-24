#pragma once

#include "DesignGame.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct DesignGame : DesignGameT<DesignGame>
	{
		void Init();
		void Init(uint8_t const& _height, uint8_t const& _width, IVector<IVector<bool>> const& _board);

		void UpdateSize(IInspectable const&, SizeChangedEventArgs const& e);
		void DragStart(IInspectable const&, PointerRoutedEventArgs const& e);
		void DragEnd(IInspectable const&, PointerRoutedEventArgs const& e);
		void AsWrite(IInspectable const&, TappedRoutedEventArgs const&);
		void AsReverse(IInspectable const&, TappedRoutedEventArgs const&);
		void AsErase(IInspectable const&, TappedRoutedEventArgs const&);
		void Set(IInspectable const&, RoutedEventArgs const&) const;
		void Help(IInspectable const&, RoutedEventArgs const&) const;
		void StartGame(IInspectable const&, RoutedEventArgs const&) const;
		fire_and_forget DisplayRecords(IInspectable const&, RoutedEventArgs const&);

	private:
		uint8_t height = 16, width = 16;
		IVector<IVector<bool>> board;
		vector<vector<Border>> buttons;
		enum :uint8_t { Reverse, Write, Erase } status;
		Point start;
		bool dragging, loaded;

		Border CreateButton(uint8_t const& x, uint8_t const& y, bool const& v = false);
		void ResetButton(Border const& button, uint8_t const& x, uint8_t const& y);
		Button CreateRemoveRow(uint8_t const& row);
		Button CreateRemoveColumn(uint8_t const& column);
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
