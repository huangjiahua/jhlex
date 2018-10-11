#include <fstream>
#include "LexFile.h"
using namespace std;

int main() {
    ifstream is("sampleLex.txt");
    LexFile lexFile(is);
    return 0;
}