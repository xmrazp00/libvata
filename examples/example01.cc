// example1.cc - loading and dumping an automaton

// VATA headers
#include <vata/bdd_bu_tree_aut.hh>
#include <vata/bdd_td_tree_aut.hh>
#include <vata/explicit_tree_aut.hh>
#include <vata/parsing/timbuk_parser.hh>
#include <vata/serialization/timbuk_serializer.hh>
#include <vata/util/two_way_trans_dict.hh>

const char* autStr =
	"Ops           a/b:0 b/d:2\n"
	"Automaton     aut\n"
	"States        q0 q1\n"
	"Final States  q1\n"
	"Transitions\n"
	"a/b        -> q0\n"
	"b/d(q0, q0)  -> q1\n";

typedef VATA::ExplicitTreeAut Automaton;
//typedef VATA::BDDBottomUpTreeAut Automaton;  // uncomment for BDD BU automaton
//typedef VATA::BDDTopDownTreeAut Automaton;   // uncomment for BDD TD automaton

int main()
{
	// create the parser for the Timbuk format
	std::unique_ptr<VATA::Parsing::AbstrParser> parser(
		new VATA::Parsing::TimbukParser());

	// create the dictionary for translating state names to internal state numbers
	VATA::AutBase::StateDict stateDict;
 
    std::cout << autStr;

//================================================== TEST TRANSDUCERS ===========================================
/*
    Automaton::SymbolType t1(0);              
    Automaton::SymbolType t2(1);              
    Automaton::SymbolType t3(2);              
    Automaton::SymbolType t4(3);              
    
    using StringRank = Automaton::StringRank;
    using SymDict = VATA::Util::TwoWayDict<StringRank, Automaton::SymbolType>;
    SymDict symdict;
    size_t symbol2 = 0;
    VATA::Util::TranslatorWeak<SymDict> TransSymbolDict(symdict, [&](const StringRank) {return symbol2++;});
    
    StringRank SR("Ferko", 2); 

    size_t num = TransSymbolDict(SR);
    auto back = symdict.TranslateBwd(num);
   
    std::cout << "STRINGRANK bol prelozeny do : " << num << ", a jeho hodnoty su : " << back.symbolStr << " a " << back.rank <<
    "\n";
    
    
    using DoubleSymbolType = std::pair<Automaton::SymbolType, Automaton::SymbolType>; 
    DoubleSymbolType DST = std::make_pair(t1, t2);
    DoubleSymbolType DST2 = std::make_pair(t3, t4);
    
    using Dict = VATA::Util::TwoWayDict<DoubleSymbolType, Automaton::SymbolType>;
    Dict dict;
    size_t symbol = 0;

    VATA::Util::TranslatorWeak<Dict> TransDict(dict, [&](const DoubleSymbolType&) {return symbol++;});

    size_t s = TransDict(DST);
    auto sym = dict.TranslateBwd(s);
    
    std::cout << "Prelozene do cisla je : " << s << " a je to dvojica : " << sym.first <<", " << sym.second << "\n";
   
    s = TransDict(DST2);
    sym = dict.TranslateBwd(s);

    std::cout << "Prelozene do cisla je : " << s << " a je to dvojica : " << sym.first <<", " << sym.second << "\n";

*/
//================================================== TEST TRANSDUCERS ===========================================

	// create and load the automaton
	Automaton aut;
	aut.LoadFromString(*parser, autStr, stateDict);

	// create the serializer for the Timbuk format
	VATA::Serialization::AbstrSerializer* serializer =
		new VATA::Serialization::TimbukSerializer();
    
	// dump the automaton
	std::cout << aut.DumpToString(*serializer, stateDict);
}
