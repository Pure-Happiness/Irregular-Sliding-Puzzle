#pragma once

#include "PlayGame.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct PlayGame : PlayGameT<PlayGame>
	{
		bool Init(uint8_t const& _height, uint8_t const& _width, IVector<IVector<bool>> const& _board);

		void Pause(IInspectable const&, RoutedEventArgs const&);
		void Resume(IInspectable const&, RoutedEventArgs const&);
		void Surrender(IInspectable const&, RoutedEventArgs const&) const;

		void GoBack() const;
		void MoveRaw(uint8_t const& x, uint8_t const& y);

	private:
		uint8_t height, width, ex, ey;
		uint16_t num;
		uint32_t time;
		chrono::time_point<chrono::steady_clock> start_time;
		IVector<IVector<bool>> board;
		vector<vector<uint16_t>> numbers;
		vector<vector<Button>> buttons;
		DispatcherTimer timer;
		vector<uint8_t> record;

		Button CreateButton(uint8_t const& x, uint8_t const& y, uint16_t const& n);
		fire_and_forget WriteRecord() const;
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct PlayGame : PlayGameT<PlayGame, implementation::PlayGame>
	{
	};
}
