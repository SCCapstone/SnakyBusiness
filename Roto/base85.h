#ifndef BASE85_H
#define BASE85_H


#include <vector>
#include <string>

using std::vector;
using std::string;

class base85
{
public:
    base85();
    string toBase85(unsigned int n, int maxLen = 5);
    unsigned int fromBase85(string s);

private:
    vector <char> baseArr;
};

#endif // BASE85_H
