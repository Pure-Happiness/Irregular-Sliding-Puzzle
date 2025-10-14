#pragma once

#include "Graph.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct Graph : GraphT<Graph>
	{
		void AddVertex(IInspectable const& v);
		void EraseVertex(IInspectable const& v);
		void AddEdge(IInspectable const& e, IInspectable const& u, IInspectable const& v);
		void EraseEdge(IInspectable const& e);
		void GetEdge(IInspectable const& e, SillyPair const& p) const;
		void forEachVertex(VV const& func) const;
		bool forEachVertex2(VB const& func) const;
		void forEachEdge(EV const& func) const;
		bool forEachEdge2(EB const& func) const;
		void ForEachNeighbor(IInspectable const& v, NV const& func) const;
		bool ForEachNeighbor2(IInspectable const& v, NB const& func) const;
		[[nodiscard]] uint32_t VertexCount() const;
		[[nodiscard]] uint32_t EdgeCount() const;

	private:
		map<IInspectable, pair<IInspectable, IInspectable>> edges;
		map<IInspectable, map<IInspectable, IInspectable>> adj;
	};
}

namespace winrt::Irregular_Sliding_Puzzle::factory_implementation
{
	struct Graph : GraphT<Graph, implementation::Graph>
	{
	};
}
