#include "../regex_tree/regex_tree.h"
#include <string>
#include <iostream>
using namespace std;

void print_tool(const MatchTool& tool, std::ostream& os) {
    os << "{" << endl;
    for (int i = 0; i < tool.dtran.size(); i++) {
        os << "\t{";
        for (int j = 0; j < tool.dtran[i].size(); j++) {
            os << tool.dtran[i][j];
            if (j != tool.dtran[i].size() - 1)
                os << ", ";
        }
        os << "}" << endl;
    }
    os << "}\n{";
    for (int i = 0; i < tool.targets.size(); i++) {
        os << tool.targets[i];
        if (i != tool.targets.size() - 1)
            os << ", ";
    }
    os << "}" << endl;

}


int main() {
    string ws = "( |\t|\n|\r)+";
    string letter = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z"
                    "|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
    string digit = "(1|2|3|4|5|6|7|8|9|0)";
    string integer = "(+|-)?" + digit + "+";
    string number = integer + "(\\." + digit + "+)?" + "((e|E)" + integer + "+)?";
    string id = "(_|" + letter + ")+";

    MatchTable mt;
    mt.add(ws.c_str());
    mt.add(id.c_str());
    mt.add(number.c_str());
    mt.generate();

    print_tool(mt.getTool(0), cout);
    print_tool(mt.getTool(1), cout);
    print_tool(mt.getTool(2), cout);

}