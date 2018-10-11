#ifndef JHLEX_LEXFILE_H
#define JHLEX_LEXFILE_H
#include <string>
#include <vector>
#include <map>
#include "../regex_tree/regex_tree.h"


class LexFile {
private:
    // user defined header code
    std::string headerCode;

    // regex variables
    std::map<std::string, std::string> nameToReg;

    // match tools according to user defined regular expression
    std::vector<MatchTool> tools;

    // user defined code to tackle the lexeme
    std::vector<std::string> lexemeCode;

    // user defined tail code
    std::string tailCode;

    inline void parseHeaderCode(std::istream& is);
    inline void parseRegex(std::istream& is);
    inline void parseLexemeCode(std::istream& is);
    inline void parseTailCode(std::istream& is);

    inline void parsePattern(const std::string& raw, std::string& pattern,
            std::size_t start);

    std::string buffer;
public:
    explicit LexFile(std::istream& is);

};

#endif