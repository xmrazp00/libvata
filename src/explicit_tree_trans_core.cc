/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Implementation of the core of an explicitly represented tree automaton.
 *
 *****************************************************************************/


#include "explicit_tree_trans_core.hh"
#include "explicit_tree_sim.hh"
#include "explicit_tree_transl.hh"
#include "explicit_tree_unreach.hh"
#include "loadable_aut.hh"


using VATA::AutBase;
using VATA::ExplicitTreeTransCore;
using VATA::Util::Convert;

using namespace VATA::ExplicitTreeTransCoreUtil;


// global tuple cache definition
ExplicitTreeTransCore::TupleCache ExplicitTreeTransCore::globalTupleCache_;

// global alphabet
ExplicitTreeTransCore::AlphabetType ExplicitTreeTransCore::globalAlphabet_ =
	AlphabetType(new ExplicitTreeAut::OnTheFlyAlphabet);

ExplicitTreeTransCore::TransDict ExplicitTreeTransCore::globalTransDict_ = 
    ExplicitTreeTransCore::TransDict();


BaseTransIterator::BaseTransIterator(
	const ExplicitTreeTransCore&        aut) :
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

ExplicitTreeTransCore::ExplicitTreeTransCore(
	TupleCache&          tupleCache,
	AlphabetType&        alphabet,
    TransDict&           transDict) :                                                   //Transducers debug !
	cache_(tupleCache),
	finalStates_(),
	transitions_(StateToTransitionClusterMapPtr(new StateToTransitionClusterMap())),
	alphabet_(alphabet),
    transDict_(transDict)
{ }


ExplicitTreeTransCore::ExplicitTreeTransCore(
	const ExplicitTreeTransCore&    aut,
	bool                          copyTrans,
	bool                          copyFinal) :
	cache_(aut.cache_),
	finalStates_(),
	transitions_(),
	alphabet_(aut.alphabet_),
    transDict_(aut.transDict_)
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


ExplicitTreeTransCore::ExplicitTreeTransCore(
	ExplicitTreeTransCore&&         aut) :
	cache_(aut.cache_),
	finalStates_(std::move(aut.finalStates_)),
	transitions_(std::move(aut.transitions_)),
	alphabet_(std::move(aut.alphabet_)),
    transDict_(aut.transDict_)
{ }
