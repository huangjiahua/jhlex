#include "regex_tree.h"

RegexTree::RegexTree(const char *str) {
    root = new tree_node;
    root->c = CAT;
    root->right = new tree_node;
    root->right->c = ACCEPT;

    int len = static_cast<int>(strlen(str));
    root->left = build_block(str, len);
    root->left = clean_block(root->left);
}

tree_node *RegexTree::build_block(const char *str, int num) {
    tree_node *curr;
    int i = 0;
    bool escape = false;

    if (str[i] != '\\' && str[i] != '(') {
        curr = new tree_node;
        curr->c = BLOCK;

        curr->left = new tree_node;
        curr->left->c = str[i];
        i++;
    } else if (str[i] == '\\') {
        curr = new tree_node;
        curr->c = BLOCK;

        curr->left = new tree_node;
        curr->left->c = parse_escape(str[i + 1]);
        i += 2;
    } else {
        int blen = block_len(str + 1);
        curr = build_block(str + 1, blen - 1);
        i += blen + 1;
    }


    for (; i < num; i++) {
        char ch = str[i];
        if (str[i] == '\\' && !escape) {
            escape = true;
            continue;
        }

        if (!escape) {
            ch = parse_control(ch);
        } else {
            ch = parse_escape(ch);
        }

        escape = false;
        switch (ch) {
            case STAR: {
                if (isBlock(curr))
                    curr->c = STAR;
                else {
                    tree_node *tmp = curr->right;
                    if (isBlock(tmp))
                        tmp->c = STAR;
                    else {
                        tree_node *star = new tree_node;
                        star->c = STAR;
                        star->left = tmp;
                        curr->right = star;
                    }
                }
                break;
            }
            case QUESTION: {
                tree_node *epsilon = new tree_node;
                epsilon->c = EPSILON;
                if (isBlock(curr)) {
                    curr->c = OR;
                    curr->right = epsilon;
                } else {
                    tree_node *tmp = curr->right;
                    if (isBlock(tmp)) {
                        tmp->c = OR;
                        tmp->right = epsilon;
                    } else {
                        tree_node *_or = new tree_node;
                        _or->c = OR;
                        _or->left = tmp;
                        _or->right = epsilon;
                        curr->right = _or;
                    }
                }
                break;
            }
            case PLUS: {
                if (isBlock(curr)) {
                    curr->c = CAT;

                    tree_node *star = new tree_node;
                    star->c = STAR;
                    star->left = copy_tree(curr->left);
                    curr->right = star;
                } else {
                    tree_node *tmp = curr->right;
                    if (isBlock(tmp)) {
                        tmp->c = CAT;

                        tree_node *star = new tree_node;
                        star->c = STAR;
                        star->left = copy_tree(tmp->left);
                        tmp->right = star;
                    } else {
                        tree_node *cat = new tree_node;
                        cat->c = CAT;

                        tree_node *star = new tree_node;
                        star->c = STAR;

                        cat->right = star;
                        cat->left = tmp;

                        star->left = copy_tree(tmp);
                        curr->right = cat;
                    }
                }
                break;
            }
            case OR: {
                tree_node *right_side;
                if (str[i + 1] != '\\' && str[i + 1] != '(') { // normal char
                    right_side = new tree_node;
                    right_side->c = str[i + 1];
                    i++;
                } else if (str[i + 1] == '\\') { // escape char
                    right_side = new tree_node;
                    right_side->c = parse_escape(str[i + 2]);
                    i += 2;
                } else { // a block
                    int blen = block_len(str + i + 2);
                    right_side = build_block(str + i + 2, blen - 1);
                    i += blen + 1;
                }

                if (isBlock(curr)) {
                    curr->c = OR;
                    curr->right = right_side;
                } else {
                    tree_node *tmp = curr->right;
                    if (isBlock(tmp)) {
                        tmp->c = OR;
                        tmp->right = right_side;
                    } else {
                        tree_node *_or = new tree_node;
                        _or->c = OR;
                        _or->left = tmp;

                        curr->right = _or;
                        _or->right = right_side;
                    }
                }
                break;
            }
            case BLOCK: {
                tree_node *right_side;
                int blen = block_len(str + i + 1);
                right_side = build_block(str + i + 1, blen - 1);
                if (isBlock(curr)) {
                    curr->c = CAT;
                    curr->right = right_side;
                } else {
                    tree_node *cat = new tree_node;
                    cat->c = CAT;
                    cat->left = curr;
                    curr = cat;
                    curr->right = right_side;
                }
                i += blen;
                break;
            }

            case WILDCARD:
            default: {
                if (curr->c == BLOCK)
                    curr->c = CAT;
                else {
                    tree_node *cat = new tree_node;
                    cat->c = CAT;
                    cat->left = curr;
                    curr = cat;
                }
                curr->right = new tree_node;
                curr->right->c = ch;
                break;
            }
        }
    }

    tree_node *block = new tree_node;
    block->c = BLOCK;
    block->left = curr;
    return block;
}

RegexTree::~RegexTree() {
    destruct(root);
}

void RegexTree::destruct(tree_node *ptr) {
    if (ptr == nullptr) return;
    destruct(ptr->left);
    destruct(ptr->right);
    delete ptr;
}

tree_node *RegexTree::copy_tree(tree_node *src) {
    if (src == nullptr) return nullptr;
    tree_node *ret = new tree_node;
    ret->c = src->c;
    ret->left = copy_tree(src->left);
    ret->right = copy_tree(src->right);
    return ret;
}

bool RegexTree::isBlock(tree_node *ptr) {
    return ptr != nullptr && ptr->c == BLOCK;
}

char RegexTree::parse_escape(char c) {
    switch (c) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        default:
            return c;
    }
}

int RegexTree::block_len(const char *start) {
    int stack = 1;
    for (int i = 0; start[i] != '\0'; i++) {
        if (start[i] == '(') ++stack;
        if (start[i] == ')') --stack;
        if (stack == 0) return i + 1;
    }
    return -1;
}

char RegexTree::parse_control(char ch) {
    switch (ch) {
        case '*':
            return STAR;
        case '+':
            return PLUS;
        case '?':
            return QUESTION;
        case '(':
            return BLOCK;
        case '|':
            return OR;
        case '.':
            return WILDCARD;
        default:
            return ch;
    }
}

char RegexTree::show_control(char c) {
    switch (c) {
        case STAR:
            return '*';
        case CAT:
            return '~';
        case OR:
            return '|';
        case WILDCARD:
            return '.';
        case BLOCK:
            return 'B';
        case EPSILON:
            return 'e';
        default:
            return c;
    }
}

void RegexTree::display_tree(std::ostream &os) {
    display_node(root->left, os);
}

void RegexTree::display_node(const tree_node *ptr, std::ostream &os) {
    if (ptr == nullptr)
        return;
    else if (ptr->left == nullptr && ptr->right == nullptr) {
        os << show_control(ptr->c);
        return;
    }

    char toshow = show_control(ptr->c);
    os << toshow << "(";
    display_node(ptr->left, os);
    os << ", ";
    display_node(ptr->right, os);
    os << ")";
}

tree_node *RegexTree::clean_block(tree_node *ptr) {
    if (ptr == nullptr)
        return nullptr;
    ptr->left = clean_block(ptr->left);
    ptr->right = clean_block(ptr->right);

    if (ptr->c == BLOCK) {
        tree_node *ret = ptr->left;
        delete ptr;
        return ret;
    } else
        return ptr;
}

const tree_node *RegexTree::getRoot() const {
    return root;
}

void RegexTree::number_node(std::unordered_map<const tree_node *, int> &m) const {
    number_node(m, root);
}

int RegexTree::number_node(std::unordered_map<const tree_node *, int> &m, tree_node *ptr, int i) {
    if (ptr->left != nullptr) {
        i = number_node(m, ptr->left, i);
    }
    if (ptr->right != nullptr) {
        i = number_node(m, ptr->right, i);
    }
    if (ptr->right == nullptr && ptr->left == nullptr) {
        m.insert({ptr, i});
        return i + 1;
    }
    return i;
}

bool RegexTree::nullable(const tree_node *ptr) {
    if (ptr->c == EPSILON) return true;

    if (ptr->left == nullptr && ptr->right == nullptr) return false;
    if (ptr->c == OR)
        return nullable(ptr->left) || nullable(ptr->right);
    if (ptr->c == CAT)
        return nullable(ptr->left) && nullable(ptr->right);
    return ptr->c == STAR;
}

bool RegexTree::nullable() const {
    return nullable(root);
}

RegexTree::StateSet RegexTree::firstpos() const {
    StateSet s;
    firstpos(root, s);
    return std::move(s);
}

void RegexTree::firstpos(const tree_node *ptr, RegexTree::StateSet &s) {
    if (ptr == nullptr || ptr->c == EPSILON) return;
    if (ptr->left == nullptr && ptr->right == nullptr)
        s.insert(ptr);
    if (ptr->c == OR) {
        firstpos(ptr->left, s);
        firstpos(ptr->right, s);
        return;
    }
    if (ptr->c == CAT) {
        if (nullable(ptr->left)) {
            firstpos(ptr->left, s);
            firstpos(ptr->right, s);
        } else
            firstpos(ptr->left, s);
        return;
    }
    if (ptr->c == STAR)
        firstpos(ptr->left, s);
}

RegexTree::StateSet RegexTree::lastpos() const {
    StateSet st;
    lastpos(root, st);
    return std::move(st);
}

void RegexTree::lastpos(const tree_node *ptr, RegexTree::StateSet &s) {
    if (ptr == nullptr || ptr->c == EPSILON) return;
    if (ptr->left == nullptr && ptr->right == nullptr)
        s.insert(ptr);
    if (ptr->c == OR) {
        lastpos(ptr->left, s);
        lastpos(ptr->right, s);
        return;
    }
    if (ptr->c == CAT) {
        if (nullable(ptr->right)) {
            lastpos(ptr->left, s);
            lastpos(ptr->right, s);
        } else
            lastpos(ptr->right, s);
        return;
    }
    if (ptr->c == STAR)
        lastpos(ptr->left, s);
}

RegexTree::FollowPos RegexTree::get_followpos(const StateMap &mp) const {
    FollowPos ret(mp.size() + 1, std::vector<int>());
    generate_followpos(mp, ret, root);
    return std::move(ret);
}

void RegexTree::generate_followpos(const RegexTree::StateMap &mp,
                                   RegexTree::FollowPos &fp, const tree_node *ptr) {
    if (no_child(ptr)) return;
    if (ptr->left != nullptr)
        generate_followpos(mp, fp, ptr->left);
    if (ptr->right != nullptr)
        generate_followpos(mp, fp, ptr->right);
    if (ptr->c == CAT || ptr->c == STAR) {
        StateSet c1, c2;
        lastpos(ptr->left, c1);
        if (ptr->c == CAT)
            firstpos(ptr->right, c2);
        else
            firstpos(ptr->left, c2);
        for (const tree_node *p : c1) {
            for (const tree_node *q : c2)
                fp[mp.at(p)].push_back(mp.at(q));
        }
    }
}

bool RegexTree::no_child(const tree_node *ptr) {
    return (ptr->left == nullptr && ptr->right == nullptr);
}

void RegexTree::or_link(RegexTree &rhs) {
    tree_node *_or = new tree_node;
    _or->c = OR;
    _or->right = root;
    _or->left = rhs.root;
    rhs.root = nullptr;
    root = _or;
}

void RegexTree::or_link(RegexTree &&rhs) {
    tree_node *_or = new tree_node;
    _or->c = OR;
    _or->right = root;
    _or->left = rhs.root;
    rhs.root = nullptr;
    root = _or;
}

int RegexTree::size() const {
    return get_size(root);
}

int RegexTree::get_size(const tree_node *ptr) {
    if (ptr == nullptr) return 0;
    if (ptr->left == nullptr && ptr->right == nullptr) return 1;
    return get_size(ptr->left) + get_size(ptr->right);
}

MatchTool generate_Dtran(const RegexTree& tree) {
    std::vector<Dstate> Dstates(1, Dstate());
    std::vector<std::vector<int>> Dtran;
    std::vector<int> targets;
    RegexTree::StateMap mp;
    tree.number_node(mp);
    std::vector<const tree_node *> table(mp.size() + 1);
    for (const auto &p : mp)
        table[p.second] = p.first;
    std::vector<int> firstpos;
    RegexTree::FollowPos fp = tree.get_followpos(mp);
    int is_marked = 0;
    int curr = 0;

    RegexTree::StateSet first_set = tree.firstpos();
    for (const tree_node *p : first_set) firstpos.push_back(mp.at(p));
    std::sort(firstpos.begin(), firstpos.end());
    Dstates.front().states = firstpos;
    if (std::find(Dstates.front().states.cbegin(),
            Dstates.front().states.cend(), mp.size()) != Dstates.front().states.cend())
        targets.push_back(1);

    while (is_marked < Dstates.size()) {
        for (int i = 0; i < Dstates.size(); i++)
            if (Dstates[i].mark == -1) {
                curr = i;
                break;
            }
        ++is_marked;
        Dstates[curr].mark = is_marked; // mark this state

        bool alphabet[256];
        for (bool &i : alphabet)
            i = false;
        alphabet[ACCEPT] = true;

        for (int state : Dstates[curr].states) {
            char ch = table[state]->c;
            if (alphabet[ch])
                continue;
            else
                alphabet[ch] = true;

            std::vector<int> tmp;
            for (int state2 : Dstates[curr].states) {
                if (table[state2]->c == ch)
                    for (int j : fp[state2]) {
                        if (std::find(tmp.begin(), tmp.end(), j) == tmp.end())
                            tmp.push_back(j);
                    }
            }
            std::sort(tmp.begin(), tmp.end());

            auto iter = std::find_if(Dstates.begin(), Dstates.end(), [&tmp](Dstate &s) {
                return s.states == tmp;
            });
            int state_num = static_cast<int>(iter - Dstates.begin()) + 1;
            if (iter == Dstates.end()) {
                Dstates.emplace_back();
                if (std::find(tmp.cbegin(), tmp.cend(), mp.size()) != tmp.cend())
                    targets.push_back(static_cast<int>(Dstates.size()));
                Dstates.back().states = std::move(tmp);
            }
            if (Dtran.size() < is_marked + 1)
                Dtran.resize(static_cast<unsigned long>(is_marked + 1) // NOLINT
                        , std::vector<int>(256, 0));
            Dtran[is_marked][ch] = state_num;
        }
    }
    return MatchTool(std::move(Dtran), std::move(targets));
}

void MatchTable::add(const char *reg) {
    regs.push_back(reg);
}

int MatchTable::get_index(const char *reg) const {
    return 0;
}

void MatchTable::generate() {
    for (const char* str : regs) {
        tools.push_back(generate_Dtran(RegexTree(str)));
    }
}

int MatchTable::match(const char *txt) const {
    std::vector<int> curr_vec(regs.size(), 1);
    for (int i = 0; txt[i] != '\0'; i++) {
        int ch = (int)txt[i];
        for (int j = 0; j < curr_vec.size(); j++) {
            curr_vec[j] = tools[j].dtran[curr_vec[j]][ch];
        }
    }
    for (int i = 0; i < curr_vec.size(); i++) {
        int state = curr_vec[i];
        if (state == 0) continue;
        std::vector<int>::const_iterator first = tools[i].targets.cbegin(),
                f,
                last = tools[i].targets.cend();
        if ((f = std::find(first, last, state)) != last) {
            return i;
        }
    }
    return -1;
}

const MatchTool &MatchTable::getTool(int i) const {
    return tools[i];
}


MatchTool::MatchTool(std::vector<std::vector<int>> &&tran, std::vector<int> &&tar):
    dtran(tran), targets(tar) { }

bool MatchTool::match(const char *txt) {
    int curr = 1;
    for (int i = 0; txt[i] != '\0'; i++) {
        curr = dtran[curr][(unsigned long)txt[i]];
        if (curr == 0) return false;
    }
    return std::find(targets.cbegin(), targets.cend(), curr) != targets.cend();
}
