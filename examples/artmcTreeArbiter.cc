// Tree Arbiter

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
    "States        p0 p1\n"
    "Final States  p1\n"
    "Transitions\n"
    "ii           -> p0\n"
    "i(p0,p0)     -> p0\n"
    "t(p0,p0)     -> p1\n";


const char* Bad1 =
    "Ops           \n"
    "Automaton     aut\n"
    "States        p0 pt perr\n"
    "Final States  p0 perr\n"
    "Transitions\n"
    "rr             -> p0\n"
    "ii             -> p0\n"
    "tt             -> pt\n"
    "i(p0,p0)       -> p0\n"
    "i(pt,p0)       -> pt\n"
    "i(p0,pt)       -> pt\n"
    "i(pt,pt)       -> perr\n"
    "i(perr,p0)     -> perr\n"
    "i(perr,pt)     -> perr\n"
    "i(p0,perr)     -> perr\n"
    "i(pt,perr)     -> perr\n"
    "i(perr,perr)   -> perr\n"
    "b(p0,p0)       -> p0\n"
    "b(pt,p0)       -> pt\n"
    "b(p0,pt)       -> pt\n"
    "b(pt,pt)       -> perr\n"
    "b(perr,p0)     -> perr\n"
    "b(perr,pt)     -> perr\n"
    "b(p0,perr)     -> perr\n"
    "b(pt,perr)     -> perr\n"
    "b(perr,perr)   -> perr\n"
    "r(p0,p0)       -> p0\n"
    "r(pt,p0)       -> pt\n"
    "r(p0,pt)       -> pt\n"
    "r(pt,pt)       -> perr\n"
    "r(perr,p0)     -> perr\n"
    "r(perr,pt)     -> perr\n"
    "r(p0,perr)     -> perr\n"
    "r(pt,perr)     -> perr\n"
    "r(perr,perr)   -> perr\n"
    "t(p0,p0)       -> pt\n"
    "t(pt,p0)       -> perr\n"
    "t(p0,pt)       -> perr\n"
    "t(pt,pt)       -> perr\n"
    "t(perr,p0)     -> perr\n"
    "t(perr,pt)     -> perr\n"
    "t(p0,perr)     -> perr\n"
    "t(pt,perr)     -> perr\n"
    "t(perr,perr)   -> perr\n";


const char* transition =
    "Ops           \n"
    "Automaton     aut\n"
    "States        qi qreq qrel qgrant qcomp\n"
    "Final States  qcomp\n"
    "Transitions\n"
    "ii/ii               -> qi\n"
    "tt/ii               -> qrel\n"
    "i/r(qreq, qreq)     -> qreq\n"	
    "r/t(qreq, qi)       -> qgrant\n"
    "t/b(qi, qgrant)     -> qcomp\n"
    "t/i(qi, qreq)       -> qrel\n"
    "b/t(qi, qrel)       -> qcomp\n"
    "b/b(qi, qcomp)      -> qcomp\n"
    "ii/rr               -> qreq\n"
    "i/i(qi, qi)         -> qi\n"
    "r/r(qreq, qi)       -> qreq\n"
    "r/t(qi, qreq)       -> qgrant\n"
    "t/b(qgrant, qreq)   -> qcomp\n"
    "t/i(qreq, qi)       -> qrel\n"
    "b/t(qrel, qreq)     -> qcomp\n"
    "b/b(qcomp, qreq)    -> qcomp\n"
    "rr/rr               -> qreq\n"
    "i/r(qreq, qi)       -> qreq\n"
    "r/r(qi, qreq)       -> qreq\n"
    "r/t(qreq, qreq)     -> qgrant\n"
    "t/b(qreq, qgrant)   -> qcomp\n"
    "t/i(qreq, qreq)     -> qrel\n"
    "b/t(qreq, qrel)     -> qcomp\n"
    "b/b(qreq, qcomp)    -> qcomp\n"
    "rr/tt               -> qgrant\n"
    "i/r(qi, qreq)       -> qreq\n"
    "r/r(qreq, qreq)     -> qreq\n"
    "t/b(qgrant, qi)     -> qcomp\n"
    "t/i(qi, qi)         -> qrel\n"
    "b/t(qrel, qi)       -> qcomp\n"
    "b/b(qcomp, qi)      -> qcomp\n"
    "t/t(qreq, qreq)     -> qcomp\n"
    "t/t(qi, qi)         -> qcomp\n"
    "t/t(qreq, qi)       -> qcomp\n"
    "t/t(qi, qreq)       -> qcomp\n";


typedef VATA::ExplicitTreeAut Automaton;

typedef VATA::ExplicitTreeTrans Transducer;
typedef VATA::LoadableAut<Transducer> LoadableTransducer;
typedef VATA::LoadableAut<VATA::ExplicitTreeAutCore> LoadableAutCore;

int main()
{
    // Initialize timbuk parser and state dicts (needed only for print out aut/trans)
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(
            new VATA::Parsing::TimbukParser());

    VATA::AutBase::StateDict stateDictInit;
    VATA::AutBase::StateDict stateDictTrans;

    VATA::AutBase::StateDict stateDictBad;

    //Loading Init automaton
    Automaton AutAlpha;
    AutAlpha.LoadFromString(*parser, Init, stateDictInit);
    const VATA::ExplicitTreeAut autInit = AutAlpha;

    //Loading Bad States Automaton
    Automaton BadStates;

    BadStates.LoadFromString(*parser, Bad1, stateDictBad);

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
                std::cout << "Info: Increase height on " << height + 1 << " after " << n + 1<< " steps." << std::endl;

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
            std::cout << "Result:  Fixpoint reached after " << n + 1 << " steps." << std::endl;
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
