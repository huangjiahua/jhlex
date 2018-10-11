#ifndef JHLEX_REGEX_TREE_H
#define JHLEX_REGEX_TREE_H

#include <cstring>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iterator>

const char printable[] = {
        ' ', '!', '\"', '#', '$', '%', '&', '\'', '(', ')',
        '*', '+', ',', '-', '.', '/', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
        '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
        'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
        'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[',
        '\\', ']', '^', '_', '`', 'a', 'b', 'c', 'd', 'e',
        'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
        'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
        'z', '{', '|', '}', '~'
};

const char EPSILON = 7;
const char CAT = 8;
const char STAR = 9;
const char OR = 10;
const char PLUS = 11;
const char QUESTION = 12;
const char ACCEPT = 15;
const char WILDCARD = 16;
const char BLOCK = 17;

struct tree_node {
    tree_node *left = nullptr;
    tree_node *right = nullptr;
    char c = 0;
};

class RegexTree {
public:
    typedef std::unordered_set<const tree_node *> StateSet;
    typedef std::unordered_map<const tree_node *, int> StateMap;
    typedef std::vector<std::vector<int>> FollowPos;

    explicit RegexTree(const char *str);

    ~RegexTree();

    void display_tree(std::ostream &os);

    const tree_node *getRoot() const;

    void number_node(std::unordered_map<const tree_node *, int> &m) const;

    static void display_node(const tree_node *ptr, std::ostream &os);

    bool nullable() const;

    static bool nullable(const tree_node *ptr);

    StateSet firstpos() const;

    static void firstpos(const tree_node *ptr, StateSet &s);

    StateSet lastpos() const;

    static void lastpos(const tree_node *ptr, StateSet &s);

    FollowPos get_followpos(const StateMap &mp) const;

    void or_link(RegexTree &rhs);

    void or_link(RegexTree &&rhs);

    int size() const;

private:
    tree_node *root;

    tree_node *build_block(const char *str, int num);

    tree_node *copy_tree(tree_node *src);

    bool isBlock(tree_node *ptr);

    void destruct(tree_node *ptr);

    int block_len(const char *start);

    static char parse_control(char c);

    static char parse_escape(char c);

    static char show_control(char c);

    tree_node *clean_block(tree_node *ptr);

    static int number_node(std::unordered_map<const tree_node *, int> &m, tree_node *ptr, int i = 1);

    static void generate_followpos(const StateMap &mp, FollowPos &fp, const tree_node *ptr);

    static bool no_child(const tree_node *ptr);

    static int get_size(const tree_node *ptr);
};

struct Dstate {
    int mark = -1;
    std::vector<int> states = std::vector<int>();
};

struct MatchTool {
    std::vector<std::vector<int>> dtran;
    std::vector<int> targets;
    MatchTool(std::vector<std::vector<int>>&& tran, std::vector<int>&& tar);
    bool match(const char *txt);
};

MatchTool generate_Dtran(const RegexTree& tree);

class MatchTable {
private:
    std::vector<const char*> regs;
    std::vector<MatchTool> tools;
public:
    MatchTable() = default;

    void add(const char* reg);

    int get_index(const char* reg) const;

    void generate();

    int match(const char* txt) const;
    
    const MatchTool& getTool(int i) const;
};

#endif