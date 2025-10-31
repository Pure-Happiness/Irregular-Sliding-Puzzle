#pragma once

#include "Graph.g.h"

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	struct Graph : GraphT<Graph>
	{
		void AddVertex(IInspectable const& v);
		void EraseVertex(IInspectable const& v);
		void AddEdge(IInspectable const& e, IInspectable const& u, IInspectable const& v);
		void EraseEdge(IInspectable const& e, IInspectable const& u, IInspectable const& v);
		void ForEachVertex(VV const& func) const;
		[[nodiscard]] bool ForEachVertex2(VB const& func) const;
		void ForEachEdge(EV const& func) const;
		[[nodiscard]] bool ForEachEdge2(EB const& func) const;
		void ForEachNeighbor(IInspectable const& v, NV const& func) const;
		[[nodiscard]] bool ForEachNeighbor2(IInspectable const& v, NB const& func) const;
		[[nodiscard]] uint32_t VertexCount() const;
		[[nodiscard]] uint32_t EdgeCount() const;
		[[nodiscard]] uint32_t Degree(IInspectable const& v) const;
		void ClearEdges();
		void Clear();

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
