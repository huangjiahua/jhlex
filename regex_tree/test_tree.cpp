#include <iostream>
#include "regex_tree.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

int main() {
    string letter = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z"
                    "|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
    string stra = "";
    string id = "(_|" + letter + ")+";
    string digit = "(1|2|3|4|5|6|7|8|9|0)";
    string integer = "(+|-)?" + digit + "+";
    string number = integer + "(\\." + digit + "+)?" + "((e|E)" + integer + "+)?";
    // (+|-)?(1|2|3|4|5|6|7|8|9|0)+(\.(1|2|3|4|5|6|7|8|9|0)+)?((e|E)(+|-)?(1|2|3|4|5|6|7|8|9|0)++)?
//    RegexTree tree1(number.c_str());
//
//    RegexTree::display_node(tree1.getRoot(), cout);
//
//    auto dt = generate_Dtran(tree1);
//
//    cout << endl;
//    for (int i = 0; i < dt.dtran.size(); i++) {
//        cout << i << ". ";
//        cout << "{";
//        for (int j = 0; j < 256; j++) {
//            if (dt.dtran[i][j] != 0) cout << (char)j << ": " << dt.dtran[i][j] << ", ";
//        }
//        cout << "}" << endl;
//    }
//    cout << "targets: ";
//    for (int k : dt.targets) cout << k << " ";
//
//    cout << "\nresult: " << boolalpha << dt.match("-1.234E20") << endl;
    MatchTable table;
    table.add(id.c_str());
    table.add(integer.c_str());
    table.add(number.c_str());
    table.generate();

    cout << table.match("_func") << endl;
    cout << table.match("234") << endl;
    cout << table.match("23.23e12") << endl;

    return 0;
}