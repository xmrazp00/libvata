/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Implementation of Intersection() on explicit tree automata.
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>
#include <vata/util/transl_weak.hh>

// Standard library headers
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "explicit_tree_aut_core.hh"
#include "util/expl_bu_index.hh"

using VATA::ExplicitTreeAutCore;

ExplicitTreeAutCore ExplicitTreeAutCore::IntersectionBU(
	const ExplicitTreeAutCore&           lhs,
	const ExplicitTreeAutCore&           rhs,
	VATA::AutBase::ProductTranslMap*     pTranslMap)
{
	ExplicitTreeAutCore res(lhs.cache_);
	std::vector<const ProductTranslMap::value_type*> stack;

	VATA::AutBase::ProductTranslMap translMap;

	if (nullptr == pTranslMap)
	{
		pTranslMap = &translMap;
	}

	// Init auxiliary data structures
	IndexedSymbolToIndexedTransitionListMap lhsIndex, rhsIndex;
	SymbolToTransitionListMap lhsLeaves, rhsLeaves;
	size_t symbolCnt = 0;
	std::unordered_map<SymbolType, size_t> symbolMap;
	Util::TranslatorWeak2<std::unordered_map<SymbolType, size_t>>
		symbolTranslator(
			symbolMap,
			[&symbolCnt](const SymbolType& sym){ return sym; }
		);
	bottomUpIndex(
		lhs, lhsIndex, lhsLeaves, symbolTranslator
	);
	bottomUpIndex(
		rhs, rhsIndex, rhsLeaves, symbolTranslator
	);

	std::unordered_set<size_t> newStates;

	// process empty leaves
	for (size_t lhsSym = 0; lhsSym  < lhsLeaves.size(); ++lhsSym)
	{
		for (const auto& transLhs : lhsLeaves.at(lhsSym))
		{
			if (rhsLeaves.size() <= lhsSym)
			{
				continue;
			}

			const auto& parentLhs = transLhs->state();
			
			for (const auto& transRhs : rhsLeaves.at(lhsSym))
			{
				const auto& parentRhs = transRhs->state();
				const auto productState = pTranslMap->insert(
					std::make_pair(std::make_pair(parentLhs, parentRhs), pTranslMap->size())
					).first;

				if (lhs.IsStateFinal(parentLhs) && rhs.IsStateFinal(parentRhs))
				{
					res.SetStateFinal(productState->second);
				}
				res.AddTransition(std::vector<size_t>(), symbolTranslator.at(lhsSym), productState->first.second);
				stack.push_back(&*productState);
			}
		}
	}

	//const auto& transitions = res.transitions_;

	while (!stack.empty())
	{
		const auto p = stack.back();
		stack.pop_back();

		if (newStates.count(p->second))
		{
			continue;
		}
		else
		{
			newStates.insert(p->second);
		}

		const auto& leftCluster = ExplicitTreeAutCore::genericLookup(
			*lhs.transitions_, p->first.first);

		if (!leftCluster)
		{
			continue;
		}

		const auto& rightCluster = ExplicitTreeAutCore::genericLookup(
			*rhs.transitions_, p->first.second);

		if (!rightCluster)
		{
			continue;
		}

		assert(leftCluster);

		//ExplicitTreeAutCore::TransitionClusterPtr cluster(nullptr);

		const std::pair<size_t, size_t>& productState = p->first;
		if (lhsIndex.size() <= productState.first
				|| rhsIndex.size() <= productState.second)
		{
			continue;
		}

		for (size_t lhsSym = 0; lhsSym < lhsIndex.at(productState.first).size(); ++lhsSym)
		{
			for (size_t lhsStateIndex = 0;
					lhsStateIndex < lhsIndex.at(productState.first).at(lhsSym).size();
					++lhsStateIndex)
			{
				if (lhsSym >= rhsIndex.at(productState.second).size() 
						|| lhsStateIndex >= rhsIndex.at(productState.second).at(lhsSym).size())
				{ // left symbol or index of a state is not in the right TA
					continue;
				}

				const auto& rhsTransitions = rhsIndex.at(productState.second).at(
						lhsSym).at(lhsStateIndex);

				for (const auto& lhsTransPtr : lhsIndex.at(productState.first).at(lhsSym).at(lhsStateIndex))
				{
					const BUIndexTransition& lhsTrans = *lhsTransPtr;

					for (const auto& rhsTransPtr : rhsTransitions)
					{
						const BUIndexTransition& rhsTrans = *rhsTransPtr;
						
						const auto newProduct = pTranslMap->insert(
							std::make_pair(std::make_pair(lhsTrans.state(), rhsTrans.state()),
								pTranslMap->size())).first; 
						
						assert(rhsTrans.children().size() == lhsTrans.children().size());
						
						bool allTupleInProduct = true;
						std::vector<size_t> newTuple;
						for (size_t tupleIndex = 0; tupleIndex < rhsTrans.children().size(); ++tupleIndex)
						{
							const auto statePair = std::make_pair(lhsTrans.children().at(tupleIndex), rhsTrans.children().at(tupleIndex));
							const auto findResult = pTranslMap->find(statePair);
							
							if (findResult == pTranslMap->end() && lhsTrans.children().size() > 0)
							{
								allTupleInProduct = false;
							}
							else
							{
								newTuple.push_back(findResult->second);
							}
						}

						if (!allTupleInProduct)
						{
							continue;
						}

						res.AddTransition(newTuple, symbolTranslator.at(lhsSym), newProduct->second);
						stack.push_back(&*newProduct);
					}
				}

				// todo create product automata for each state of lhs with each o rhs. If product state has
				// not been added yet, so added.
				// Check if both states are already in product automaton
			}
		}
		/*
			const auto& rightTupleSet =
				ExplicitTreeAutCore::genericLookup(
					*rightCluster, leftSymbolStateTupleSetPtr.first);

			if (!rightTupleSet)
			{
				continue;
			}

			if (!cluster)
			{
				cluster = transitions->uniqueCluster(p->second);
			}

			const auto& tuplePtrSet = cluster->uniqueTuplePtrSet(leftSymbolStateTupleSetPtr.first);

			for (const auto& leftTuplePtr : *leftSymbolStateTupleSetPtr.second)
			{
				for (const auto& rightTuplePtr : *rightTupleSet)
				{
					assert(leftTuplePtr->size() == rightTuplePtr->size());

					ExplicitTreeAutCore::StateTuple children;

					for (size_t i = 0; i < leftTuplePtr->size(); ++i)
					{
						const auto u = pTranslMap->insert(
							std::make_pair(
								std::make_pair((*leftTuplePtr)[i], (*rightTuplePtr)[i]),
								pTranslMap->size()
							)
						);

						if (u.second)
						{
							stack.push_back(&*u.first);
						}

						children.push_back(u.first->second);
					}

//					res.AddTransition(children, leftSymbolStateTupleSetPtr.first, p->second);
					tuplePtrSet->insert(res.tupleLookup(children));
				}
			}
		}
			*/
	}

	return res;
}
