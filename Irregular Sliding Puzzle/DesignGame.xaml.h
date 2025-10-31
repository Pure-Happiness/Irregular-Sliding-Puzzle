#pragma once

#include "DesignGame.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct DesignGame : DesignGameT<DesignGame>
	{
		void Init();
		void Init(uint8_t const& _height, uint8_t const& _width, IVector<IVector<bool>> const& _board);
		void Init(GraphP const& _g);
		void AsGraphMode();

		void DragStart(IInspectable const&, PointerRoutedEventArgs const& e);
		void DragEnd(IInspectable const&, PointerRoutedEventArgs const& e);
		void DrawPressed(IInspectable const&, PointerRoutedEventArgs const& e);
		void DrawMoved(IInspectable const&, PointerRoutedEventArgs const& e);
		void DrawReleased(IInspectable const&, PointerRoutedEventArgs const& e);
		void ToGraphMode(IInspectable const&, RoutedEventArgs const&);
		void ToGridMode(IInspectable const&, RoutedEventArgs const&);
		void AsWrite(IInspectable const&, TappedRoutedEventArgs const&);
		void AddVertices(IInspectable const&, TappedRoutedEventArgs const&);
		void AsReverse(IInspectable const&, TappedRoutedEventArgs const&);
		void AddSegments(IInspectable const&, TappedRoutedEventArgs const&);
		void AsErase(IInspectable const&, TappedRoutedEventArgs const&);
		void AddCurves(IInspectable const&, TappedRoutedEventArgs const&);
		void Set(IInspectable const&, RoutedEventArgs const&) const;
		void Help(IInspectable const&, RoutedEventArgs const&) const;
		void StartGame(IInspectable const&, RoutedEventArgs const&) const;
		fire_and_forget DisplayRecords(IInspectable const&, RoutedEventArgs const&);

	private:
		uint8_t height = 16, width = 16;
		IVector<IVector<bool>> board;
		vector<vector<Border>> cells;
		enum :uint8_t { Reverse, Write, Erase } status;
		enum :uint8_t { Vertex, Segment, Curve } graph_status;
		Point start;
		vector<Point> intermediate;
		bool dragging, loaded, is_graph;
		GraphP g = nullptr;
		map<IInspectable, SEllipse> vertices;
		map<IInspectable, SPolyline> edges;

		static void AllSkip(UIElement const& element);
		static FontIcon RemoveIcon();
		static FontIcon AddIcon();
		static Button AddRemove(FontIcon const& icon, auto&& func);
		Border CreateCell(uint8_t const& x, uint8_t const& y, bool const& v = false);
		void ResetCell(Border const& border, uint8_t const& x, uint8_t const& y);
		Button CreateRemoveRow(uint8_t const& row);
		Button CreateRemoveColumn(uint8_t const& column);
		Button CreateAddRow();
		Button CreateAddColumn();
		SEllipse CreateVertex(IInspectable const& p);
		SPolyline CreateEdge(IVector<Point> const& p, IInspectable const& u, IInspectable const& v);
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct DesignGame : DesignGameT<DesignGame, implementation::DesignGame>
	{
	};
}
