//
// Created by jiahua on 2018/10/16.
//

#include "OutputFile.h"
#include "LexFile.h"

OutputFile::OutputFile(LexFile *lexFilePtr): lexFile(lexFilePtr),
    type_count(lexFilePtr->tools.size()) {

}

void OutputFile::generateOutputFile(std::ostream &os) const {
    os << headers << std::endl;

    // print user defined headers
    os << "\n// User defined from lex file\n";
    os << lexFile->headerCode << std::endl;

    // print buffer size
    os << "// Buffer size\n";
    os << "constexpr int jh_buffer_size = " << buffer_size << ';' << std::endl;
    // print type count
    os << "// Lexeme count\n";
    os << "constexpr int jh_type_cnt  = " << type_count << ';' << std::endl;

    // print dtrans
    print_dtran(os);

    // print targets
    print_targets(os);

    // print tackle function
    print_tackle(os);

    // print update vec function
    os << "void jh_update_vec(int vec[jh_type_cnt], char ch) {\n"
          "    for (int i = 0; i < jh_type_cnt; i++) {\n"
          "        vec[i] = jh_dtrans[i][vec[i]][(int)ch];\n"
          "    }\n"
          "}\n" << std::endl;

    os << "int detect_type(const int vec[jh_type_cnt]) {\n"
          "    bool not_zero = false;\n"
          "    for (int i = 0; i < jh_type_cnt; i++) {\n"
          "        int state = vec[i];\n"
          "        if (state != 0) not_zero = true;\n"
          "        if (std::find(jh_target_bucket[i].cbegin(), jh_target_bucket[i].cend(), state)\n"
          "                != jh_target_bucket[i].cend())\n"
          "            return i;\n"
          "    }\n"
          "    if (not_zero)\n"
          "        return -2;\n"
          "    else\n"
          "        return -1;\n"
          "}\n" << std::endl;

    // TODO

}

void OutputFile::print_dtran(std::ostream &os) const {
    // TODO
}

void OutputFile::print_targets(std::ostream &os) const {
    // TODO
}

void OutputFile::print_tackle(std::ostream &os) const {
    // TODO
}
