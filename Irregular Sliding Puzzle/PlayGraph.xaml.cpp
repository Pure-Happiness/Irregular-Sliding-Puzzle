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
		{
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
		}
		numbers[empty] = 0;
		record.push_back(0xf0);
		{
			const uint16_t vc = g.VertexCount();
			record.push_back(vc & 0xff), record.push_back(vc >> 8);
			uint16_t v_rk{};
			g.ForEachVertex([this, &v_rk](IInspectable const& v)
				{
					vertex_rk[v] = v_rk++;
					auto const& [X, Y] = unbox_value<Point>(v);
					const uint32_t x = *reinterpret_cast<const uint32_t*>(&X), y = *reinterpret_cast<const uint32_t*>(&Y);
					record.push_back(x & 0xff), record.push_back(x >> 8 & 0xff), record.push_back(x >> 16 & 0xff), record.push_back(x >> 24);
					record.push_back(y & 0xff), record.push_back(y >> 8 & 0xff), record.push_back(y >> 16 & 0xff), record.push_back(y >> 24);
				});
		}
		{
			const uint16_t ec = g.EdgeCount();
			record.push_back(ec & 0xff), record.push_back(ec >> 8);
			uint16_t e_rk{};
			g.ForEachEdge([this, &e_rk](IInspectable const& e, IInspectable const& u, IInspectable const& v)
				{
					edge_rk[e] = e_rk++;
					const uint16_t u_rk = vertex_rk.at(u), v_rk = vertex_rk.at(v);
					record.push_back(u_rk & 0xff), record.push_back(u_rk >> 8);
					record.push_back(v_rk & 0xff), record.push_back(v_rk >> 8);
				});
		}
		g.ForEachVertex([this](IInspectable const& v)
			{
				const uint16_t num = numbers.at(v);
				record.push_back(num & 0xff), record.push_back(num >> 8);
			});
		SetTimer(timer, time, Timer());
		timer.Start();
		start_time = chrono::steady_clock::now();
		return true;
	}

	void PlayGraph::Pause(IInspectable const&, RoutedEventArgs const&)
	{
		CommonPause(timer, pause(), resume(), board());
	}

	void PlayGraph::Resume(IInspectable const&, RoutedEventArgs const&)
	{
		CommonResume(timer, pause(), resume(), board());
	}

	void PlayGraph::Surrender(IInspectable const&, RoutedEventArgs const&) const
	{
		timer.Stop();
		WriteRecord(record);
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
				{
					const uint32_t d = duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count();
					record.push_back(d & 0xFF), record.push_back(d >> 8 & 0xFF), record.push_back(d >> 16 & 0xFF), record.push_back(d >> 24);
				}
				record.push_back(0);
				{
					IInspectable const& v = rev.at(button);
					const uint16_t v_rk = vertex_rk.at(v);
					record.push_back(v_rk & 0xff), record.push_back(v_rk >> 8);
					CommonMove(v, g, empty, this);
				}
				CheckComplete();
			});
		return buttons[rev[button] = p] = button;
	}

	SPolyline PlayGraph::CreateEdge(IVector<Point> const& p, IInspectable const& u, IInspectable const& v)
	{
		const SPolyline pl = CommonLine(p, u, v);
		pl.Tapped([this, p, u, v](IInspectable const&, TappedRoutedEventArgs const&)
			{
				{
					const uint32_t d = duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count();
					record.push_back(d & 0xFF), record.push_back(d >> 8 & 0xFF), record.push_back(d >> 16 & 0xFF), record.push_back(d >> 24);
				}
				record.push_back(0xff);
				{
					const uint16_t e_rk = edge_rk.at(p);
					record.push_back(e_rk & 0xff), record.push_back(e_rk >> 8);
				}
				CommonMove(u, v, empty, this);
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
			record.front() = 0xf1;
			WriteRecord(record);
			Congratulations(time, XamlRoot(), this);
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
}
