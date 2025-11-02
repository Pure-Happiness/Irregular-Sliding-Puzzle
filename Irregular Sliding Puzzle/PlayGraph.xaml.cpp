#include "pch.h"
#include "PlayGraph.xaml.h"
#if __has_include("PlayGraph.g.cpp")
#include "PlayGraph.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	bool PlayGraph::Init(GraphP const& _g)
	{
		g = _g;
		CreateBackground(board(), g, num);
		if (!num)
			return false;
		CreateTarget(target(), g, num);
		{
			uint16_t now = 1;
			const UIElementCollection children = board().Children();
			g.ForEachVertex([this, &children, &now](IInspectable const& v)
				{
					if (now < num)
						children.Append(CreateButton(v, numbers[v] = now++));
					else
						empty = v;
				});
			g.ForEachEdge([this, &children](IInspectable const& e, IInspectable const& u, IInspectable const& v)
				{
					children.Append(CreateEdge(e.as<IVector<Point>>(), u, v));
				});
		}
		map<IInspectable, vector<IInspectable>> mp;
		if (!g.ForEachVertex2([this, &mp](IInspectable const& v)
			{
				vector<IInspectable>& mpv = mp[v];
				g.ForEachNeighbor(v, [&mpv](IInspectable const&, IInspectable const& w)
					{
						mpv.push_back(w);
					});
				return !mpv.empty();
			}))
			return false;
		mt19937 r(static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count()) ^ random_device()());
		for (uint32_t i{}; i < 0x10000; ++i)
		{
			vector<IInspectable> const& mpe = mp.at(empty);
			MoveRaw(mpe[r() % mpe.size()]);
		}
		numbers[empty] = 0;
		SetTimer(timer, time, Timer());
		timer.Start();
		start_time = chrono::steady_clock::now();
		return true;
	}

	void PlayGraph::Pause(IInspectable const&, RoutedEventArgs const&)
	{
		timer.Stop();
		pause().Visibility(Visibility::Collapsed);
		resume().Visibility(Visibility::Visible);
		board().Visibility(Visibility::Collapsed);
	}

	void PlayGraph::Resume(IInspectable const&, RoutedEventArgs const&)
	{
		pause().Visibility(Visibility::Visible);
		resume().Visibility(Visibility::Collapsed);
		board().Visibility(Visibility::Visible);
		timer.Start();
	}

	void PlayGraph::Surrender(IInspectable const&, RoutedEventArgs const&) const
	{
		timer.Stop();
		GoBack();
	}

	void PlayGraph::GoBack() const
	{
		const DesignGame content;
		content.Init(o_height, o_width, o_board, g, true);
		container.Child(content);
	}

	Button PlayGraph::CreateButton(IInspectable const& p, uint16_t const& n)
	{
		const Button button;
		const auto& [X, Y] = unbox_value<Point>(p);
		Canvas::SetLeft(button, X - 16);
		Canvas::SetTop(button, Y - 16);
		Canvas::SetZIndex(button, 2);
		button.Height(32);
		button.Width(32);
		button.CornerRadius({ 16, 16, 16, 16 });
		button.Padding({});
		button.Content(box_value(to_hstring(n)));
		button.Click([this, button](IInspectable const&, RoutedEventArgs const&)
			{
				Move(rev.at(button));
				CheckComplete();
			});
		return buttons[rev[button] = p] = button;
	}

	SPolyline PlayGraph::CreateEdge(IVector<Point> const& p, IInspectable const& u, IInspectable const& v)
	{
		const SPolyline pl = CommonLine(p, u, v);
		pl.Tapped([this, u, v](IInspectable const&, TappedRoutedEventArgs const&)
			{
				Move(u, v);
				CheckComplete();
			});
		return pl;
	}

	void PlayGraph::CheckComplete()
	{
		if (uint16_t now = 1; g.ForEachVertex2([&now, this](IInspectable const& v)
			{
				return now >= num || numbers.at(v) == now++;
			}))
		{
			timer.Stop();
			Congratulations(time, XamlRoot(), mem_fn(&PlayGraph::GoBack), this);
		}
	}

	void PlayGraph::MoveRaw(IInspectable const& v)
	{
		const Button& button = buttons.at(v);
		const auto& [X, Y] = unbox_value<Point>(empty);
		Canvas::SetLeft(button, X - 16);
		Canvas::SetTop(button, Y - 16);
		buttons[empty] = button;
		numbers[empty] = numbers.at(v);
		swap(rev.at(button), empty);
	}

	void PlayGraph::Move(IInspectable const& u, IInspectable const& v)
	{
		if (u == empty)
			MoveRaw(v);
		else if (v == empty)
			MoveRaw(u);
	}

	void PlayGraph::Move(IInspectable const& p)
	{
		g.ForEachNeighbor2(p, [this, &p](IInspectable const&, IInspectable const& v)
			{
				if (v == empty)
				{
					MoveRaw(p);
					return false;
				}
				return true;
			});
	}
}
