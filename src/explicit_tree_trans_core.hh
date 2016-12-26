/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Header file for the core of an explicitly represented tree automaton.
 *
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_TREE_TRANS_CORE_HH_
#define _VATA_EXPLICIT_TREE_TRANS_CORE_HH_

// VATA headers
#include <vata/aut_base.hh>
#include <vata/explicit_tree_aut.hh>
#include "explicit_tree_aut_core.hh"
#include "expl_bu_index.hh"

#include <vata/util/convert.hh>
#include <vata/util/ord_vector.hh>
#include <vata/util/transl_strict.hh>
#include <vata/util/util.hh>

#include <vata/explicit_lts.hh>
#include <vata/incl_param.hh>

#include "util/cache.hh"

namespace VATA
{
	class ExplicitTreeTransCore;

	/**
	 * @brief  Namespace with utilities for ExplicitTreeAutCore
	 *
	 * This need to be in a namespace due to the limitation of C++ which
	 * disallows forward declaration of a nested class. Therefore, in order to be
	 * able to create the ExplicitTreeAut::iterator class that references an
	 * iterator of the type ExplicitTreeAutCore::iterator, I needed to move
	 * ExplicitTreeAutCore::Iterator out of ExplicitTreeAutCore, and together
	 * with it also other classes.
	 */
	namespace ExplicitTreeTransCoreUtil
	{
		using StateType        = ExplicitTreeAut::StateType;
		using FinalStateSet    = ExplicitTreeAut::FinalStateSet;
		using SymbolType       = ExplicitTreeAut::SymbolType;
        //---------TRANSDUCER------------------------------------
        using DoubleSymbolType = std::pair<SymbolType, SymbolType>;
        using TransDict        = VATA::Util::TwoWayDict<DoubleSymbolType, SymbolType>;

        using ExplicitTreeAutCore = 
            VATA::ExplicitTreeAutCore;
        //---------TRANSDUCER------------------------------------
		using TuplePtr         = std::shared_ptr<ExplicitTreeAut::StateTuple>;
		using TuplePtrSet      = std::set<TuplePtr>;
		using TuplePtrSetPtr   = std::shared_ptr<TuplePtrSet>;
		using Transition       = ExplicitTreeAut::Transition;

		class BaseTransIterator;
		class Iterator;
		class AcceptTransIterator;
		class DownAccessorIterator;

		GCC_DIAG_OFF(effc++)
		class TransitionCluster : public std::unordered_map<
			SymbolType,
			TuplePtrSetPtr>
		{
		GCC_DIAG_ON(effc++)

		public:

			const TuplePtrSetPtr& uniqueTuplePtrSet(const SymbolType& symbol)
			{
				auto& tupleSet = this->insert(
					std::make_pair(symbol, TuplePtrSetPtr(nullptr))
				).first->second;

				if (!tupleSet)
				{
					tupleSet = TuplePtrSetPtr(new TuplePtrSet());
				}
				else if (!tupleSet.unique())
				{
					tupleSet = TuplePtrSetPtr(new TuplePtrSet(*tupleSet));
				}

				return tupleSet;
			}
            
		};

		using TransitionClusterPtr    = std::shared_ptr<TransitionCluster>;

		GCC_DIAG_OFF(effc++)
		class StateToTransitionClusterMap : public std::unordered_map<
			StateType,
			TransitionClusterPtr>
		{
		GCC_DIAG_ON(effc++)

		public:

			const TransitionClusterPtr& uniqueCluster(const StateType& state)
			{
				auto& cluster = this->insert(
					std::make_pair(state, TransitionClusterPtr(nullptr))
				).first->second;

				if (!cluster)
				{
					cluster = TransitionClusterPtr(new TransitionCluster());
				}
				else if (!cluster.unique())
				{
					cluster = TransitionClusterPtr(new TransitionCluster(*cluster));
				}

				return cluster;
			}
		};

		class AcceptTrans;
		class DownAccessor;
	}
}


/**
 * @brief  Base class for transition iterators
 */
class VATA::ExplicitTreeTransCoreUtil::BaseTransIterator
{
public:   // data types

	using iterator_category  = std::input_iterator_tag;
	using difference_type    = size_t;

	using value_type = Transition;
	using pointer    = Transition*;
	using reference  = Transition&;

protected:  // data members

	const ExplicitTreeTransCore& aut_;

	
    StateToTransitionClusterMap::const_iterator stateClusterIterator_;
	TransitionCluster::const_iterator symbolSetIterator_;
	TuplePtrSet::const_iterator tupleIterator_;

protected:// methods

	BaseTransIterator(
		int                          /* FILL (only to distinguish signature?) */,
		const ExplicitTreeTransCore&   aut) :
		aut_(aut),
		stateClusterIterator_(),
		symbolSetIterator_(),
		tupleIterator_()
	{ }

	BaseTransIterator(
		const ExplicitTreeTransCore&   aut);
/*
	BaseTransIterator(
		const BaseTransIterator&           baseTransIter) :
		aut_(baseTransIter.aut_),
		stateClusterIterator_(baseTransIter.stateClusterIterator_),
		symbolSetIterator_(baseTransIter.symbolSetIterator_),
		tupleIterator_(baseTransIter.tupleIterator_)
	{ }
*/
	Transition getTrans() const
	{
		assert(*tupleIterator_);

		return Transition(
			stateClusterIterator_->first,
			symbolSetIterator_->first,
			**tupleIterator_
		);
	}

public:   // methods

	Transition operator*() const
	{
		assert(*tupleIterator_);

		return this->getTrans();
	}

	bool operator==(const BaseTransIterator& rhs) const
	{
		return tupleIterator_ == rhs.tupleIterator_;
	}


	bool operator!=(const BaseTransIterator& rhs) const
	{
		return tupleIterator_ != rhs.tupleIterator_;
	}
};

GCC_DIAG_OFF(effc++)
class VATA::ExplicitTreeTransCoreUtil::Iterator : public BaseTransIterator
{
GCC_DIAG_ON(effc++)

public:   // methods

	Iterator(
		int                          /* FILL (only to distinguish signature?) */,
		const ExplicitTreeTransCore&   aut) :
		BaseTransIterator(0, aut)
	{ }

	explicit Iterator(
		const ExplicitTreeTransCore&   aut) :
		BaseTransIterator(aut)
	{ }
/*
	Iterator(const Iterator& iter) :
		BaseTransIterator(iter)
	{ }

*/
	Iterator& operator++();

};


GCC_DIAG_OFF(effc++)
class VATA::ExplicitTreeTransCoreUtil::AcceptTransIterator : public BaseTransIterator
{
GCC_DIAG_ON(effc++)

private:  // data members

	FinalStateSet::const_iterator stateSetIterator_;

private:  // methods

	void init();

public:   // methods
/*
	AcceptTransIterator(
		int                             ,
		const ExplicitTreeAutCore&       aut);

	explicit AcceptTransIterator(
		const ExplicitTreeAutCore&       aut);
*/
//	AcceptTransIterator& operator++();
};

class VATA::ExplicitTreeTransCoreUtil::DownAccessorIterator
{
private:  // data members
	using DownAccessor     = ExplicitTreeTransCoreUtil::DownAccessor;

	const DownAccessor& accessor_;

	TransitionCluster::const_iterator symbolSetIterator_;
	TuplePtrSet::const_iterator tupleIterator_;

private:  // methods

//	Transition getTrans() const;

public:   // methods

	DownAccessorIterator(
		int                           /* FILL */,
		const DownAccessor&           accessor) :
		accessor_(accessor),
		symbolSetIterator_(),
		tupleIterator_()
	{ }
/*
	explicit DownAccessorIterator(
		const DownAccessor&           accessor);

	DownAccessorIterator& operator++();

	bool operator==(const DownAccessorIterator& rhs) const
	{
		return tupleIterator_ == rhs.tupleIterator_;
	}

	bool operator!=(const DownAccessorIterator& rhs) const
	{
		return tupleIterator_ != rhs.tupleIterator_;
	}

	Transition operator*() const
	{
		assert(*tupleIterator_);

		return this->getTrans();
	}
*/
};


class VATA::ExplicitTreeTransCoreUtil::DownAccessor
{
private:  // data types

//	friend class DownAccessorIterator;

public:   // data types

//	using iterator        = DownAccessorIterator;
//	using const_iterator  = DownAccessorIterator;

private:  // data members

//	size_t state_;
//	const TransitionCluster* cluster_;

public:   // methods
/*
	DownAccessor(
		const ExplicitTreeAutCore&     aut,
		size_t                         state);

	DownAccessorIterator begin() const
	{
		return DownAccessorIterator(*this);
	}

	DownAccessorIterator end() const
	{
		return DownAccessorIterator(0, *this);
	}

	bool empty() const
	{
		return nullptr == cluster_;
	}
*/
};


class VATA::ExplicitTreeTransCoreUtil::AcceptTrans
{
private:  // data members

	const ExplicitTreeTransCore& aut_;

public:   // methods

	explicit AcceptTrans(const ExplicitTreeTransCore& aut) :
		aut_(aut)
	{ }

	using iterator        = AcceptTransIterator;
	using const_iterator  = AcceptTransIterator;

//	AcceptTransIterator begin() const { return AcceptTransIterator(aut_); }
//	AcceptTransIterator end() const { return AcceptTransIterator(0, aut_); }
};


GCC_DIAG_OFF(effc++)
class VATA::ExplicitTreeTransCore : public TreeAutBase
{
GCC_DIAG_ON(effc++)

	friend class ExplicitUpwardInclusion;
	friend class ExplicitDownwardComplementation;
	friend class ExplicitDownwardInclusion;

	friend class ExplicitTreeTransCoreUtil::BaseTransIterator;
	friend class ExplicitTreeTransCoreUtil::Iterator;
	friend class ExplicitTreeTransCoreUtil::AcceptTrans;
	friend class ExplicitTreeTransCoreUtil::AcceptTransIterator;
	friend class ExplicitTreeTransCoreUtil::DownAccessor;
	friend class ExplicitTreeTransCoreUtil::DownAccessorIterator;

public:   // data types

	using SymbolType       = ExplicitTreeAut::SymbolType;
    using Symbol           = VATA::Util::AutDescription::Symbol;          
    using DoubleSymbolType = std::pair<SymbolType, SymbolType>;      //TRANSDUCERS
	using StringSymbolType = ExplicitTreeAut::StringSymbolType;
	using TuplePtr         = ExplicitTreeTransCoreUtil::TuplePtr;
	using FinalStateSet    = ExplicitTreeTransCoreUtil::FinalStateSet;
	using Transition       = ExplicitTreeAut::Transition;
	using AcceptTrans      = ExplicitTreeTransCoreUtil::AcceptTrans;
	using DownAccessor     = ExplicitTreeTransCoreUtil::DownAccessor;

	using DownInclStateTupleSet       = std::set<TuplePtr>;
	using DownInclStateTupleVector    = std::vector<TuplePtr>;

	using AbstractAlphabet = ExplicitTreeAut::AbstractAlphabet;

private:  // data types


	using AlphabetType     = ExplicitTreeAut::AlphabetType;

	using StateSetLight    = Util::OrdVector<StateType>;

	using StringRank       = ExplicitTreeAut::StringRank;


	using TuplePtrSet      = ExplicitTreeTransCoreUtil::TuplePtrSet;
	using TuplePtrSetPtr   = ExplicitTreeTransCoreUtil::TuplePtrSetPtr;
	using TupleSet         = std::set<StateTuple>;
	using TupleCache       = Util::Cache<StateTuple>;

	using SymbolDict                      = ExplicitTreeAut::SymbolDict;
    using TransDict                       = 
        VATA::Util::TwoWayDict<DoubleSymbolType, SymbolType>;           //TRANSDUCER
	using StringSymbolToSymbolTranslStrict= ExplicitTreeAut::StringSymbolToSymbolTranslStrict;
	using StringSymbolToSymbolTranslWeak  = ExplicitTreeAut::StringSymbolToSymbolTranslWeak;
    using DoubleSymbolToSymbolTranslWeak  = Util::TranslatorWeak<TransDict>;    //TRANSDUCER
	using SymbolBackTranslStrict          = ExplicitTreeAut::SymbolBackTranslStrict;


	using iterator        = ExplicitTreeTransCoreUtil::Iterator;
	using const_iterator  = ExplicitTreeTransCoreUtil::Iterator;


	using TransitionCluster              = ExplicitTreeTransCoreUtil::TransitionCluster;
	using TransitionClusterPtr           = ExplicitTreeTransCoreUtil::TransitionClusterPtr;
	using StateToTransitionClusterMap    = ExplicitTreeTransCoreUtil::StateToTransitionClusterMap;
	using StateToTransitionClusterMapPtr = std::shared_ptr<StateToTransitionClusterMap>;

	using Convert          = VATA::Util::Convert;


private:  // data members

	TupleCache& cache_;

	FinalStateSet finalStates_;

	StateToTransitionClusterMapPtr transitions_;

	/**
	 * @brief  The alphabet of the automaton
	 *
	 * This data member is declared as mutable because it may change, e.g.,
	 * during loading of another automaton.
	 */
    mutable AlphabetType alphabet_;

	static AlphabetType globalAlphabet_;

	static TupleCache globalTupleCache_;

    static TransDict globalTransDict_;

    TransDict & transDict_ = globalTransDict_;                       //TRANSDUCERS debug !

private:  // methods


	template <class T>
	static const typename T::mapped_type::element_type* genericLookup(
		const T&                        cont,
		const typename T::key_type&     key)
	{
		auto iter = cont.find(key);
		if (iter == cont.end())
		{
			return nullptr;
		}

		return iter->second.get();
	}


	TuplePtr tupleLookup(const StateTuple& tuple)
	{
		return cache_.lookup(tuple);
	}


	const StateToTransitionClusterMapPtr& uniqueClusterMap()
	{
		assert(nullptr != transitions_);

		if (!transitions_.unique())
		{
			transitions_ = StateToTransitionClusterMapPtr(
				new StateToTransitionClusterMap(*transitions_)
			);

		}

		return transitions_;
	}


	void internalAddTransition(
		const TuplePtr&           children,
		const SymbolType&         symbol,
		const StateType&          parent)
	{
		this->uniqueClusterMap()->uniqueCluster(parent)
			->uniqueTuplePtrSet(symbol)->insert(children);
	}


public:   // methods


	explicit ExplicitTreeTransCore(
		TupleCache&                   tupleCache = globalTupleCache_,
		AlphabetType&                 alphabet = globalAlphabet_,
        TransDict&                    transDict = globalTransDict_ );


	/**
	 * @brief  Selective copy constructor
	 *
	 * This copy constructor copies the whole automaton @p aut, starting from the
	 * @e environment (the alphabet and tuple cache), and optionally also the
	 * transitions (if @p copyTrans  is @p true) and final states (if @p
	 * copyFinal is @p true).
	 *
	 * @param[in]  aut        The automaton to be copied
	 * @param[in]  copyTrans  Copy the transitions?
	 * @param[in]  copyFinal  Copy the final states?
	 */
	ExplicitTreeTransCore(
		const ExplicitTreeTransCore&    aut,
		bool                          copyTrans = true,
		bool                          copyFinal = true);


	ExplicitTreeTransCore(
		ExplicitTreeTransCore&&         aut);

/*
	ExplicitTreeAutCore(
		const ExplicitTreeAutCore&    aut,
		TupleCache&                   tupleCache);
*/

	ExplicitTreeTransCore& operator=(                                 //explicit_tree_incl.cc
		const ExplicitTreeTransCore&    rhs);

/*
	ExplicitTreeAutCore& operator=(
		ExplicitTreeAutCore&&         rhs);
*/

	~ExplicitTreeTransCore()
	{ }

	const FinalStateSet& GetFinalStates() const
	{
		return finalStates_;
	}

	void SetStateFinal(
		const StateType&          state)
	{
		finalStates_.insert(state);
	}
/*
	void SetStatesFinal(
        const std::set<StateType>&    states)
	{
		finalStates_.insert(states.begin(), states.end());
	}
*/
	bool IsStateFinal(                                                      //explicit_tree_sim.cc
		const StateType&          state) const
	{
		return finalStates_.count(state) > 0;
	}
/*
	void EraseFinalStates()
	{
		finalStates_.clear();
	}
*/
/*
	AcceptTrans GetAcceptTrans() const
	{
		return AcceptTrans(*this);bstractAlphabet::BwdTranslatorPtr
	}
*/
	const StateToTransitionClusterMapPtr& GetTransitions() const            //explicit_tree_isect_bu.cc
	{
		return transitions_;
	}

   /**
    * @brief Retrieves a container with all states of the automaton
    *
    * @return A vector with all used states
    */


//	std::unordered_set<size_t> GetUsedStates() const;


/*
	void Clear()
	{
		assert(nullptr != transitions_);

		if (!transitions_.unique())
		{
			transitions_ = StateToTransitionClusterMapPtr(
				new StateToTransitionClusterMap());
		}
		else
		{ // TODO Is this clear enough?
			this->uniqueClusterMap()->clear();
		}

		this->EraseFinalStates();
	}
*/    
/*
	DownAccessor GetDown(
		const StateType&           state) const
	{
		return DownAccessor(*this, state);
	}

	DownAccessor operator[](
		const StateType&           state) const
	{
		return this->GetDown(state);
	}
*/
	const_iterator begin() const
	{
		return const_iterator(*this);
	}


	const_iterator end() const
	{
		return const_iterator(0, *this);
	}

/*
	void AddTransition(const Transition& trans)
	{
		this->AddTransition(
			trans.GetChildren(), trans.GetSymbol(), trans.GetParent());
	}
*/
	void AddTransition(
		const StateTuple&         children,
		const SymbolType&         symbol,
		const StateType&          parent)
	{
		this->internalAddTransition(this->tupleLookup(children), symbol, parent);
	}

/*
	bool ContainsTransition(
		const StateTuple&         children,
		const SymbolType&         symbol,
		const StateType&          parent)
	{
		assert(nullptr != transitions_);

		auto itStateToClusterMap = transitions_->find(parent);
		if (transitions_->end() != itStateToClusterMap)
		{
			assert(nullptr != itStateToClusterMap->second);
			const TransitionCluster& cluster = *itStateToClusterMap->second;

			auto itSymbolToTuplePtrSet = cluster.find(symbol);
			if (cluster.end() != itSymbolToTuplePtrSet)
			{
				assert(nullptr != itSymbolToTuplePtrSet->second);
				const TuplePtrSet& tuplePtrSet = *itSymbolToTuplePtrSet->second;

				return (tuplePtrSet.end() != tuplePtrSet.find(this->tupleLookup(children)));
			}
		}

		return false;
	}

	bool ContainsTransition(
		const Transition&         trans)
	{
		return this->ContainsTransition(
			trans.GetChildren(),
			trans.GetSymbol(),
			trans.GetParent());
	}
    */
/*
	bool AreTransitionsEmpty()
	{
		return this->uniqueClusterMap()->empty();
	}
*/
//	static void CopyTransitions(
//		ExplicitTreeAutCore&           dst,
//		const ExplicitTreeAutCore&     src)
//	{
//		dst.transitions_ = src.transitions_;
//	}


protected:// methods

    template <
        class SymbolTranslFunc>
    std::pair<SymbolType, SymbolType> parseSymbol(
        SymbolTranslFunc&                symbolTransl,
        Symbol&                          symbol)
    {
        size_t position; 
        std::string delimiter = "/";

        std::string trans = symbol.first;
        position = trans.find(delimiter);            //Najde poziciu oddelovaca - "/"
        std::string strSymbol1 = trans.substr(0, position);       //Od indexu 0 po rozdelovac je prvy symbol
        trans.erase(0, position + delimiter.length());       //prvy symbol a oddelovac vymaze

        size_t sym1 = symbolTransl(StringRank(strSymbol1, symbol.second));      //prelozi oba symboly a zapamata si ich
        size_t sym2 = symbolTransl(StringRank(trans, symbol.second));      //oba symboly maju rovnaky rank

        return std::make_pair(sym1, sym2);
    }

	template <
		class StateTranslFunc,
		class SymbolTranslFunc>
	void loadFromAutDescInternal(
		const AutDescription&          desc,
		StateTranslFunc&               stateTransl,
		SymbolTranslFunc&              symbolTransl,
		const std::string&            /* params */ = "")
	{

    size_t counter = transDict_.size();

        DoubleSymbolToSymbolTranslWeak TransTransl(transDict_, [&](const DoubleSymbolType&) {return counter++;});

    	for (auto symbolRankPair : desc.symbols)
		{         
            TransTransl(parseSymbol(symbolTransl, symbolRankPair));                            //Prelozi dvojicu symbolov 
		}

		for (const AutDescription::State& s : desc.finalStates)
		{
			finalStates_.insert(stateTransl(s));
		}

		for (const AutDescription::Transition& t : desc.transitions)
		{
			// traverse the transitions
			const AutDescription::StateTuple& childrenStr = t.first;
			const std::string& symbolStr = t.second;
			const AutDescription::State& parentStr = t.third;

			// translate children
			StateTuple children;
			for (const AutDescription::State& c : childrenStr)
			{ // for all children states
				children.push_back(stateTransl(c));
			}

            Symbol symbol = std::make_pair(symbolStr, children.size()); //obsahuje povodny symbol v  tvare "a/b" a rank ako cislo
			this->AddTransition(
				children,
				TransTransl(parseSymbol(symbolTransl, symbol)),
				stateTransl(parentStr));
		}
	}


    template <
        class BackSymbolTranslFunc>
    StringRank mergeSymbol(
        const BackSymbolTranslFunc&               symbolTransl,
        const SymbolType                          doubleSymbol) const
    {
        std::string mergedSymbol;                                       //tu bude uz spojeny symbol
        std::pair<SymbolType, SymbolType> transSymbol = transDict_.TranslateBwd(doubleSymbol); //rozdeli na dva symboli
        
        StringRank sym1 = (*symbolTransl)(transSymbol.first);
        StringRank sym2 = (*symbolTransl)(transSymbol.second);
    
        mergedSymbol = sym1.symbolStr;
        mergedSymbol += "/";
        mergedSymbol += sym2.symbolStr;

        assert(sym1.rank == sym2.rank);                     //only for structure preserved

        return StringRank(mergedSymbol, sym1.rank);

    }


	template <
		class StateBackTranslFunc>
	AutDescription dumpToAutDescInternal(
		StateBackTranslFunc                       stateTransl,
		const AlphabetType&                       alphabet,
		const std::string&                        /* params */ = "") const
	{
		assert(nullptr != alphabet);

		AbstractAlphabet::BwdTranslatorPtr symbolTransl =
			alphabet->GetSymbolBackTransl();
		assert(nullptr != symbolTransl);

        AutDescription desc;

		for (const StateType& s : finalStates_)
		{
			desc.finalStates.insert(stateTransl(s));
		}

		for (const Transition& t : *this)
		{
			std::vector<std::string> tupleStr;

			for (const StateType& s : t.GetChildren())
			{
				tupleStr.push_back(stateTransl(s));
			}

			AutDescription::Transition trans(
				tupleStr,
				mergeSymbol(symbolTransl, t.GetSymbol()).symbolStr,
				stateTransl(t.GetParent()));
			desc.transitions.insert(trans);
		}

		return desc;
	}

//-----------------------------------------------------------Aplikacia prevodnika na stromomat---------------------

std::string mergeStates(std::pair<StateType, StateType>     DoubleState,
                        VATA::AutBase::StateDict*           stateDictTrans = nullptr,
                        VATA::AutBase::StateDict*           stateDictAut = nullptr)
{
    std::string mergedState;
    if(stateDictTrans)
    {
        mergedState.append(stateDictTrans->TranslateBwd(DoubleState.first));       
    }
    else
    {   
        mergedState.append(std::to_string(DoubleState.first));
    }

    mergedState.append("_");

    if(stateDictAut)
    {
        mergedState.append(stateDictAut->TranslateBwd(DoubleState.second));       
    }
    else
    {   
        mergedState.append(std::to_string(DoubleState.second));
    }

    return mergedState;
}



public:   // methods

ExplicitTreeAutCore Apply(const ExplicitTreeAutCore& aut,
                            bool                       backward = false,
                            VATA::AutBase::StateDict*  stateDictApply = nullptr,
                            VATA::AutBase::StateDict*  stateDictTrans = nullptr,
                            VATA::AutBase::StateDict*  stateDictAut = nullptr)
{
    
    if(stateDictApply == nullptr)
    {
        stateDictApply = new VATA::AutBase::StateDict;
    }

    StateType state(0);
    VATA::AutBase::StringToStateTranslWeak StateTranslator(*stateDictApply, [&state](const std::string&){ return state++; });     //state


    ExplicitTreeAutCore res;

	const AbstractAlphabet::BwdTranslatorPtr& TransSymbolTransl =
			this->GetAlphabet()->GetSymbolBackTransl();

	const AbstractAlphabet::BwdTranslatorPtr& AutSymbolTransl =
            aut.GetAlphabet()->GetSymbolBackTransl();

	VATA::SymbolToDoubleIndexedTransitionListMap STDITLM;

    std::vector<const ProductTranslMap::value_type*> stack;

    VATA::AutBase::ProductTranslMap TranslMap;
    VATA::AutBase::ProductTranslMap *pTranslMap = &TranslMap;


    IndexedSymbolToIndexedTransitionListMap lhsIndex, rhsIndex;
    SymbolToTransitionListMap lhsLeaves, rhsLeaves;
    size_t symbolCnt = 0;
    std::unordered_map<SymbolType, size_t> symbolMap;

	std::unordered_set<size_t> newStates;

    Util::TranslatorWeak2<std::unordered_map<SymbolType, size_t>>
            symbolTranslator(
            symbolMap,
            [&symbolCnt](const SymbolType& sym){ return sym; }
    );
    bottomUpIndex(
            aut, rhsIndex, rhsLeaves, symbolTranslator
    );

    bottomUpIndex(
            *this, lhsIndex, lhsLeaves, symbolTranslator
    );


    for (const auto& lhsSymPair : lhsLeaves)
    {
        SymbolType lhsSym = (transDict_.TranslateBwd(lhsSymPair.first)).first;                
		SymbolType lhsSymSec = (transDict_.TranslateBwd(lhsSymPair.first)).second;               

        if(backward)
        {
            SymbolType tmp = lhsSym;
            lhsSym = lhsSymSec;
            lhsSymSec = tmp;
        }

        for (const auto& transLhs : lhsSymPair.second)
        {
            if (!rhsLeaves.count(lhsSym))
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

                StateTranslator(mergeStates(productState->first, stateDictTrans, stateDictAut));           

                res.AddTransition(std::vector<size_t>(), symbolTranslator.at(lhsSymSec), productState->second);

                if (this->IsStateFinal(parentLhs) && aut.IsStateFinal(parentRhs))
                {
                    res.SetStateFinal(productState->second);
                }
                stack.push_back(&*productState);
            }
        }
    }

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
										
		if (this->IsStateFinal(p->first.first) && aut.IsStateFinal(p->first.second))
		{
			res.SetStateFinal(p->second);
		}

		const auto& leftCluster = ExplicitTreeAutCore::genericLookup(
				*this->transitions_, p->first.first);

		if (!leftCluster)
		{
			continue;
		}

		const auto& rightCluster = ExplicitTreeAutCore::genericLookup(
				*aut.transitions_, p->first.second);

		if (!rightCluster)
		{
			continue;
		}

		assert(leftCluster);

		const std::pair<size_t, size_t>& productState = p->first;

		if (!lhsIndex.count(productState.first)
			|| !rhsIndex.count(productState.second))
		{
			continue;
		}

		for (const auto& lhsSymPair : lhsIndex.at(productState.first))
		{
			const auto lhsDoubleSym = lhsSymPair.first;
			SymbolType lhsSym = (transDict_.TranslateBwd(lhsDoubleSym)).first;
			SymbolType lhsSymSec = (transDict_.TranslateBwd(lhsDoubleSym)).second;

            if(backward)
            {
                SymbolType tmp = lhsSym;
                lhsSym = lhsSymSec;
                lhsSymSec = tmp;
            }

			for (size_t lhsStateIndex = 0;
				 lhsStateIndex < lhsSymPair.second.size();
				 ++lhsStateIndex)
			{
				if (!rhsIndex.at(productState.second).count(lhsSym)
					|| lhsStateIndex >= rhsIndex.at(productState.second).at(lhsSym).size())
				{ 
					continue;
				}

				const auto& rhsTransitions = rhsIndex.at(productState.second).at(
						lhsSym).at(lhsStateIndex);

				for (const auto& lhsTransPtr : lhsSymPair.second.at(lhsStateIndex))
				{
					const BUIndexTransition& lhsTrans = *lhsTransPtr;

					for (const auto& rhsTransPtr : rhsTransitions)
					{
						const BUIndexTransition& rhsTrans = *rhsTransPtr;

						const auto newProduct = pTranslMap->insert(
								std::make_pair(std::make_pair(lhsTrans.state(), rhsTrans.state()),
											   pTranslMap->size())).first;

                        StateTranslator(mergeStates(newProduct->first, stateDictTrans, stateDictAut));           

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

						res.AddTransition(newTuple, symbolTranslator.at(lhsSymSec), newProduct->second);
						stack.push_back(&*newProduct);
					}
				}
			}
		}
	}

    return res;
}




	template <class Index>
	void BuildStateIndex(                                                           ///explicit_tree_comp_down.cc
		Index&                     index) const
	{
		for (const StateType& state : finalStates_)
		{
			index(state);
		}

		for (auto& stateClusterPair : *transitions_)
		{
			assert(stateClusterPair.second);

			index(stateClusterPair.first);

			for (auto& symbolTupleSetPair : *stateClusterPair.second)
			{
				assert(symbolTupleSetPair.second);

				for (auto& tuple : *symbolTupleSetPair.second)
				{
					assert(tuple);

					for (const StateType& s : *tuple)
					{
						index(s);
					}
				}
			}
		}
	}

	template <class Index>
	void ReindexStates(
		ExplicitTreeTransCore&    dst,
		Index&                    index,
		bool                      addFinalStates = true) const
	{
		if (addFinalStates)
		{
			for (const StateType& state : finalStates_)
			{
				dst.SetStateFinal(index.at(state));
			}
		}

		auto clusterMap = dst.uniqueClusterMap();

		for (auto& stateClusterPair : *transitions_)
		{
			assert(stateClusterPair.second);

			auto cluster = clusterMap->uniqueCluster(index.at(stateClusterPair.first));

			for (auto& symbolTupleSetPair : *stateClusterPair.second)
			{
				assert(symbolTupleSetPair.second);

				auto tuplePtrSet = cluster->uniqueTuplePtrSet(symbolTupleSetPair.first);

				for (auto& tuple : *symbolTupleSetPair.second)
				{
					assert(tuple);

					StateTuple newTuple;

					for (const StateType& s : *tuple)
					{
						newTuple.push_back(index.at(s));
					}

					tuplePtrSet->insert(dst.tupleLookup(newTuple));
				}
			}
		}
	}


	template <class Index>
	ExplicitTreeTransCore ReindexStates(                                                  //explicit_tree_incl.cc /incl_param.hh
		Index&                    index,
		bool                      addFinalStates = true) const
	{
		ExplicitTreeTransCore res;
		this->ReindexStates(res, index, addFinalStates);

		return res;
	}

	template <class OperationFunc>
	static void ForeachDownSymbolFromStateAndStateSetDo(                                    //explicit_tree_incl_down.cc
		const ExplicitTreeTransCore&   lhs,
		const ExplicitTreeTransCore&   rhs,
		const StateType&             lhsState,
		const StateSetLight&         rhsSet,
		OperationFunc&               opFunc)
	{
		assert(nullptr != lhs.transitions_);
		assert(nullptr != rhs.transitions_);

		auto leftCluster = ExplicitTreeTransCore::genericLookup(*lhs.transitions_, lhsState);

		if (!leftCluster)
		{
			return;
		}

		std::vector<const TransitionCluster*> rightClusters;

		for (const StateType& rhsState : rhsSet)
		{
			auto rightCluster = ExplicitTreeTransCore::genericLookup(*rhs.transitions_, rhsState);

			if (rightCluster)
			{
				rightClusters.push_back(rightCluster);
			}
		}

		for (auto& leftSymbolTupleSetPair : *leftCluster)
		{
			TuplePtrSet rightTuples;

			for (auto& rightCluster : rightClusters)
			{
				auto rightTupleSet = ExplicitTreeTransCore::genericLookup(
					*rightCluster, leftSymbolTupleSetPair.first
				);

				if (!rightTupleSet)
				{
					continue;
				}

				rightTuples.insert(rightTupleSet->begin(), rightTupleSet->end());
			}

			auto AccessElementF = [](const TuplePtr& tuplePtr){return *tuplePtr;};

			assert(leftSymbolTupleSetPair.second);

			opFunc(
				*leftSymbolTupleSetPair.second,
				AccessElementF,
				rightTuples,
				AccessElementF
			);
		}
	}

	AlphabetType& GetAlphabet() const                                                   //explicit_tree_comp_down.hh
	{
		// Assertions
		assert(nullptr != alphabet_);

		return alphabet_;
	}


	void SetAlphabet(AlphabetType& alphabet)
	{
		// Assertions
		assert(nullptr != alphabet);

		alphabet_ = alphabet;
	}

	static DownInclStateTupleVector StateTupleSetToVector(                              //explicit_tree_incl.cc
		const DownInclStateTupleSet& tupleSet)
	{
		return DownInclStateTupleVector(tupleSet.begin(), tupleSet.end());
	}

	static ExplicitTreeTransCore Union(
		const ExplicitTreeTransCore&            lhs,
		const ExplicitTreeTransCore&            rhs,
		AutBase::StateToStateMap*             pTranslMapLhs = nullptr,
		AutBase::StateToStateMap*             pTranslMapRhs = nullptr);







	static ExplicitTreeTransCore UnionDisjointStates(
		const ExplicitTreeTransCore&           lhs,
		const ExplicitTreeTransCore&           rhs);


	template <
		class SymbolTranslateF>
	ExplicitTreeTransCore TranslateSymbols(
		SymbolTranslateF&                   symbTransl) const
	{
		// copy the environment from this
		ExplicitTreeTransCore aut(*this, false, true);
		for (const Transition& trans : *this)
		{
			aut.AddTransition(
				trans.GetChildren(),
				symbTransl(trans.GetSymbol()),
				trans.GetParent());
		}

		return aut;
	}
};

#endif
