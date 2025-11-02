#pragma once

#include "ReplayGame.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct ReplayGame : ReplayGameT<ReplayGame>
	{
		void Init(IVectorView<uint8_t> const& _content);
		[[nodiscard]] DispatcherTimer Timer() const;

		void Resume(IInspectable const&, RoutedEventArgs const&);
		void Pause(IInspectable const&, RoutedEventArgs const&);
		void Previous(IInspectable const&, RoutedEventArgs const&);
		void Next(IInspectable const&, RoutedEventArgs const&);
		void ChooseSpeed(IInspectable const& sender, SelectionChangedEventArgs const&);

		void MoveRaw(uint8_t const& x, uint8_t const& y);

	private:
		double speed = 1.0;
		uint8_t height, width, ex, ey;
		uint16_t num;
		uint32_t read_pos = 3;
		IVector<IVector<bool>> board = single_threaded_vector<IVector<bool>>();
		vector<vector<Button>> buttons;
		IVectorView<uint8_t> content;
		DispatcherTimer timer;
		stack<pair<uint8_t, uint8_t>> empties;
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct ReplayGame : ReplayGameT<ReplayGame, implementation::ReplayGame>
	{
	};
}
