#include "LexFile.h"

LexFile::LexFile(std::istream &is) {
    // First block of lex file
    parseHeaderCode(is);
    parseRegex(is);

    // Second block of lex file
    parseLexemeCode(is);

    // Third block of lex file
    parseTailCode(is);
}

void LexFile::parseHeaderCode(std::istream &is) {
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty()) continue;
        else break;
    }

    if (line.substr(0, 2) != "%{") { // No Header
        buffer = line;
        return;
    }

    line.erase(0, 2);
    headerCode += line + '\n';

    while (std::getline(is, line)) {
        if (line.empty()) continue;
        if (line.substr(0, 2) == "%}") break;
        headerCode += line + '\n';
    }

    std::cout << headerCode << std::endl;
}

void LexFile::parseRegex(std::istream &is) {
    std::string line;

    if (buffer.empty()) {
        std::getline(is, line);
    } else {
        line = std::move(buffer);
    }

    while (line.substr(0, 2) != "%%") {
        if (line.empty() || line[0] == ' ') {
            if (!getline(is, line)) break;
            continue;
        }
        std::string name, pattern;
        std::size_t delim = line.find(' ', 0);
        name = line.substr(0, delim);
        pattern.reserve(line.size() - delim + 1);
        parsePattern(line, pattern, delim + 1);
        nameToReg[name] = pattern;
        if(!getline(is, line)) break;
    }

    for (const auto& p : nameToReg) {
        std::cout << p.first << ":" << std::endl;
        std::cout << p.second << std::endl;
    }
}

void LexFile::parseLexemeCode(std::istream &is) {
    std::string line;
    bool inBlock = false;
    int braceSt = 0;
    while (std::getline(is, line)) {
        if (line.empty()) continue;
        else break;
    }

    while (line.substr(0, 2) != "%%") {
        if (inBlock) {
            for (char ch : line) {
                if (ch == '{') braceSt++;
                else if (ch == '}') braceSt--;
            }
            lexemeCode.back() += line + '\n';
            if (braceSt == 0) {
                inBlock = false;
                lexemeCode.back().erase(lexemeCode.back().size() - 2, 2);
            }
        } else {
            std::string::size_type i;
            std::string pat;
            if (line.empty()) {
                if (!getline(is, line)) break;
            }
            std::string::size_type blockBegin = line.rfind('{');
            if (blockBegin == std::string::npos) break;
            i = blockBegin - 1;
            for (; line[i] == ' '; --i)
                ;
            parsePattern(line.substr(0, i + 1), pat, 0);
            tools.push_back(generate_Dtran(RegexTree(pat.c_str())));
            lexemeCode.emplace_back();
            inBlock = true;
            braceSt = 1;
        }

        if (!getline(is, line)) break;
    }

    for (const auto& s : lexemeCode) {
        std::cout << s << std::endl;
    }
}

void LexFile::parseTailCode(std::istream &is) {

}

void LexFile::parsePattern(const std::string &raw, std::string &pattern,
        std::size_t start) {
    for (std::size_t i = start; i < raw.size(); i++) {
        char ch = raw[i];
        if (ch == '\\') {
            ++i;
            if (i == raw.size()) break;
            pattern.push_back(raw[i]);
            continue;
        }

        if (ch == '{') {
            std::size_t j = raw.find('}', i);
            std::string variableName = raw.substr(i + 1, j - i - 1);
            auto iter = nameToReg.find(variableName);

            if (iter == nameToReg.end())
                break;
            else
                pattern += iter->second;
            i = j;
        } else {
            pattern.push_back(ch);
        }
    }
}
