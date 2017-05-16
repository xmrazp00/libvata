// VATA headers
#include <vata/explicit_tree_aut.hh>
#include <vata/explicit_tree_trans.hh>
#include "../src/loadable_aut.hh"
#include "../src/vata_abstraction.hh"
#include <vata/parsing/timbuk_parser.hh>
#include <vata/serialization/timbuk_serializer.hh>

namespace VATA
{
    namespace ARTMC
    {
        typedef VATA::ExplicitTreeAut Automaton;
        typedef VATA::ExplicitTreeTrans Transducer;

        bool artmc(const VATA::ExplicitTreeAut& a_InitAut,
                   const VATA::ExplicitTreeAut& a_BadStatesAut,
                   const VATA::ExplicitTreeTrans& a_TransitionsTransducer,
                   int a_InitHeight = 1);

        bool artmc(const std::string a_InitStr,
                   const std::string a_BadStatesStr,
                   const std::string a_TransitionsStr,
                   int a_InitHeight = 1);

        bool artmcPredicate(const VATA::ExplicitTreeAut &a_InitAut,
                            const VATA::ExplicitTreeAut &a_BadStatesAut,
                            const VATA::ExplicitTreeTrans &a_TransitionsTransducer);

        bool artmcPredicate(const std::string a_InitStr,
                   const std::string a_BadStatesStr,
                   const std::string a_TransitionsStr);
    }
}

//declaration for backward run
bool backwardRun(VATA::ExplicitTreeAut& AutAlpha,
                 VATA::ExplicitTreeTrans& transducer,
                 const std::vector<VATA::ExplicitTreeAut>& autStack,
                 VATA::ExplicitTreeAutCore* outPredicate = nullptr);


//returns true, if system is correct, otherwise false
bool VATA::ARTMC::artmc(const VATA::ExplicitTreeAut &a_InitAut, const VATA::ExplicitTreeAut &a_BadStatesAut,
                        const VATA::ExplicitTreeTrans &a_TransitionsTransducer, int a_InitHeight)
{
    Transducer transTransitions = a_TransitionsTransducer;
    Automaton AutAlpha = a_InitAut;
    Automaton BadStates = a_BadStatesAut;
    const VATA::ExplicitTreeAut autInit = AutAlpha;

    //Definition of fixpoint automaton and stack for backward run
    VATA::ExplicitTreeAut autFixpoint (autInit);
    std::vector<VATA::ExplicitTreeAut> autStack;

    //Main loop ARTMC
    bool fixpoint = false;
    int n = 0;
    int height = a_InitHeight;

    while(!fixpoint)
    {
        //Push actual aut alpha to stack - each round
        autStack.push_back(AutAlpha);

        //checking whether aut alpha has empty intersection with bad states automaton
        //if it is not empty, trigger backward run, which has 2 cases : 1)return value is true - exists real counterexample
        //2)return value is false - counterexample was spurious, increasing height of abstraction
        if(!VATA::ExplicitTreeAut::Intersection(AutAlpha, BadStates).IsLangEmpty())
        {
            AutAlpha = VATA::ExplicitTreeAut::Intersection(AutAlpha, BadStates);

            if((n == 0 && height == 1) ||
               backwardRun(AutAlpha, transTransitions, autStack))
            {
                //std::cout << "Result: Real counterexample." << std::endl;
                return false;
            }
            else
            {
                std::cout << "Info: Increase height on " << height + 1 << " after " << n + 1 << " steps." << std::endl;

                autStack.clear();
                AutAlpha = Automaton(autInit);
                autFixpoint.Clear();

                height++;
                n = 0;
                continue;
            }
        }

        //make abstraction and apply transducer
        AutAlpha = Automaton(VATAAbstraction::GetHeightAbstraction(AutAlpha.GetCore(), height));
        AutAlpha = transTransitions.Apply(AutAlpha, false);

        //checking whether abstraction and apply brings some new state/states
        //if so, fixpoint was reached, end of computation
        if(VATA::ExplicitTreeAut::CheckInclusion(AutAlpha, autFixpoint))
        {
            std::cout << "Result :  Fixpoint reached after " << n + 1<< " steps." << std::endl;
            fixpoint = true;
        }
        else
        {
            autFixpoint = VATA::ExplicitTreeAut::Union(AutAlpha, autFixpoint);
        }

        //removing useless and unreachable states - works also without them
        AutAlpha = AutAlpha.RemoveUselessStates();
        AutAlpha = AutAlpha.RemoveUnreachableStates();

        n++;
    }
    return true;
}


bool VATA::ARTMC::artmc(const std::string a_InitStr, const std::string a_BadStatesStr,
                        const std::string a_TransitionsStr, int a_InitHeight)
{
    // Initialize timbuk parser and state dicts (needed only for print out aut/trans)
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(
            new VATA::Parsing::TimbukParser());

    VATA::AutBase::StateDict stateDictInit;
    VATA::AutBase::StateDict stateDictTrans;

    VATA::AutBase::StateDict stateDictBad;

    //Loading Init automaton
    Automaton AutInit;
    AutInit.LoadFromString(*parser, a_InitStr, stateDictInit);

    //Loading Bad States Automaton
    Automaton BadStates;

    BadStates.LoadFromString(*parser, a_BadStatesStr, stateDictBad);

    //Loading transducer and setting symbol alphabet for Transducer - it has to be shared
    Transducer transTransitions;
    transTransitions.SetAlphabet(AutInit.GetAlphabet());
    transTransitions.LoadFromString(*parser, a_TransitionsStr, stateDictTrans);

    return artmc(AutInit, BadStates, transTransitions, a_InitHeight);
}

//returns true, if system is correct, otherwise false
bool VATA::ARTMC::artmcPredicate(const VATA::ExplicitTreeAut &a_InitAut, const VATA::ExplicitTreeAut &a_BadStatesAut,
                                const VATA::ExplicitTreeTrans &a_TransitionsTransducer)
{
    Transducer transTransitions = a_TransitionsTransducer;
    Automaton AutAlpha = a_InitAut;
    Automaton BadStates = a_BadStatesAut;
    const VATA::ExplicitTreeAut autInit = AutAlpha;

    //Definition of fixpoint automaton and stack for backward run
    VATA::ExplicitTreeAut autFixpoint (autInit);
    std::vector<VATA::ExplicitTreeAut> autStack;

    //Main loop ARTMC
    bool fixpoint = false;
    int n = 0;

    std::vector<ExplicitTreeAutCore> predicates;
    ExplicitTreeAutCore newPredicate;

    while(!fixpoint)
    {
        //Push actual aut alpha to stack - each round
        autStack.push_back(AutAlpha);

        //checking whether aut alpha has empty intersection with bad states automaton
        //if it is not empty, trigger backward run, which has 2 cases : 1)return value is true - exists real counterexample
        //2)return value is false - counterexample was spurious, increasing height of abstraction
        if(!VATA::ExplicitTreeAut::Intersection(AutAlpha, BadStates).IsLangEmpty())
        {
            AutAlpha = VATA::ExplicitTreeAut::Intersection(AutAlpha, BadStates);

            if(backwardRun(AutAlpha, transTransitions, autStack, &newPredicate))
            {
                //std::cout << "Result: Real counterexample." << std::endl;
                return false;
            }
            else
            {
                if(!newPredicate.IsLangEmpty()) //correct condition ??
                {
                    std::cout << "Info : Adding new predicate" << std::endl;

                    newPredicate = newPredicate.RemoveUnreachableStates();
                    newPredicate = newPredicate.RemoveUselessStates();

                    VATAAbstraction::AddNewPreditace(predicates, newPredicate);
                }

                autStack.clear();
                AutAlpha = Automaton(autInit);
                autFixpoint.Clear();

                n = 0;
                continue;
            }
        }

        //make abstraction and apply transducer
        AutAlpha = Automaton(VATAAbstraction::GetPredicateAbstraction(AutAlpha.GetCore(), predicates));
        AutAlpha = transTransitions.Apply(AutAlpha, false);

        //checking whether abstraction and apply brings some new state/states
        //if so, fixpoint was reached, end of computation
        if(VATA::ExplicitTreeAut::CheckInclusion(AutAlpha, autFixpoint))
        {
            std::cout << "Result:  Fixpoint reached after " << n + 1<< " steps." << std::endl;
            fixpoint = true;
        }
        else
        {
            autFixpoint = VATA::ExplicitTreeAut::Union(AutAlpha, autFixpoint);
        }

        //removing useless and unreachable states - works also without them
        AutAlpha = AutAlpha.RemoveUselessStates();
        AutAlpha = AutAlpha.RemoveUnreachableStates();

        n++;
    }
    return true;
}


bool VATA::ARTMC::artmcPredicate(const std::string a_InitStr, const std::string a_BadStatesStr,
                                const std::string a_TransitionsStr)
{
    // Initialize timbuk parser and state dicts (needed only for print out aut/trans)
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(
            new VATA::Parsing::TimbukParser());

    VATA::AutBase::StateDict stateDictInit;
    VATA::AutBase::StateDict stateDictTrans;

    VATA::AutBase::StateDict stateDictBad;

    //Loading Init automaton
    Automaton AutInit;
    AutInit.LoadFromString(*parser, a_InitStr, stateDictInit);

    //Loading Bad States Automaton
    Automaton BadStates;

    BadStates.LoadFromString(*parser, a_BadStatesStr, stateDictBad);

    //Loading transducer and setting symbol alphabet for Transducer - it has to be shared
    Transducer transTransitions;
    transTransitions.SetAlphabet(AutInit.GetAlphabet());
    transTransitions.LoadFromString(*parser, a_TransitionsStr, stateDictTrans);

    return artmcPredicate(AutInit, BadStates, transTransitions);
}

//Definition of backward run
bool backwardRun(VATA::ExplicitTreeAut& AutAlpha,
                 VATA::ExplicitTreeTrans& transducer,
                 const std::vector<VATA::ExplicitTreeAut>& autStack,
                VATA::ExplicitTreeAutCore* outPredicate)
{
    int h = autStack.size();
    VATA::ExplicitTreeAutCore newPredicate;

    while(h)
    {
        VATA::ExplicitTreeAut StoredAut = autStack.at(h - 1);

        AutAlpha = transducer.Apply(AutAlpha, true);              //without dicts...

        AutAlpha = VATA::ExplicitTreeAut::Intersection(StoredAut, AutAlpha);

        AutAlpha = AutAlpha.RemoveUselessStates();
        AutAlpha = AutAlpha.RemoveUnreachableStates();

        if(AutAlpha.IsLangEmpty())
        {
            if(outPredicate != nullptr)
            {
                *outPredicate = newPredicate;
            }

            return false;
        }

        newPredicate = VATA::ExplicitTreeAutCore(AutAlpha.GetCore());
        h--;
    }
    return true;
}
