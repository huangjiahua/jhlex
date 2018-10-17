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
    os << "constexpr int jh_buffer_sz = " << buffer_size << ';' << std::endl;
    // print type count
    os << "// Lexeme count\n";
    os << "constexpr int jh_type_cnt  = " << type_count << ';' << std::endl;
    os << "\nFILE *jh_in = stdin;\nchar jh_buffer[jh_buffer_sz]\n";

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

    // print type detecting function
    os << "int jh_detect_type(const int vec[jh_type_cnt]) {\n"
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


    // print the lexeme element extracting function
    os << "char *jh_get_lexeme(char *b, int *type) {\n"
          "    int curr_vec[jh_type_cnt];\n"
          "    char *f = b, *old_f = b;\n"
          "    int old_type = -1, new_type = -1;\n"
          "    for (int &i : curr_vec)\n"
          "        i = 1;\n"
          "    while(*f != '\\0') {\n"
          "        jh_update_vec(curr_vec, *f);\n"
          "        new_type = jh_detect_type(curr_vec);\n"
          "        if (new_type >= 0) {\n"
          "            old_type = new_type;\n"
          "            old_f = f;\n"
          "        } else if (new_type == -1) {\n"
          "            *type = old_type;\n"
          "            return old_f + 1;\n"
          "        }\n"
          "        ++f;\n"
          "    }\n"
          "    *type = old_type;\n"
          "    return old_f + 1;\n"
          "}" << std::endl;

    // print the lex function
    os << "int jh_lex() {\n"
          "    int type = -1;\n"
          "    if (!fread(jh_buffer, 1, jh_buffer_sz, jh_in))\n"
          "        return -1;\n"
          "    char *lexemeBegin = jh_buffer;\n"
          "    char *forward;\n"
          "\n"
          "    while (*lexemeBegin != '\\0') {\n"
          "        forward = jh_get_lexeme(lexemeBegin, &type);\n"
          "        if (forward == lexemeBegin) return -2;\n"
          "        jh_tackle(lexemeBegin, (int)(forward - lexemeBegin), type);\n"
          "        lexemeBegin = forward;\n"
          "    }\n"
          "    return 0;\n"
          "}" << std::endl;

    os << "\n// User defined code" << std::endl;
    os << lexFile->tailCode << std::endl;
}

void OutputFile::print_dtran(std::ostream &os) const {
    os << "\nstd::vector<std::vector<std::vector<int>>> jh_dtrans = {" << std::endl;
    for (int j = 0; j < lexFile->tools.size(); j++) {
        MatchTool &matchTool = lexFile->tools[j];
        os << "    {\n";
        for (int k = 0; k < matchTool.dtran.size(); k++) {
            std::vector<int> &v = matchTool.dtran[k];
            os << "        {";
            for (int i = 0; i < v.size() - 1; i++) {
                os << v[i] << ", ";
                if (i != 0 && i % 32 == 0)
                    os << "\n         ";
            }
            os << v.back() << "}";
            if (k != matchTool.dtran.size() - 1) os << ',';
            os << std::endl;
        }
        os << "    }";
        if (j != lexFile->tools.size() - 1) os << ',';
        os << std::endl;
    }
    os << "};" << std::endl;
}

void OutputFile::print_targets(std::ostream &os) const {
    os << "std::vector<std::vector<int>> jh_target_bucket = {" << std::endl;
    for (int j = 0; j < lexFile->tools.size(); j++) {
        MatchTool &matchTool = lexFile->tools[j];
        os << "    {";
        for (int i = 0; i < matchTool.targets.size() - 1; i++)
            os << matchTool.targets[i] << ", ";
        os << matchTool.targets.back() << "}";
        if (j != lexFile->tools.size() - 1)
            os << ",";
        os << "\n";
    }
    os << "};\n" << std::endl;
}

void OutputFile::print_tackle(std::ostream &os) const {
    os << "void jh_tackle(char *jh_text, int jh_size, int type) {" << std::endl;
    os << "    switch (type) {\n";
    for (int i = 0; i < lexFile->lexemeCode.size(); i++) {
        std::string line;
        std::istringstream iss(lexFile->lexemeCode[i]);
        os << "        case " << i << ": {\n";
        while (getline(iss, line))
            os << "        " << line << '\n';
        os << "        }\n"
           << "        break;\n";
    }
    os << "        default: break;\n";
    os << "    }\n";
    os << "}\n" << std::endl;
}
