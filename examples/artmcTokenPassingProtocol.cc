// Token Passing Protocol

// VATA headers
#include <vata/explicit_tree_aut.hh>
#include <vata/explicit_tree_trans.hh>
#include "../src/loadable_aut.hh"
#include "../src/vata_abstraction.hh"
#include <vata/parsing/timbuk_parser.hh>
#include <vata/serialization/timbuk_serializer.hh>

//declaration for backward run
bool backwardRun(VATA::ExplicitTreeAut& AutAlpha,
                 VATA::ExplicitTreeTrans& transducer,
                 const std::vector<VATA::ExplicitTreeAut>& autStack);

const char* Init =
    "Ops            \n"
    "Automaton     aut\n"
    "States        qN qD\n"
    "Final States  qD\n"
    "Transitions\n"
    "null         -> qN\n"
    "dot          -> qD\n"
    "null(qN, qN) -> qN\n"
    "null(qD, qN) -> qD\n"
    "null(qN, qD) -> qD\n";


const char* Bad1 =
    "Ops           \n"
    "Automaton     aut\n"
    "States        qN\n"
    "Final States  qN\n"
    "Transitions\n"
    "null          -> qN\n"
    "null(qN, qN)  -> qN\n";


const char* Bad2 =
    "Ops            \n"
    "Automaton     aut\n"
    "States        qN qD qF\n"
    "Final States  qF\n"
    "Transitions\n"
    "null         -> qN\n"
    "dot          -> qD\n"
    "null(qN, qN) -> qN\n"
    "dot (qN, qN) -> qD\n"
    "null(qD, qN) -> qD\n"
    "null(qN, qD) -> qD\n"
    "null(qD, qD) -> qF\n"
    "null(qF, qD) -> qF\n"
    "null(qF, qN) -> qF\n"
    "null(qF, qF) -> qF\n"
    "null(qN, qF) -> qF\n"
    "null(qD, qF) -> qF\n"
    "dot (qF, qD) -> qF\n"
    "dot (qF, qN) -> qF\n"
    "dot (qF, qF) -> qF\n"
    "dot (qN, qF) -> qF\n"
    "dot (qD, qF) -> qF\n";


const char* transition =
    "Ops           \n"
    "Automaton     aut\n"
    "States        qN q1\n"
    "Final States  qN\n"
    "Transitions\n"
    "null/null         -> qN\n"
    "dot/null          -> q1\n"
    "dot/null (qN, qN) -> q1\n"
    "null/dot (qN, q1) -> qN\n"
    "null/dot (q1, qN) -> qN\n"
    "null/null(qN, qN) -> qN\n";


typedef VATA::ExplicitTreeAut Automaton;

typedef VATA::ExplicitTreeTrans Transducer;
//typedef VATA::LoadableAut<Transducer> LoadableTransducer;
//typedef VATA::LoadableAut<VATA::ExplicitTreeAutCore> LoadableAutCore;

int main()
{
    // Initialize timbuk parser and state dicts (needed only for print out aut/trans)
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(
		new VATA::Parsing::TimbukParser());

   	VATA::AutBase::StateDict stateDictInit;
	VATA::AutBase::StateDict stateDictTrans;

	VATA::AutBase::StateDict stateDictBad1;
    VATA::AutBase::StateDict stateDictBad2;

    //Loading Init automaton
    Automaton AutAlpha;
    AutAlpha.LoadFromString(*parser, Init, stateDictInit);
    const VATA::ExplicitTreeAut autInit = AutAlpha;

    //Loading Bad States Automatons and making their union
    Automaton BadStates1;
    Automaton BadStates2;

    BadStates1.LoadFromString(*parser, Bad1, stateDictBad1);
    BadStates2.LoadFromString(*parser, Bad2, stateDictBad2);
    
    VATA::ExplicitTreeAutCore::StateToStateMap stateTranslMap1;
	VATA::ExplicitTreeAutCore::StateToStateMap stateTranslMap2;

    VATA::ExplicitTreeAut BadStates =
        VATA::ExplicitTreeAut::Union(BadStates1, BadStates2, &stateTranslMap1, &stateTranslMap2);

    VATA::AutBase::StateDict stateDictBad = VATA::Util::CreateUnionStringToStateMap(stateDictBad1, stateDictBad2,
        &stateTranslMap1, &stateTranslMap2);

    //Loading transducer and setting symbol alphabet for Transducer - it has to be shared
    Transducer transTransitions;
    transTransitions.SetAlphabet(AutAlpha.GetAlphabet());
    transTransitions.LoadFromString(*parser, transition, stateDictTrans);

    VATA::AutBase::StateDict stateDictAlpha = VATA::AutBase::StateDict(stateDictInit);
    VATA::AutBase::StateDict stateDictFixpoint = VATA::AutBase::StateDict(stateDictInit);

    //Definition of fixpoint automaton and stack for backward run
    VATA::ExplicitTreeAut autFixpoint (autInit);
    std::vector<VATA::ExplicitTreeAut> autStack;

    //Main loop ARTMC
    bool fixpoint = false;
    int n = 0;
    int height = 1;

    while(!fixpoint)
    {
        //Push actual aut alpha to stack - each round
        autStack.push_back(AutAlpha);
        VATA::AutBase::StateDict stateDictTmp;

        //checking whether aut alpha has empty intersection with bad states automaton
        //if it is not empty, trigger backward run, which has 2 cases : 1)return value is true - exists real counterexample
        //2)return value is false - counterexample was spurious, increasing height of abstraction
        if(!VATA::ExplicitTreeAut::Intersection(AutAlpha, BadStates).IsLangEmpty())
        {
            AutAlpha = VATA::ExplicitTreeAut::Intersection(AutAlpha, BadStates);

            if((n == 0 && height == 1) || 
                    backwardRun(AutAlpha, transTransitions, autStack))
            {
                std::cout << "Result: Real counterexample." << std::endl;
                break;
            }
            else
            {
                std::cout << "Info: Increase height on " << height + 1 << " after " << n + 1 << " steps." << std::endl;

                autStack.clear();
                AutAlpha = Automaton(autInit);
                autFixpoint.Clear();
                stateDictFixpoint = VATA::AutBase::StateDict();
            
                height++;
                n = 0;
                continue;
            }
        }

        //make abstraction and apply transducer
        AutAlpha = Automaton(VATAAbstraction::GetHeightAbstraction(AutAlpha.GetCore(), height));
        AutAlpha = transTransitions.Apply(AutAlpha, false, &stateDictTmp, &stateDictTrans);
        stateDictAlpha = stateDictTmp;

        //checking whether abstraction and apply brings some new state/states
        //if so, fixpoint was reached, end of computation
        if(VATA::ExplicitTreeAut::CheckInclusion(AutAlpha, autFixpoint))
        {
            std::cout << "Result:  Fixpoint reached after " << n + 1<< " steps." << std::endl;
            fixpoint = true;
        }
        else
        {
        	Automaton::StateToStateMap stateTranslMap1;
        	Automaton::StateToStateMap stateTranslMap2;
       
            autFixpoint = VATA::ExplicitTreeAut::Union(AutAlpha, autFixpoint, &stateTranslMap1, &stateTranslMap2);
        	stateDictFixpoint =
    	    	VATA::Util::CreateUnionStringToStateMap(stateDictAlpha, stateDictFixpoint, &stateTranslMap1, &stateTranslMap2);
        }

        //removing useless and unreachable states - works also without them
        AutAlpha = AutAlpha.RemoveUselessStates();
        AutAlpha = AutAlpha.RemoveUnreachableStates();

        n++;
    }

    return 0;
}

//Definition of backward run
bool backwardRun(VATA::ExplicitTreeAut& AutAlpha,
                 VATA::ExplicitTreeTrans& transducer,
                 const std::vector<VATA::ExplicitTreeAut>& autStack)
{
    int h = autStack.size();
    
    while(h)
    {
        VATA::ExplicitTreeAut StoredAut = autStack.at(h - 1);

        AutAlpha = transducer.Apply(AutAlpha, true);              //without dicts...

        AutAlpha = VATA::ExplicitTreeAut::Intersection(StoredAut, AutAlpha);

        AutAlpha = AutAlpha.RemoveUselessStates();
        AutAlpha = AutAlpha.RemoveUnreachableStates();

        if(AutAlpha.IsLangEmpty())
        {
            return false;
        }
        h--;
    }
    return true;
}
