#include "pch.h"
#include "Graph.h"
#if __has_include("Graph.g.cpp")
#include "Graph.g.cpp"
#endif

namespace winrt::Irregular_Sliding_Puzzle::implementation
{
	void Graph::AddVertex(IInspectable const& v)
	{
		adj[v] = {};
	}

	void Graph::EraseVertex(IInspectable const& v)
	{
		for (auto const& [e, u] : adj.at(v))
		{
			adj.at(u).erase(e);
			edges.erase(e);
		}
		adj.erase(v);
	}

	void Graph::AddEdge(IInspectable const& e, IInspectable const& u, IInspectable const& v)
	{
		adj.at(u)[e] = v, adj.at(v)[e] = u;
		edges[e] = { u, v };
	}

	void Graph::EraseEdge(IInspectable const& e)
	{
		auto const& [u, v] = edges.at(e);
		adj.at(u).erase(e), adj.at(v).erase(e);
		edges.erase(e);
	}

	void Graph::GetEdge(IInspectable const& e, SillyPair const& p) const
	{
		auto const& [u, v] = edges.at(e);
		p(u, v);
	}

	void Graph::forEachVertex(VV const& func) const
	{
		for (auto const& v : adj | views::keys)
			func(v);
	}

	bool Graph::forEachVertex2(VB const& func) const
	{
		for (auto const& v : adj | views::keys)
			if (!func(v))
				return false;
		return true;
	}

	void Graph::forEachEdge(EV const& func) const
	{
		for (auto const& e : edges | views::keys)
			func(e);
	}

	bool Graph::forEachEdge2(EB const& func) const
	{
		for (auto const& e : edges | views::keys)
			if (!func(e))
				return false;
		return true;
	}

	void Graph::ForEachNeighbor(IInspectable const& v, NV const& func) const
	{
		for (auto const& [e, u] : adj.at(v))
			func(e, u);
	}

	bool Graph::ForEachNeighbor2(IInspectable const& v, NB const& func) const
	{
		for (auto const& [e, u] : adj.at(v))
			if (!func(e, u))
				return false;
		return true;
	}

	uint32_t Graph::VertexCount() const
	{
		return adj.size();
	}

	uint32_t Graph::EdgeCount() const
	{
		return edges.size();
	}
}
