//
// Created by jiahua on 2018/10/16.
//

#ifndef JHLEX_OUTPUTFILE_H
#define JHLEX_OUTPUTFILE_H
#include "lex.h"
#include <sstream>


class OutputFile {
private:
    // the lexfile class
    LexFile *lexFile;

    // header including
    std::string headers = "#include <iostream>\n#include <vector>\n#include <cstdio>\n#include <algorithm>";

    // input buffer size
    int buffer_size = 4097;

    // the count of all lexeme types
    int type_count;


    // print dtran
    void print_dtran(std::ostream &os) const;

    // print targets
    void print_targets(std::ostream& os) const;

    // print tackling function
    void print_tackle(std::ostream& os) const;



public:
    OutputFile(LexFile *lexFilePtr);
    void generateOutputFile(std::ostream &os) const;
};


#endif //JHLEX_OUTPUTFILE_H
