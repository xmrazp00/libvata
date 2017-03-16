/*
 * Copyright (C) 2014 Martin Hruska
 *
 * This file is part of forester.
 *
 * forester is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * forester is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with forester.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VATA_ABSTRACTION_H
#define VATA_ABSTRACTION_H

// std headers
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include <vata/aut_base.hh>
#include "explicit_tree_aut_core.hh"
#include "expl_bu_index.hh"


class VATAAbstraction
{
public:
    // currently erases '1' from the relation
	static void heightAbstraction(
		const VATA::ExplicitTreeAutCore&           aut,
		std::unordered_map<size_t, size_t>&        result,
		size_t                                     height)
	{
			std::unordered_map<size_t, size_t> tmp;
			const std::unordered_set<size_t> usedStates(aut.GetUsedStates());

			while (height--)
			{
					tmp = result;

                    //std::cout << "Height is " << height << std::endl;

					for (const size_t state1 : usedStates)
					{
							for (const size_t state2 : usedStates)
							{
									if (VATAAbstraction::areStatesEquivalent(
												aut, state1, state2, tmp))
									{
            //                                std::cout << "State : " <<  state2  << " is mapping on : " << state1 << ". " << std::endl;
											result[state2] = state1;
									}
							}
					}
				}

				VATAAbstraction::completeSymmetricIndex(result, usedStates);
	}

private:
    /**
     * @brief Function check whether two states are in a relation in a given automaton
     *
     * Checks whether two states are in a relation that means checking if all of 
     * their transitions match.
     *
     * @return true When states are in the relation, otherwise false
     */

	static bool areStatesEquivalent(
		const VATA::ExplicitTreeAutCore&         aut,
		const size_t                             state1,
		const size_t                             state2,
		const std::unordered_map<size_t,size_t>& tmp)
	{
			if (state1 == state2)
			{
					return true;
			}

    bool StatesMatched;
    
    for (const typename VATA::ExplicitTreeAutCore::Transition trans1 : aut[state2])
		{
            StatesMatched = false;
			for (const typename VATA::ExplicitTreeAutCore::Transition trans2 : aut[state1])
			{
				if (VATAAbstraction::transMatch(
						trans1, trans2, tmp))
				{
					StatesMatched = true;
                    break;
				}
			}
            if(!StatesMatched)
            {
                return false;
            }
		}

    for (const typename VATA::ExplicitTreeAutCore::Transition trans1 : aut[state1])
		{
            StatesMatched = false;
			for (const typename VATA::ExplicitTreeAutCore::Transition trans2 : aut[state2])
			{
				if (VATAAbstraction::transMatch(
						trans1, trans2, tmp))
				{
					StatesMatched = true;
                    break;
				}
			}
            if(!StatesMatched)
            {
                return false;
            }
		}
	return true;
	}

   /**
	 * @brief  Determines whether two transitions match
	 *
	 * This function determines whether two transitions_ match (and can therefore
	 * e.g. be merged during abstraction). First, the @p funcMatch functor is used
	 * to determine whether the transitions are to be checked at all.
	 */
	template <class T>
	static bool transMatch(
		const T&                                  trans1,
		const T&                                  trans2,
		const std::unordered_map<size_t, size_t>& rel)
	{
			if (trans1.GetChildrenSize() != trans2.GetChildrenSize())
			{
					return false;
			}


			if (trans1.GetSymbol() != trans2.GetSymbol())
			{ // check symbol
					return false;
			}


			for (size_t m = 0; m < trans1.GetChildrenSize(); ++m)
			{
				const size_t ch1 = trans1.GetNthChildren(m);
				const size_t ch2 = trans2.GetNthChildren(m);
				
				if (!statesInRel(ch1, ch2, rel))
				{ // NOTE A signature of children is not checked here
						return false;
				}
			}

			return true;
	}

	static bool statesInRel(
		const size_t                             state1,
		const size_t                             state2,
		const std::unordered_map<size_t,size_t>& tmp);


	static void completeSymmetricIndex(
		std::unordered_map<size_t,size_t>&         result,
		const std::unordered_set<size_t>&          usedStates);


    /*
     *  PREDICATE ABSTRACTION
     */

    static StateType GetLastIndex(
            const std::vector<VATA::ExplicitTreeAutCore>&          auts
    );


public:
	static VATA::ExplicitTreeAutCore GetHeightAbstraction(
			const VATA::ExplicitTreeAutCore&			aut,
			size_t 										height)

	{
		VATA::ExplicitTreeAutCore res(aut, false, false);
		res.SetAlphabet(aut.GetAlphabet());

		std::unordered_map<size_t, size_t> ResultMap;
		VATAAbstraction::heightAbstraction(aut, ResultMap, height);

		for (const VATA::ExplicitTreeAutCoreUtil::Transition& t : aut)
		{
			VATA::ExplicitTreeAutCore::StateTuple ChildrenTuple;

			for (const VATA::ExplicitTreeAutCore::StateType& s : t.GetChildren())
			{
				ChildrenTuple.push_back(ResultMap[s]);
			}

			res.AddTransition(ChildrenTuple, t.GetSymbol(), ResultMap[t.GetParent()]);
		}

        for(auto& st : aut.GetFinalStates())
        {
            res.SetStateFinal(ResultMap[st]);
        }
		return res;
	}

    /*
     *  PREDICATE ABSTRACTION
     */


    static std::vector<VATA::ExplicitTreeAutCore> AddNewPreditace(
            std::vector<VATA::ExplicitTreeAutCore>&                 predicates,
            VATA::ExplicitTreeAutCore&                              newPredicate
    );

    static VATA::ExplicitTreeAutCore GetPredicateAbstraction(
            const VATA::ExplicitTreeAutCore&                    aut,
            const std::vector<VATA::ExplicitTreeAutCore>&       predicates
    );


};

#endif
