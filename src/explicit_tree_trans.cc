/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Source file for an explicit tree automaton.
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>
#include <vata/explicit_tree_trans.hh>
#include <vata/explicit_tree_aut.hh>

#include "explicit_tree_trans_core.hh"
#include "loadable_aut.hh"

using VATA::AutBase;
using VATA::ExplicitTreeTrans;
using VATA::Util::AutDescription;


ExplicitTreeTrans::ExplicitTreeTrans() :
	core_(new CoreTrans(CoreTrans::ParentAut()))
{ }


ExplicitTreeTrans::ExplicitTreeTrans(
	CoreTrans&&                    core) :
	core_(new CoreTrans(std::move(core)))
{ }


ExplicitTreeTrans::ExplicitTreeTrans(
	const ExplicitTreeTrans&         aut,
	bool                           copyTrans,
	bool                           copyFinal) :
	core_(new CoreTrans(*aut.core_, copyTrans, copyFinal))
{ }


ExplicitTreeTrans::~ExplicitTreeTrans()
{ }


void ExplicitTreeTrans::SetAlphabet(AlphabetType& alphabet)
{
	assert(nullptr != core_);

	core_->SetAlphabet(alphabet);
}


ExplicitTreeTrans::AlphabetType& ExplicitTreeTrans::GetAlphabet()
{
	assert(nullptr != core_);

	return core_->GetAlphabet();
}


const ExplicitTreeTrans::AlphabetType& ExplicitTreeTrans::GetAlphabet() const
{
	assert(nullptr != core_);

	return core_->GetAlphabet();
}


void ExplicitTreeTrans::LoadFromString(
	VATA::Parsing::AbstrParser&       parser,
	const std::string&                str,
	const std::string&                params)
{
	assert(nullptr != core_);

	core_->LoadFromString(parser, str, params);
}


void ExplicitTreeTrans::LoadFromString(
	VATA::Parsing::AbstrParser&       parser,
	const std::string&                str,
	StringToStateTranslWeak&          stateTransl,
	const std::string&                params)
{
	assert(nullptr != core_);

	core_->LoadFromString(parser, str, stateTransl, params);
}


void ExplicitTreeTrans::LoadFromString(
	VATA::Parsing::AbstrParser&      parser,
	const std::string&               str,
	StateDict&                       stateDict,
	const std::string&               params)
{
	assert(nullptr != core_);

	core_->LoadFromString(parser, str, stateDict, params);
}


void ExplicitTreeTrans::LoadFromAutDesc(
	const VATA::Util::AutDescription&   desc,
	StateDict&                          stateDict,
	const std::string&                  params)
{
	assert(nullptr != core_);

	core_->LoadFromAutDesc(desc, stateDict, params);
}


std::string ExplicitTreeTrans::DumpToString(
	VATA::Serialization::AbstrSerializer&     serializer,
	const std::string&                        params) const
{
	assert(nullptr != core_);

	return core_->DumpToString(serializer, params);
}


std::string ExplicitTreeTrans::DumpToString(
	VATA::Serialization::AbstrSerializer&     serializer,
	const StateDict&                          stateDict,
	const std::string&                        params) const
{
	assert(nullptr != core_);

	return core_->DumpToString(serializer, stateDict, params);
}


std::string ExplicitTreeTrans::DumpToString(
	VATA::Serialization::AbstrSerializer&  serializer,
	const StateBackTranslStrict&           stateTransl,
	const std::string&                     params) const
{
	assert(nullptr != core_);

	return core_->DumpToString(serializer, stateTransl, params);
}


AutDescription ExplicitTreeTrans::DumpToAutDesc(
	const std::string&                        params) const
{
	assert(nullptr != core_);

	return core_->DumpToAutDesc(params);
}


AutDescription ExplicitTreeTrans::DumpToAutDesc(
	const StateDict&                          stateDict,
	const std::string&                        params) const
{
	assert(nullptr != core_);

	return core_->DumpToAutDesc(stateDict, params);
}


AutDescription ExplicitTreeTrans::DumpToAutDesc(
	const StateBackTranslStrict&           stateTransl,
	const std::string&                     params) const
{
	assert(nullptr != core_);

	return core_->DumpToAutDesc(stateTransl, params);
}


ExplicitTreeTrans ExplicitTreeTrans::Union(
	const ExplicitTreeTrans&                lhs,
	const ExplicitTreeTrans&                rhs,
	AutBase::StateToStateMap*             pTranslMapLhs,
	AutBase::StateToStateMap*             pTranslMapRhs)
{
	assert(nullptr != lhs.core_);
	assert(nullptr != rhs.core_);

	return ExplicitTreeTrans(
		CoreTrans::Union(*lhs.core_, *rhs.core_, pTranslMapLhs, pTranslMapRhs));
}


ExplicitTreeTrans ExplicitTreeTrans::UnionDisjointStates(
	const ExplicitTreeTrans&                lhs,
	const ExplicitTreeTrans&                rhs)
{
	assert(nullptr != lhs.core_);
	assert(nullptr != rhs.core_);

	return ExplicitTreeTrans(
		CoreTrans::UnionDisjointStates(*lhs.core_, *rhs.core_));
}

VATA::ExplicitTreeAut ExplicitTreeTrans::Apply(VATA::ExplicitTreeAut& aut,
						bool                       backward,
						VATA::AutBase::StateDict*  stateDictApply,
						VATA::AutBase::StateDict*  stateDictTrans,
						VATA::AutBase::StateDict*  stateDictAut)
{
	return VATA::ExplicitTreeAut(core_->Apply(aut.GetCore(), backward, stateDictApply, stateDictTrans, stateDictAut));

}


