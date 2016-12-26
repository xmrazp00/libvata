/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c)
 *
 *  Description:
 *    Implementation of union on explicit tree transducers.
 *
 *****************************************************************************/

#include "explicit_tree_trans_core.hh"

using VATA::ExplicitTreeTransCore;

ExplicitTreeTransCore ExplicitTreeTransCore::Union(
	const ExplicitTreeTransCore&            lhs,
	const ExplicitTreeTransCore&            rhs,
	AutBase::StateToStateMap*             pTranslMapLhs,
	AutBase::StateToStateMap*             pTranslMapRhs)
{
	StateToStateMap translMapLhs;
	StateToStateMap translMapRhs;

	if (!pTranslMapLhs)
	{
		pTranslMapLhs = &translMapLhs;
	}

	if (!pTranslMapRhs)
	{
		pTranslMapRhs = &translMapRhs;
	}
	
    StateType stateCnt = 0;
	auto translFunc = [&stateCnt](const StateType&){return stateCnt++;};

	StateToStateTranslWeak stateTransLhs(*pTranslMapLhs, translFunc);
	StateToStateTranslWeak stateTransRhs(*pTranslMapRhs, translFunc);

	ExplicitTreeTransCore res(lhs.cache_);

	lhs.ReindexStates(res, stateTransLhs);
	rhs.ReindexStates(res, stateTransRhs);

	return res;
}


ExplicitTreeTransCore ExplicitTreeTransCore::UnionDisjointStates(
	const ExplicitTreeTransCore&        lhs,
	const ExplicitTreeTransCore&        rhs)
{
    ExplicitTreeTransCore res(lhs);

	assert(rhs.transitions_);

	res.uniqueClusterMap()->insert(rhs.transitions_->begin(), rhs.transitions_->end());
	assert(lhs.transitions_->size() + rhs.transitions_->size() == res.transitions_->size());

	res.finalStates_.insert(rhs.finalStates_.begin(), rhs.finalStates_.end());
	assert(lhs.finalStates_.size() + rhs.finalStates_.size() == res.finalStates_.size());

	return res;
}
