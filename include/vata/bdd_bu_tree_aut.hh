/*****************************************************************************
 *  Vojnar's Army Tree Automata Library
 *
 *  Copyright (c) 2011  Ondra Lengal <ilengal@fit.vutbr.cz>
 *
 *  Description:
 *    Header file for a BDD-based bottom-up tree automaton.
 *
 *****************************************************************************/

#ifndef _VATA_BDD_BU_TREE_AUT_HH_
#define _VATA_BDD_BU_TREE_AUT_HH_

// VATA headers
#include <vata/vata.hh>
#include <vata/aut_base.hh>
#include <vata/aut_op.hh>
#include <vata/mtbdd/apply2func.hh>
#include <vata/mtbdd/ondriks_mtbdd.hh>
#include <vata/util/bdd_trans_table.hh>
#include <vata/util/ord_vector.hh>
#include <vata/util/vector_map.hh>

namespace VATA { class BDDBottomUpTreeAut; }

GCC_DIAG_OFF(effc++)
class VATA::BDDBottomUpTreeAut
	: public AutBase
{
GCC_DIAG_ON(effc++)

	template <class Aut>
	friend Aut Union(const Aut&, const Aut&, AutBase::StateToStateMap*);

	template <class Aut>
	friend Aut Intersection(const Aut&, const Aut&,
		AutBase::ProductTranslMap*);

	template <class Aut>
	friend Aut RemoveUnreachableStates(const Aut&,
		AutBase::StateToStateMap* pTranslMap);

	template <class Aut>
	friend Aut RemoveUselessStates(const Aut&,
		AutBase::StateToStateMap* pTranslMap);

	template <class Aut>
	friend bool CheckInclusion(const Aut&, const Aut&);

	template <class Aut>
	friend bool CheckInclusionNoUseless(const Aut&, const Aut&);

public:   // public data types

	typedef VATA::MTBDDPkg::VarAsgn SymbolType;

	typedef std::unordered_set<StateType> StateHT;
	typedef VATA::Util::OrdVector<StateType> StateSet;
	typedef std::vector<StateType> StateTuple;

private:  // private data types

	typedef size_t MTBDDHandle;
	typedef VATA::MTBDDPkg::OndriksMTBDD<StateSet> TransMTBDD;
	typedef VATA::MTBDDPkg::OndriksMTBDD<bool> BDD;

	typedef VATA::Util::BDDTransTable<MTBDDHandle, StateSet> TransTable;

	typedef VATA::Util::AutDescription AutDescription;

	typedef std::shared_ptr<TransTable> TransTablePtr;

	typedef VATA::Util::VectorMap<StateType, MTBDDHandle> TupleToMTBDDMap;

	typedef VATA::Util::TwoWayDict<std::string, SymbolType> StringToSymbolDict;

	typedef VATA::Util::Convert Convert;


	GCC_DIAG_OFF(effc++)    // suppress missing virtual destructor warning
	class UnionApplyFunctor :
		public VATA::MTBDDPkg::Apply2Functor<UnionApplyFunctor, StateSet,
		StateSet, StateSet>
	{
	GCC_DIAG_ON(effc++)
	public:   // methods

		inline StateSet ApplyOperation(const StateSet& lhs, const StateSet& rhs)
		{
			return lhs.Union(rhs);
		}
	};

private:  // constants

	static const size_t SYMBOL_SIZE = 16;

private:  // data members

	StateHT finalStates_;
	TransTablePtr transTable_;
	TupleToMTBDDMap mtbddMap_;
	MTBDDHandle defaultTrFuncHandle_;

	static StringToSymbolDict symbolDict_;
	static SymbolType nextSymbol_;

private:  // methods

	bool isValid() const;

	bool isStandAlone() const;

	inline bool hasEmptyStateSet() const
	{
		// Assertions
		assert(isValid());

		return mtbddMap_.empty() && finalStates_.empty();
	}

	void copyStates(const BDDBottomUpTreeAut& src);

	static SymbolType addSymbol()
	{
		return nextSymbol_++;
	}

	inline const MTBDDHandle& getMtbddHandle(const StateTuple& children) const
	{
		// Assertions
		assert(isValid());

		typename TupleToMTBDDMap::const_iterator it;
		if ((it = mtbddMap_.find(children)) == mtbddMap_.end())
		{
			return defaultTrFuncHandle_;
		}
		else
		{
			return (*it).second;
		}
	}

	inline const TransMTBDD& getMtbdd(const MTBDDHandle& handle) const
	{
		// Assertions
		assert(isValid());

		return transTable_->GetMtbdd(handle);
	}

	inline const TransMTBDD& getMtbdd(const StateTuple& children) const
	{
		// Assertions
		assert(isValid());

		return getMtbdd(getMtbddHandle(children));
	}

	inline void decrementHandleRefCnt(const StateTuple& children)
	{
		// Assertions
		assert(isValid());

		typename TupleToMTBDDMap::iterator it;
		if ((it = mtbddMap_.find(children)) != mtbddMap_.end())
		{	// in case there is something
			transTable_->DecrementHandleRefCnt((*it).second);
		}
	}

	inline void setMtbdd(const MTBDDHandle& handle, const TransMTBDD& mtbdd)
	{
		// Assertions
		assert(isValid());
		assert(transTable_->GetHandleRefCnt(handle) == 1);

		std::vector<const TransMTBDD*> vec = {&mtbdd};

		transTable_->SetMtbdd(handle, mtbdd);
	}

	inline void setMtbdd(const StateTuple& children, const TransMTBDD& mtbdd)
	{
		// Assertions
		assert(isValid());

		typename TupleToMTBDDMap::iterator it;
		if ((it = mtbddMap_.find(children)) == mtbddMap_.end())
		{	// in case the value is unknown
			MTBDDHandle handle = transTable_->AddHandle();
			setMtbdd(handle, mtbdd);
			mtbddMap_.insert(std::make_pair(children, handle));
		}
		else
		{
			MTBDDHandle& handle = (*it).second;

			if (transTable_->GetHandleRefCnt(handle) == 1)
			{	// in case there is only one reference to the MTBDD
				setMtbdd(handle, mtbdd);
			}
			else
			{
				transTable_->DecrementHandleRefCnt(handle);
				handle = transTable_->AddHandle();
				setMtbdd(handle, mtbdd);
			}
		}
	}

	template <typename T, class Container>
	inline StateType safelyTranslateToState(const T& value, Container& dict)
	{
		// Assertions
		assert(isValid());

		StateType state;
		typename Container::const_iterator itHt;
		if ((itHt = dict.find(value)) != dict.end())
		{	// in case the state is known
			state = itHt->second;
		}
		else
		{	// in case there is no translation for the state
			state = AddState();
			dict.insert(std::make_pair(value, state));
		}

		return state;
	}

	void loadFromAutDescExplicit(const AutDescription& desc,
		StringToStateDict* pStateDict);

	void loadFromAutDescSymbolic(const AutDescription& desc,
		StringToStateDict* pStateDict);

	AutDescription dumpToAutDescExplicit(
		const StringToStateDict* pStateDict) const;

	AutDescription dumpToAutDescSymbolic(
		const StringToStateDict* pStateDict) const;

	inline void deallocateTuples()
	{
		// Assertions
		assert(isValid());

		for (auto tupleHandlePair : mtbddMap_)
		{	// release all states
			transTable_->DecrementHandleRefCnt(tupleHandlePair.second);
		}

		mtbddMap_.clear();
	}

public:   // methods

	BDDBottomUpTreeAut() :
		finalStates_(),
		transTable_(new TransTable),
		mtbddMap_(),
		defaultTrFuncHandle_(transTable_->AddHandle())
	{
		// Assertions
		assert(isValid());
	}

	BDDBottomUpTreeAut(TransTablePtr transTable) :
		finalStates_(),
		transTable_(transTable),
		mtbddMap_(),
		defaultTrFuncHandle_(transTable_->AddHandle())
	{
		// Assertions
		assert(isValid());
	}

	BDDBottomUpTreeAut(const BDDBottomUpTreeAut& aut) :
		finalStates_(),
		transTable_(aut.transTable_),
		mtbddMap_(),
		defaultTrFuncHandle_(transTable_->AddHandle())
	{
		copyStates(aut);

		// Assertions
		assert(isValid());
	}

	BDDBottomUpTreeAut(BDDBottomUpTreeAut&& aut) :
		finalStates_(aut.finalStates_),
		transTable_(aut.transTable_),
		mtbddMap_(aut.mtbddMap_),
		defaultTrFuncHandle_(aut.defaultTrFuncHandle_)
	{
		aut.transTable_ = nullptr;

		// Assertions
		assert(isValid());
	}

	BDDBottomUpTreeAut& operator=(const BDDBottomUpTreeAut& rhs);

	inline TransTablePtr& GetTransTable() const
	{
		return const_cast<TransTablePtr&>(transTable_);
	}

	inline StateType AddState()
	{
		// Assertions
		assert(isValid());

		StateType newState = transTable_->AddState();

		assert(isValid());

		return newState;
	}

	void AddSimplyTransition(const StateTuple& children, SymbolType symbol,
		const StateType& parent);

	inline void SetStateFinal(const StateType& state)
	{
		// Assertions
		assert(isValid());

		finalStates_.insert(state);
	}

	inline const TupleToMTBDDMap& GetTuples() const
	{
		return mtbddMap_;
	}

	inline const StateHT& GetFinalStates() const
	{
		// Assertions
		assert(isValid());

		return finalStates_;
	}


	void LoadFromString(VATA::Parsing::AbstrParser& parser,
		const std::string& str,
		StringToStateDict* pStateDict = nullptr,
		const std::string& params = "");

	std::string DumpToString(VATA::Serialization::AbstrSerializer& serializer,
		const StringToStateDict* pStateDict = nullptr,
		const std::string& params = "") const;

	std::string DumpToDot() const
	{
		std::vector<const TransMTBDD*> tupleVec;
		for (auto tupleHandlePair : mtbddMap_)
		{
			tupleVec.push_back(&getMtbdd(tupleHandlePair.second));
		}

		return TransMTBDD::DumpToDot(tupleVec);
	}

	~BDDBottomUpTreeAut();
};

#endif
