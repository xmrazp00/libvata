/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Implementation of the core of an explicitly represented tree automaton.
 *
 *****************************************************************************/


#include "explicit_tree_aut_core.hh"
#include "explicit_tree_sim.hh"
#include "explicit_tree_transl.hh"
#include "explicit_tree_unreach.hh"
#include "loadable_aut.hh"


using VATA::AutBase;
using VATA::ExplicitTreeAutCore;
using VATA::Util::Convert;

using namespace VATA::ExplicitTreeAutCoreUtil;


// global tuple cache definition
ExplicitTreeAutCore::TupleCache ExplicitTreeAutCore::globalTupleCache_;

// global alphabet
ExplicitTreeAutCore::AlphabetType ExplicitTreeAutCore::globalAlphabet_ =
	AlphabetType(new ExplicitTreeAut::OnTheFlyAlphabet);

/*
std::unordered_set<size_t> ExplicitTreeAutCore::GetUsedStates() const
{
	std::unordered_set<size_t> res;
	for (auto trans : *this)
	{
		for (auto state : trans.GetChildren())
		{
 			res.insert(state);
		}
		res.insert(trans.GetParent());
	}

	for (auto state : GetFinalStates())
	{
		res.insert(state);
	}

	return res;
}
*/

BaseTransIterator::BaseTransIterator(
	const ExplicitTreeAutCore&        aut) :
	aut_(aut),
	stateClusterIterator_(aut.transitions_->begin()),
	symbolSetIterator_(),
	tupleIterator_()
{
	if (aut.transitions_->end() == stateClusterIterator_)
	{
		return;
	}

	symbolSetIterator_ = stateClusterIterator_->second->begin();
	assert(stateClusterIterator_->second->end() != symbolSetIterator_);

	tupleIterator_ = symbolSetIterator_->second->begin();
	assert(symbolSetIterator_->second->end() != tupleIterator_);
}


Iterator& Iterator::operator++()
{
	if (symbolSetIterator_->second->end() != ++tupleIterator_)
	{
		return *this;
	}

	if (stateClusterIterator_->second->end() != ++symbolSetIterator_)
	{
		tupleIterator_ = symbolSetIterator_->second->begin();
		return *this;
	}

	if (aut_.transitions_->end() != ++stateClusterIterator_)
	{
		symbolSetIterator_ = stateClusterIterator_->second->begin();
		tupleIterator_ = symbolSetIterator_->second->begin();
		return *this;
	}

	tupleIterator_ = TuplePtrSet::const_iterator();

	return *this;
}
/*
DownAccessor::DownAccessor(
	const ExplicitTreeAutCore&     aut,
	size_t                         state) :
	state_(state),
	cluster_(ExplicitTreeAutCore::genericLookup(*aut.transitions_, state))
{ }

Transition DownAccessorIterator::getTrans() const
{
	assert(*tupleIterator_);

	return Transition(
		accessor_.state_,
		symbolSetIterator_->first,
		**tupleIterator_
	);
}

DownAccessorIterator::DownAccessorIterator(
		const DownAccessor&           accessor) :
		accessor_(accessor),
		symbolSetIterator_(),
		tupleIterator_()
{
	if (nullptr == accessor_.cluster_)
	{
		return;
	}

	symbolSetIterator_ = accessor_.cluster_->begin();
	assert(symbolSetIterator_ != accessor_.cluster_->end());

	tupleIterator_ = symbolSetIterator_->second->begin();
	assert(tupleIterator_ != symbolSetIterator_->second->end());
}

DownAccessorIterator& DownAccessorIterator::operator++()
{
	if (++tupleIterator_ != symbolSetIterator_->second->end())
	{
		return *this;
	}

	if (++symbolSetIterator_ != accessor_.cluster_->end())
	{
		tupleIterator_ = symbolSetIterator_->second->begin();
		return *this;
	}

	tupleIterator_ = TuplePtrSet::const_iterator();
	return *this;
}


AcceptTransIterator::AcceptTransIterator(
	int                               ,
	const ExplicitTreeAutCore&       aut) :
	BaseTransIterator(0, aut),
	stateSetIterator_()
{ }

AcceptTransIterator::AcceptTransIterator(
	const ExplicitTreeAutCore&       aut) :
	BaseTransIterator(aut),
	stateSetIterator_(aut.finalStates_.begin())
{
	this->init();
}
*/

void AcceptTransIterator::init()
{
	for (; stateSetIterator_ != aut_.finalStates_.end(); ++stateSetIterator_)
	{	// we try to find the first final state that has an outgoing transition
		stateClusterIterator_ = aut_.transitions_->find(*stateSetIterator_);

		if (stateClusterIterator_ != aut_.transitions_->end())
		{
			break;
		}
	}

	if (stateSetIterator_ == aut_.finalStates_.end())
	{	// in case no final state has an outgoing transition
		tupleIterator_ = TuplePtrSet::const_iterator();
		return;
	}

	symbolSetIterator_ = stateClusterIterator_->second->begin();
	tupleIterator_ = symbolSetIterator_->second->begin();
}

/*
AcceptTransIterator& AcceptTransIterator::operator++()
{
	if (++tupleIterator_ != symbolSetIterator_->second->end())
	{
		return *this;
	}

	if (++symbolSetIterator_ != stateClusterIterator_->second->end())
	{
		tupleIterator_ = symbolSetIterator_->second->begin();
		return *this;
	}

	++stateSetIterator_;

	this->init();
	return *this;
}
*/

ExplicitTreeAutCore::ExplicitTreeAutCore(
	TupleCache&          tupleCache,
	AlphabetType&        alphabet) :
	cache_(tupleCache),
	finalStates_(),
	transitions_(StateToTransitionClusterMapPtr(new StateToTransitionClusterMap())),
	alphabet_(alphabet)
{ }


ExplicitTreeAutCore::ExplicitTreeAutCore(
	const ExplicitTreeAutCore&    aut,
	bool                          copyTrans,
	bool                          copyFinal) :
	cache_(aut.cache_),
	finalStates_(),
	transitions_(),
	alphabet_(aut.alphabet_)
{
	if (copyTrans)
	{
		transitions_ = aut.transitions_;
	}
	else
	{
		transitions_ = StateToTransitionClusterMapPtr(new StateToTransitionClusterMap());
	}

	if (copyFinal)
	{
		finalStates_ = aut.finalStates_;
	}
}


ExplicitTreeAutCore::ExplicitTreeAutCore(
	ExplicitTreeAutCore&&         aut) :
	cache_(aut.cache_),
	finalStates_(std::move(aut.finalStates_)),
	transitions_(std::move(aut.transitions_)),
	alphabet_(std::move(aut.alphabet_))
{ }

/*
ExplicitTreeAutCore::ExplicitTreeAutCore(
	const ExplicitTreeAutCore&    aut,
	TupleCache&                   tupleCache) :
	cache_(tupleCache),
	finalStates_(aut.finalStates_),
	transitions_(aut.transitions_),
	alphabet_(aut.alphabet_)
{ }


ExplicitTreeAutCore& ExplicitTreeAutCore::operator=(
	const ExplicitTreeAutCore&    rhs)
{
	if (this != &rhs)
	{
		finalStates_ = rhs.finalStates_;
		transitions_ = rhs.transitions_;
		alphabet_    = rhs.alphabet_;
		// NOTE: we don't care about cache_
	}

	return *this;
}


ExplicitTreeAutCore& ExplicitTreeAutCore::operator=(
	ExplicitTreeAutCore&&         rhs)
{
	assert(this != &rhs);

	finalStates_ = std::move(rhs.finalStates_);
	transitions_ = std::move(rhs.transitions_);
	alphabet_    = std::move(rhs.alphabet_);
	// NOTE: we don't care about cache_

	return *this;
}
*/

/*
ExplicitTreeAutCore ExplicitTreeAutCore::Reduce(
	const ReduceParam&            params) const
{


    
	typedef Util::TwoWayDict<
		StateType,
	 	StateType,
	 	std::unordered_map<StateType, StateType>,
	 	std::unordered_map<StateType, StateType>
	 > StateMap;
     
  
	 using StateMap = std::unordered_map<StateType, StateType>;
  
	 size_t stateCnt = 0;
  
	StateMap stateMap;
	 Util::TranslatorWeak<StateMap> stateTranslator(
	 	stateMap, [&stateCnt](const StateType&){ return stateCnt++; }
	);
  
	 this->BuildStateIndex(stateTranslator);

	SimParam simParam;
	switch (params.GetRelation())
	{
		case ReduceParam::e_reduce_relation::TA_DOWNWARD:
		{
			simParam.SetRelation(SimParam::e_sim_relation::TA_DOWNWARD);
            simParam.SetNumStates(stateCnt);
			break;
		}

		default:
		{
			assert(false);
		}
	}

	StateDiscontBinaryRelation sim = this->ComputeSimulation(simParam);

	//assert(false);

	// now we need to get an equivalence relation from the simulation

	// TODO: this is probably not optimal, we could probably get the mapping of
	// states for collapsing in a faster way
	sim.RestrictToSymmetric();       // sim is now an equivalence relation

	using StateToStateMap = std::unordered_map<StateType, StateType>;
	StateToStateMap collapseMap;
	sim.GetQuotientProjection(collapseMap);

	ExplicitTreeAutCore aut = this->CollapseStates(collapseMap);

	aut = aut.RemoveUnreachableStates();
	// TODO: we could probably refine using simulation... but this is not working now
	// aut = aut.RemoveUnreachableStates(sim, Util::TranslatorStrict<StateMap>(stateMap));

	return aut;
}
*/


/*
std::string ExplicitTreeAutCore::ToString(const Transition& trans) const
{
	std::ostringstream os;

	if (nullptr != alphabet_)
	{
		os << (*alphabet_->GetSymbolBackTransl())(trans.GetSymbol()).symbolStr;
	}
	else
	{
		os << trans.GetSymbol();
	}

	os << "(";

	for (auto it = trans.GetChildren().cbegin();
		it != trans.GetChildren().cend(); ++it)
	{
		if (it != trans.GetChildren().cbegin())
		{
			os << ", ";
		}

		os << *it;
	}

	os << ") -> ";
	os << trans.GetParent();

	return os.str();
}
*/

