#pragma once

#include "PlayGame.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct PlayGame : PlayGameT<PlayGame>
	{
		void Init(uint32_t const& _height, uint32_t const& _width, IVector<IVector<bool>> const& _board);

		void Pause(IInspectable const&, RoutedEventArgs const&);
		void Resume(IInspectable const&, RoutedEventArgs const&);
		void Surrender(IInspectable const&, RoutedEventArgs const&) const;

	private:
		uint32_t height, width, ex, ey, num, time;
		IVector<IVector<bool>> board;
		vector<vector<uint32_t>> numbers;
		vector<vector<Button>> buttons;
		DispatcherQueueTimer timer = DispatcherQueue::GetForCurrentThread().CreateTimer();

		static Border CreateGround(uint32_t const& x, uint32_t const& y);
		static Border CreateWall(uint32_t const& x, uint32_t const& y);
		Button CreateButton(uint32_t const& x, uint32_t const& y, uint32_t const& n);
		void ResetButton(Button const& button, uint32_t const& x, uint32_t const& y);
		[[nodiscard]] bool Complete() const;
		void Move(uint32_t const& x, uint32_t const& y);
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct PlayGame : PlayGameT<PlayGame, implementation::PlayGame>
	{
	};
}
