#pragma once

#include "ReplayGame.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct ReplayGame : ReplayGameT<ReplayGame>
	{
		void Init(IVector<uint8_t> const& _content);
		[[nodiscard]] DispatcherTimer Timer() const;

		void Resume(IInspectable const&, RoutedEventArgs const&);
		void Pause(IInspectable const&, RoutedEventArgs const&);
		void Previous(IInspectable const&, RoutedEventArgs const&);
		void Next(IInspectable const&, RoutedEventArgs const&);
		void ChooseSpeed(IInspectable const& sender, SelectionChangedEventArgs const&);

	private:
		double speed = 1.0;
		uint8_t height, width, ex, ey;
		uint16_t num;
		uint32_t read_pos = 3;
		IVector<IVector<bool>> board = single_threaded_vector<IVector<bool>>();
		vector<vector<uint16_t>> numbers;
		vector<vector<Button>> buttons;
		IVector<uint8_t> content;
		DispatcherTimer timer;
		stack<pair<uint8_t, uint8_t>> empties;

		void ResetButton(Button const& button, uint8_t const& x, uint8_t const& y);
		void Move(uint8_t const& x, uint8_t const& y);
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct ReplayGame : ReplayGameT<ReplayGame, implementation::ReplayGame>
	{
	};
}
