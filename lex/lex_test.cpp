#include <fstream>
#include "LexFile.h"
#include "OutputFile.h"
using namespace std;

int main() {
    ifstream is("sampleLex.txt");
    LexFile lexFile(is);
    cout << "\n\n";
    OutputFile outputFile(&lexFile);
    outputFile.generateOutputFile(cout);
    return 0;
}