#include <iostream>
#include <sstream>
#include <string>
#include <strings.h>
#include <map>
#include <vector>
using namespace std;

//Custom class to override std::map's  map::key_compare comparator so that it becomes case insensitive.
class CaseInsensitiveComparator
{
public:

    bool operator()(const std::string& a, const std::string& b) const noexcept
    {
        return ::strcasecmp(a.c_str(), b.c_str()) < 0;
    }

};

template <typename T>
using CaseInsensitiveMap = std::map<std::string, T, CaseInsensitiveComparator>;

CaseInsensitiveMap<int> numbers =
{
    {"one" , 1},
    {"two" , 2},
    {"three" , 3},
    {"four" , 4},
    {"five" , 5},
    {"six" , 6},
    {"seven" , 7},
    {"eight" , 8},
    {"nine" , 9},
    {"ten" , 10},
    {"eleven" , 11},
    {"twelve" , 12},
    {"thirteen" , 13},
    {"fourteen" , 14},
    {"fifteen" , 15},
    {"sixteen" , 16},
    {"seventeen" , 17},
    {"eighteen" , 18},
    {"nineteen" , 19},
    {"twenty" , 20},
    {"thirty" , 30},
    {"forty" , 40},
    {"fifty" , 50},
    {"sixty" , 60},
    {"seventy" , 70},
    {"eighty" , 80},
    {"ninety" , 90},
};

CaseInsensitiveMap<int> multipliers =
{
    {"hundred" , 100},
    {"thousand" , 1000},
    {"million" , 1000000},
    {"billion" , 1000000000}
};

CaseInsensitiveMap<int> hyphenNumbers =
{
    {"twenty" , 20},
    {"thirty" , 30},
    {"forty" , 40},
    {"fifty" , 50},
    {"sixty" , 60},
    {"seventy" , 70},
    {"eighty" , 80},
    {"ninety" , 90}
};

string omitteableChars = ",.;:!?";

vector<string> split(string s, string delimiter)
{
    vector<string> result;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        //cout << token << endl;
        result.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    //cout << s << endl;
    result.push_back(s);
    return result;
}

/*
    TO DO:
    - Treat numbers that end with punctuation: One hundred. -> 100.
    Special cases and their implementation:
    - One -> 1  | one -> 1
    - Seventy-nine -> 79
    - One, two, three. -> One, two, three.
*/
string writtenNumberToDigit(vector<string> tokenizedInput)
{
    string sresult;
    vector<string> vresult;
    int result = 0;
    int partialResult = 0;
    for(size_t i = 0; i < tokenizedInput.size(); i++){
        string token = tokenizedInput.at(i);
        string omitedChar = "";
        //SPECIAL CASE: numbers that end with punctuation
        while(omitteableChars.find(token.back()) != string::npos){
            omitedChar += token.back();
            token.pop_back();
        }
        if(multipliers.find(token) != multipliers.end()){
            //CASE: Multipliers(thousand, billion, hundred)
            if(token == "hundred"){ //SPECIAL CASE: five hundred seventy-seven thousand --> (5*100 + 77) * 1000
                partialResult = partialResult * multipliers[token];
                if(tokenizedInput.at(i+1) == "and"){ //SPECIAL CASE: hundred and ten
                    //Skip to the next one...
                    i++;
                }
            } else {
                result += partialResult * multipliers[token];
                partialResult = 0;
            }
        } else if(numbers.find(token) != numbers.end()){
            //CASE: Numbers (one, eleven, ninety)
            partialResult += numbers[token];
        } else {
            //Possible CASE: Hypen number (Ninety-nine)
            vector<string> hyphenNum = split(token, "-");
            if(hyphenNum.size() == 2){
                if(hyphenNumbers.find(hyphenNum.at(0)) != hyphenNumbers.end() && numbers.find(hyphenNum.at(1)) != numbers.end()){
                    int xnum = hyphenNumbers[hyphenNum.at(0)]; // 90, 80, 70...
                    int ynum = numbers[hyphenNum.at(1)]; // 9, 8, 7, 6...
                    if(ynum >= 10){
                        //CASE: Simple text...
                        sresult += token + " ";
                    } else {
                        //CASE: Hypen number (Ninety-nine)
                        partialResult += xnum + ynum;
                    }
                }
            } else {
                //CASE: Simple text...
                if(result != 0 || partialResult != 0){
                    //We have a number to add before the rest of the text!
                    result += partialResult;
                    vresult.push_back(std::to_string(result));
                    result = 0;
                    partialResult = 0;
                }
                vresult.push_back(token+omitedChar);
            }
        }
        //SPECIAL CASE: numbers that end with punctuation. We have finished translating the number
        if(omitedChar != ""){
            if(result != 0 || partialResult != 0){
                //We have a number to add before the rest of the text!
                result += partialResult;
                vresult.push_back(std::to_string(result)+omitedChar);
                result = 0;
                partialResult = 0;
            }
        }
    }

    //SPECIAL CASE: No text, only numbers: Make sure we have emptied everything...
    if(result != 0 || partialResult != 0){
        //We have a number to add before the rest of the text!
        result += partialResult;
        vresult.push_back(std::to_string(result));
        result = 0;
        partialResult = 0;
    }

    //Convert vector to string!
    for(size_t y=0; y < vresult.size(); y++){
        sresult += vresult[y];
        if(y < vresult.size()-1) sresult += " ";
    }

    return sresult;
}

#define ENABLE_TEST 0

// TEST INPUTS:
map<string, string> tests = {
    {"one hundred and one", "101"},
    {"one thousand one hundred and eleven", "1111"},
    {"one thousand one hundred eleven", "1111"},
    {"One thousand one hundred and eleven", "1111"},
    {"I have one hundred apples", "I have 100 apples"},
    {"This is a very cool number: one billion five hundred sixty-seven million five hundred seventy-nine thousand eight hundred eleven . Very cool, ain't it? :)","This is a very cool number: 1567579811 . Very cool, ain't it? :)"},
    {"Nine hundred and ninety-nine million nine hundred ninety nine thousand nine hundred and ninety nine","999999999"},
    {"A, B, C. It's as simple as one, two, three.","A, B, C. It's as simple as 1, 2, 3."},
    {"His power level is over nine thousand!!!", "His power level is over 9000!!!"},
    {"Give him the good ol' one, two", "Give him the good ol' 1, 2"},
    {"Give him the good ol' 1, 2", "Give him the good ol' 1, 2"},
    {"An influx of nine hundred million bees should put a stop to that!", "An influx of 900000000 bees should put a stop to that!"},
    {"Yes, nine hundred million! That's a lot of bees!", "Yes, 900000000! That's a lot of bees!"},
};

int main()
{
    #if ENABLE_TEST == 1
    map<string, string>::iterator it;
    for (it = tests.begin(); it != tests.end(); it++)
    {
        cout << "TEST: " << it->first << endl;
        vector<string> tokenInput = split(it->first, " ");
        string result = writtenNumberToDigit(tokenInput);
        cout << "RESULT: " << result << endl;
        cout << "OK? " << (result == it->second) << endl;
    }

    #else
    string input, result;
    vector<string> tokenizedInput;

    cout << "Type a sentence: " << endl;
    getline(cin,input);

    tokenizedInput = split(input, " ");

    result = writtenNumberToDigit(tokenizedInput);
    cout << "Your translated sentence is: " << endl;
    cout << result;
    #endif // ENABLE_TEST
    return 0;
}
