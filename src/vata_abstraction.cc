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

#include "vata_abstraction.hh"

#include <unordered_set>

#define FIRST_STATE 0

bool VATAAbstraction::statesInRel(
			const size_t                             state1,
			const size_t                             state2,
			const std::unordered_map<size_t,size_t>& tmp)
{
    
		if (tmp.size() == 0)
		{ // all states in a relation
			return true;
		}

		const size_t count1 = tmp.count(state1);
		const size_t count2 = tmp.count(state2);

		if (!count1 && !count2)
		{
			return true;
		}

		if (!count1 || !count2)
		{
			return false;
		}

		const size_t relState1 = tmp.at(state1);
		const size_t relState2 = tmp.at(state2);

		if (relState1 == state2 ||
				relState2 == state1 ||
				relState1 == relState2)
		{
			return true;
		}

		return false;
}

void VATAAbstraction::completeSymmetricIndex(
        std::unordered_map<size_t, size_t>&    result,
		const std::unordered_set<size_t>&      usedStates)
{
	for (const size_t state : usedStates)
    {
		if (!result.count(state))
		{
			result[state] = state;
		}
    }
}

StateType VATAAbstraction::GetLastIndex(
        const std::vector<VATA::ExplicitTreeAutCore>&       auts
)
{
    StateType lastState = FIRST_STATE;

    for(size_t i = 0; i < auts.size(); i++)
    {
        const VATA::ExplicitTreeAutCore& currentAut = auts.at(i);

        for(const size_t& currentState : currentAut.GetUsedStates())
        {
            if(currentState > lastState)
            {
                lastState = currentState;
            }
        }
    }

    return lastState;
}


std::vector<VATA::ExplicitTreeAutCore>
VATAAbstraction::AddNewPreditace(std::vector<VATA::ExplicitTreeAutCore> &predicates,
                                 VATA::ExplicitTreeAutCore &newPredicate)
{
    if (predicates.size() == 0)
    {
        predicates.push_back(newPredicate);
    }
    else
    {
        StateType firstStateToReindex = GetLastIndex(predicates);

        std::unordered_map<StateType, StateType>    translMap;

        for(const StateType& state : newPredicate.GetUsedStates())
        {
            translMap.at(state) = ++firstStateToReindex;
        }

        newPredicate.ReindexStates(translMap);
        predicates.push_back(newPredicate);
    }

    return predicates;
}



VATA::ExplicitTreeAutCore VATAAbstraction::GetPredicateAbstraction(const VATA::ExplicitTreeAutCore &aut,
                                                                   const std::vector<VATA::ExplicitTreeAutCore> &predicates)
{
    VATA::ExplicitTreeAutCore result;
    VATA::ExplicitTreeAutCore resAut;

    //translation map shared between each precicate-automata
    VATA::AutBase::ProductTranslMap buIsectTranslMap;

    //bottom-up iset for each predicate automata
    for(const VATA::ExplicitTreeAutCore& currentAut : predicates)
    {
        VATA::ExplicitTreeAutCore::IntersectionBU(currentAut, aut, &buIsectTranslMap);
    }

    std::unordered_map<StateType, std::unordered_set<StateType>> stateToSetOfStatesMap;

    //fullfil the hash table stateToSetOfStatesMap : [state] -> [set_of_states]
    for(const auto& item : buIsectTranslMap)
    {
        StateType left = item.first.first;
        StateType right = item.first.second;

        if (stateToSetOfStatesMap.find(left) == stateToSetOfStatesMap.end())
        {
            stateToSetOfStatesMap[left] = std::unordered_set<StateType>();
        }

        stateToSetOfStatesMap[left].insert(right);
    }

    std::unordered_map<StateType, StateType> stateToStateMap;

    //create equivalence groups
    for (const StateType state1 : aut.GetUsedStates())
    {
        for (const StateType state2 : aut.GetUsedStates())
        {
            if (stateToSetOfStatesMap.find(state1) == stateToSetOfStatesMap.find(state2))
            {
                stateToStateMap[state2] = state1;
            }
        }
    }

    //map equivalence groups into the new automata
    for (const VATA::ExplicitTreeAutCoreUtil::Transition& t : aut)
    {
        VATA::ExplicitTreeAutCore::StateTuple ChildrenTuple;

        for (const VATA::ExplicitTreeAutCore::StateType& s : t.GetChildren())
        {
            ChildrenTuple.push_back(stateToStateMap[s]);
        }

        resAut.AddTransition(ChildrenTuple, t.GetSymbol(), stateToStateMap[t.GetParent()]);
    }

    for(auto& st : aut.GetFinalStates())
    {
        resAut.SetStateFinal(stateToStateMap[st]);
    }

    return resAut;

}








