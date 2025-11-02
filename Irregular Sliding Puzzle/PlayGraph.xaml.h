#pragma once

#include "PlayGraph.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct PlayGraph : PlayGraphT<PlayGraph>
	{
		bool Init(GraphP const& _g);

		void Pause(IInspectable const&, RoutedEventArgs const&);
		void Resume(IInspectable const&, RoutedEventArgs const&);
		void Surrender(IInspectable const&, RoutedEventArgs const&) const;

	private:
		GraphP g;
		uint16_t num;
		uint32_t time;
		chrono::time_point<chrono::steady_clock> start_time;
		map<IInspectable, uint16_t> numbers;
		map<IInspectable, Button> buttons;
		map<Button, IInspectable> rev;
		DispatcherTimer timer;
		vector<uint8_t> record;
		IInspectable empty = nullptr;

		void GoBack() const;
		Button CreateButton(IInspectable const& p, uint16_t const& n);
		SPolyline CreateEdge(IVector<Point> const& p, IInspectable const& u, IInspectable const& v);
		void CheckComplete();
		void MoveRaw(IInspectable const& v);
		void Move(IInspectable const& u, IInspectable const& v);
		void Move(IInspectable const& p);
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct PlayGraph : PlayGraphT<PlayGraph, implementation::PlayGraph>
	{
	};
}
