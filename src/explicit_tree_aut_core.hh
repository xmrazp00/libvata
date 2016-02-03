/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Header file for the core of an explicitly represented tree automaton.
 *
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_TREE_AUT_CORE_HH_
#define _VATA_EXPLICIT_TREE_AUT_CORE_HH_

// VATA headers
#include <vata/aut_base.hh>
#include <vata/explicit_tree_aut.hh>

#include <vata/util/convert.hh>
#include <vata/util/ord_vector.hh>
#include <vata/util/transl_strict.hh>
#include <vata/util/util.hh>

#include <vata/explicit_lts.hh>
#include <vata/incl_param.hh>

#include "util/cache.hh"


namespace VATA
{
	class ExplicitTreeAutCore;

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
	namespace ExplicitTreeAutCoreUtil
	{
		using StateType        = ExplicitTreeAut::StateType;
		using FinalStateSet    = ExplicitTreeAut::FinalStateSet;
		using SymbolType       = ExplicitTreeAut::SymbolType;
        //---------TRANSDUCER------------------------------------
        using DoubleSymbolType = ExplicitTreeAut::DoubleSymbolType;
        using TransDict        = ExplicitTreeAut::TransDict;
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

//		class AcceptTrans;
//		class DownAccessor;
	}
}


/**
 * @brief  Base class for transition iterators
 */
class VATA::ExplicitTreeAutCoreUtil::BaseTransIterator
{
public:   // data types

	using iterator_category  = std::input_iterator_tag;
	using difference_type    = size_t;

	using value_type = Transition;
	using pointer    = Transition*;
	using reference  = Transition&;

protected:  // data members

	const ExplicitTreeAutCore& aut_;

	
    StateToTransitionClusterMap::const_iterator stateClusterIterator_;
	TransitionCluster::const_iterator symbolSetIterator_;
	TuplePtrSet::const_iterator tupleIterator_;

protected:// methods

	BaseTransIterator(
		int                          /* FILL (only to distinguish signature?) */,
		const ExplicitTreeAutCore&   aut) :
		aut_(aut),
		stateClusterIterator_(),
		symbolSetIterator_(),
		tupleIterator_()
	{ }

	BaseTransIterator(
		const ExplicitTreeAutCore&   aut);
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
class VATA::ExplicitTreeAutCoreUtil::Iterator : public BaseTransIterator
{
GCC_DIAG_ON(effc++)

public:   // methods

	Iterator(
		int                          /* FILL (only to distinguish signature?) */,
		const ExplicitTreeAutCore&   aut) :
		BaseTransIterator(0, aut)
	{ }

	explicit Iterator(
		const ExplicitTreeAutCore&   aut) :
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
class VATA::ExplicitTreeAutCoreUtil::AcceptTransIterator : public BaseTransIterator
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

class VATA::ExplicitTreeAutCoreUtil::DownAccessorIterator
{
private:  // data members

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

class VATA::ExplicitTreeAutCoreUtil::DownAccessor
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


class VATA::ExplicitTreeAutCoreUtil::AcceptTrans
{
private:  // data members

	const ExplicitTreeAutCore& aut_;

public:   // methods

	explicit AcceptTrans(const ExplicitTreeAutCore& aut) :
		aut_(aut)
	{ }

	using iterator        = AcceptTransIterator;
	using const_iterator  = AcceptTransIterator;

//	AcceptTransIterator begin() const { return AcceptTransIterator(aut_); }
//	AcceptTransIterator end() const { return AcceptTransIterator(0, aut_); }
};


GCC_DIAG_OFF(effc++)
class VATA::ExplicitTreeAutCore : public TreeAutBase
{
GCC_DIAG_ON(effc++)

	friend class ExplicitUpwardInclusion;
	friend class ExplicitDownwardComplementation;
	friend class ExplicitDownwardInclusion;

	friend class ExplicitTreeAutCoreUtil::BaseTransIterator;
	friend class ExplicitTreeAutCoreUtil::Iterator;
	friend class ExplicitTreeAutCoreUtil::AcceptTrans;
	friend class ExplicitTreeAutCoreUtil::AcceptTransIterator;
	friend class ExplicitTreeAutCoreUtil::DownAccessor;
	friend class ExplicitTreeAutCoreUtil::DownAccessorIterator;

public:   // data types

	using SymbolType       = ExplicitTreeAut::SymbolType;
    using Symbol           = VATA::Util::AutDescription::Symbol;           //TRANSDUCER
    using DoubleSymbolType = ExplicitTreeAut::DoubleSymbolType;         //TRANSDUCERS
	using StringSymbolType = ExplicitTreeAut::StringSymbolType;
	using TuplePtr         = ExplicitTreeAutCoreUtil::TuplePtr;
	using FinalStateSet    = ExplicitTreeAutCoreUtil::FinalStateSet;
	using Transition       = ExplicitTreeAut::Transition;
	using AcceptTrans      = ExplicitTreeAutCoreUtil::AcceptTrans;
	using DownAccessor     = ExplicitTreeAutCoreUtil::DownAccessor;

	using DownInclStateTupleSet       = std::set<TuplePtr>;
	using DownInclStateTupleVector    = std::vector<TuplePtr>;

	using AbstractAlphabet = ExplicitTreeAut::AbstractAlphabet;

private:  // data types


	using AlphabetType     = ExplicitTreeAut::AlphabetType;

	using StateSetLight    = Util::OrdVector<StateType>;

	using StringRank       = ExplicitTreeAut::StringRank;


	using TuplePtrSet      = ExplicitTreeAutCoreUtil::TuplePtrSet;
	using TuplePtrSetPtr   = ExplicitTreeAutCoreUtil::TuplePtrSetPtr;
	using TupleSet         = std::set<StateTuple>;
	using TupleCache       = Util::Cache<StateTuple>;

	using SymbolDict                      = ExplicitTreeAut::SymbolDict;
    using TransDict                       = ExplicitTreeAut::TransDict;                    //TRANSDUCER
	using StringSymbolToSymbolTranslStrict= ExplicitTreeAut::StringSymbolToSymbolTranslStrict;
	using StringSymbolToSymbolTranslWeak  = ExplicitTreeAut::StringSymbolToSymbolTranslWeak;
    using DoubleSymbolToSymbolTranslWeak  = ExplicitTreeAut::DoubleSymbolToSymbolTranslWeak;    //TRANSDUCER
	using SymbolBackTranslStrict          = ExplicitTreeAut::SymbolBackTranslStrict;


	using iterator        = ExplicitTreeAutCoreUtil::Iterator;
	using const_iterator  = ExplicitTreeAutCoreUtil::Iterator;


	using TransitionCluster              = ExplicitTreeAutCoreUtil::TransitionCluster;
	using TransitionClusterPtr           = ExplicitTreeAutCoreUtil::TransitionClusterPtr;
	using StateToTransitionClusterMap    = ExplicitTreeAutCoreUtil::StateToTransitionClusterMap;
	using StateToTransitionClusterMapPtr = std::shared_ptr<StateToTransitionClusterMap>;

	using Convert          = VATA::Util::Convert;


private:  // data members

	TupleCache& cache_;

	FinalStateSet finalStates_;

	StateToTransitionClusterMapPtr transitions_;

    TransDict transDict;

	/**
	 * @brief  The alphabet of the automaton
	 *
	 * This data member is declared as mutable because it may change, e.g.,
	 * during loading of another automaton.
	 */
	mutable AlphabetType alphabet_;

	static AlphabetType globalAlphabet_;

	static TupleCache globalTupleCache_;

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


	explicit ExplicitTreeAutCore(
		TupleCache&                   tupleCache = globalTupleCache_,
		AlphabetType&                 alphabet = globalAlphabet_);


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
	ExplicitTreeAutCore(
		const ExplicitTreeAutCore&    aut,
		bool                          copyTrans = true,
		bool                          copyFinal = true);


	ExplicitTreeAutCore(
		ExplicitTreeAutCore&&         aut);

/*
	ExplicitTreeAutCore(
		const ExplicitTreeAutCore&    aut,
		TupleCache&                   tupleCache);
*/

	ExplicitTreeAutCore& operator=(                                 //explicit_tree_incl.cc
		const ExplicitTreeAutCore&    rhs);

/*
	ExplicitTreeAutCore& operator=(
		ExplicitTreeAutCore&&         rhs);
*/

	~ExplicitTreeAutCore()
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

        std::string delimiter = "/";
        size_t counter = 0;

        DoubleSymbolToSymbolTranslWeak TransTransl(transDict, [&](const DoubleSymbolType&) {return counter++;});


    	for (auto symbolRankPair : desc.symbols)
		{         
            TransTransl(parseSymbol(symbolTransl, symbolRankPair));                            //Prelozi dvojicu symbolov 
		}
/*
            auto s = transDict.TranslateBwd(0);
            auto a = this->GetAlphabet()->GetSymbolBackTransl()->operator()(s.first); 
            auto b = this->GetAlphabet()->GetSymbolBackTransl()->operator()(s.second);
            std::cout << "\n--- SPATNY VYPIS ZNAKOV--- \n";
            std::cout << a << " a " << b <<"\n";*/

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
        std::pair<SymbolType, SymbolType> transSymbol = transDict.TranslateBwd(doubleSymbol); //rozdeli na dva symboli
        
        StringRank sym1 = (*symbolTransl)(transSymbol.first);
        StringRank sym2 = (*symbolTransl)(transSymbol.second);

        mergedSymbol = sym1.symbolStr;
        mergedSymbol += "/";
        mergedSymbol += sym2.symbolStr;

        assert(sym1.rank == sym2.rank);                     //overi, ci sa rovnaju ranky. Treba ?

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


public:   // methods


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
		ExplicitTreeAutCore&      dst,
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
	ExplicitTreeAutCore ReindexStates(                                                  //explicit_tree_incl.cc /incl_param.hh
		Index&                    index,
		bool                      addFinalStates = true) const
	{
		ExplicitTreeAutCore res;
		this->ReindexStates(res, index, addFinalStates);

		return res;
	}

/*
	ExplicitTreeAutCore ReindexStates(
		StateToStateTranslWeak&     stateTransl) const
	{
		return this->ReindexStates<StateToStateTranslWeak>(stateTransl);
	}
*/
/*
	template <class CopyFctor>
	void CopyTransitionsFrom(
		const ExplicitTreeAutCore&    src,
		CopyFctor&                    fctor)
	{
		for (const Transition& trans : src)
		{
			if (fctor(trans))
			{
				this->AddTransition(trans);
			}
		}
	}
*/
	template <class OperationFunc>
	static void ForeachDownSymbolFromStateAndStateSetDo(                                    //explicit_tree_incl_down.cc
		const ExplicitTreeAutCore&   lhs,
		const ExplicitTreeAutCore&   rhs,
		const StateType&             lhsState,
		const StateSetLight&         rhsSet,
		OperationFunc&               opFunc)
	{
		assert(nullptr != lhs.transitions_);
		assert(nullptr != rhs.transitions_);

		auto leftCluster = ExplicitTreeAutCore::genericLookup(*lhs.transitions_, lhsState);

		if (!leftCluster)
		{
			return;
		}

		std::vector<const TransitionCluster*> rightClusters;

		for (const StateType& rhsState : rhsSet)
		{
			auto rightCluster = ExplicitTreeAutCore::genericLookup(*rhs.transitions_, rhsState);

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
				auto rightTupleSet = ExplicitTreeAutCore::genericLookup(
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

/*
	void SetAlphabet(AlphabetType& alphabet)
	{
		// Assertions
		assert(nullptr != alphabet);

		alphabet_ = alphabet;
	}
*/
	static DownInclStateTupleVector StateTupleSetToVector(                              //explicit_tree_incl.cc
		const DownInclStateTupleSet& tupleSet)
	{
		return DownInclStateTupleVector(tupleSet.begin(), tupleSet.end());
	}


	template <class Index = Util::IdentityTranslator<StateType>>
	ExplicitLTS TranslateDownward(                                                         //explicit_tree_sim.cc
		size_t        numStates,
		Index&        stateIndex = Index()) const;


	template <
		class Rel,
		class Index = Util::IdentityTranslator<StateType>
	>
	ExplicitLTS TranslateUpward(                                                            //explicit_tree_transl.hh / sim.cc
		std::vector<std::vector<size_t>>&     partition,
		Util::BinaryRelation&                 relation,
		const Rel&                            param,
		Index&                                stateIndex = Index()) const;


	// template <
	// 	class Rel,
	// 	class Index = Util::IdentityTranslator<AutBase::StateType>>
	// ExplicitTreeAutCore CollapseStates(
	// 	const Rel&                       rel,
	// 	const Index&                     bwIndex = Index()) const
	// {
	// 	std::vector<size_t> representatives;
  //
	// 	rel.buildClasses(representatives);
  //
	// 	std::vector<StateType> transl(representatives.size());
  //
	// 	Util::RebindMap2(transl, representatives, bwIndex);
  //
	// 	// TODO: directly return the output of ReindexStates?
	// 	ExplicitTreeAutCore res(cache_);
  //
	// 	this->ReindexStates(res, transl);
  //
	// 	return res;
	// }


	/**
	 * @brief  Collapses states according to the given mapping
	 *
	 * @param[in]  stateMap  Mapping of old states to new states (does not need
	 *                       to be injective)
	 *
	 * @returns  The automaton with collapsed states
	 */
/*
    template <
		class MapType>
	ExplicitTreeAutCore CollapseStates(
		MapType&      stateMap) const
	{
		return this->ReindexStates(stateMap);
	}
*/
	//
	// simulation computation
	//

	StateDiscontBinaryRelation ComputeSimulation(                                           //explicit_tree_sim.cc
		const VATA::SimParam&          params) const;

	StateDiscontBinaryRelation ComputeDownwardSimulation(                                   //explicit_tree_sim.cc
		const VATA::SimParam&          params) const;

	StateDiscontBinaryRelation ComputeDownwardSimulation(                                   //explicit_tree_sim.cc
		size_t                         size) const;

	// template <class Index>
	// AutBase::StateBinaryRelation ComputeDownwardSimulation(
	// 	size_t            size,
	// 	const Index&      index) const;


	StateDiscontBinaryRelation ComputeUpwardSimulation(                                     //explicit_tree_sim.cc
		const VATA::SimParam&          params) const;

	StateDiscontBinaryRelation ComputeUpwardSimulation(                                     //explicit_tree_sim.cc
		size_t                         size) const;

	// template <class Index>
	// AutBase::StateBinaryRelation ComputeUpwardSimulation(
	// 	size_t                   size,
	// 	const Index&             index) const;


	static ExplicitTreeAutCore Union(
		const ExplicitTreeAutCore&            lhs,
		const ExplicitTreeAutCore&            rhs,
		AutBase::StateToStateMap*             pTranslMapLhs = nullptr,
		AutBase::StateToStateMap*             pTranslMapRhs = nullptr);







	static ExplicitTreeAutCore UnionDisjointStates(
		const ExplicitTreeAutCore&           lhs,
		const ExplicitTreeAutCore&           rhs);






	static ExplicitTreeAutCore Intersection(                                                //explicit_tree_isect.cc
		const ExplicitTreeAutCore&           lhs,
		const ExplicitTreeAutCore&           rhs,
		VATA::AutBase::ProductTranslMap*     pTranslMap = nullptr);


	static ExplicitTreeAutCore IntersectionBU(                                              //explicit_tree_isect_bu.cc
		const ExplicitTreeAutCore&           lhs,
		const ExplicitTreeAutCore&           rhs,
		VATA::AutBase::ProductTranslMap*     pTranslMap = nullptr);


	ExplicitTreeAutCore GetCandidateTree() const;                                           //explicit_tree_candidate.cc


	ExplicitTreeAutCore RemoveUnreachableStates(                                            //explicit_tree_candidate.cc
		AutBase::StateToStateMap*            pTranslMap = nullptr) const;


	ExplicitTreeAutCore RemoveUselessStates(                                                //explicit_tree_comp_down.cc
		StateToStateMap*                    pTranslMap = nullptr) const;


	template <
		class Rel,
		class Index
	>
	ExplicitTreeAutCore RemoveUnreachableStates(                                            //explicit_tree_unreach.hh
		const Rel&                          rel,
		const Index&                        index) const;


/*
	static bool CheckInclusion(                                                               //explicit_tree_incl.cc 
		const ExplicitTreeAutCore&          smaller,
		const ExplicitTreeAutCore&          bigger)
	{
		InclParam inclParam;
		// TODO: set more sensible defaults

		return ExplicitTreeAutCore::CheckInclusion(smaller, bigger, inclParam);
	}

*/
	static bool CheckInclusion(                                                                 //explicit_tree_incl.cc
		const ExplicitTreeAutCore&          smaller,
		const ExplicitTreeAutCore&          bigger,
		const VATA::InclParam&              params);


	template <
		class Rel>
	ExplicitTreeAutCore ComplementWithPreorder(                                                   //explicit_tree_comp_down.cc
		const Rel&                          preorder) const;


	ExplicitTreeAutCore Complement() const;

/*
	bool IsLangEmpty() const
	{
		ExplicitTreeAutCore autTmp = this->RemoveUselessStates();

		// after removing useless states, we just need to check whether there is
		// a~final state in the automaton
		return autTmp.GetFinalStates().empty();
	}


	ExplicitTreeAutCore Reduce() const
	{
		ReduceParam params;
		params.SetRelation(ReduceParam::e_reduce_relation::TA_DOWNWARD);
		return this->Reduce(params);
	}


	ExplicitTreeAutCore Reduce(
		const ReduceParam&            params) const;
*/

	template <
		class SymbolTranslateF>
	ExplicitTreeAutCore TranslateSymbols(
		SymbolTranslateF&                   symbTransl) const
	{
		// copy the environment from this
		ExplicitTreeAutCore aut(*this, false, true);
		for (const Transition& trans : *this)
		{
			aut.AddTransition(
				trans.GetChildren(),
				symbTransl(trans.GetSymbol()),
				trans.GetParent());
		}

		return aut;
	}
/*
	std::string ToString(
		const Transition&                   trans) const;
*/
};

#endif
