#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Symbol{
public:
    string name;
    int address;
};

class Literal{
public:
    string name;
    int address;
};

class AssemblerPass2{
public:
    Symbol symtab[20];
    Literal littab[20];
    int symcount, litcount = 0;

    AssemblerPass2(){
        initializeSymbolTable();
        initializeLiteralTable();
    }

    void initializeSymbolTable(){
        ifstream ins("symbol.txt");
        string line;
        symcount = 0;
        while(getline(ins, line)){
            stringstream ss(line);
            string name;
            int address;
            ss >> name >> address;
            if (name.empty()) continue;
            if (name == "Symbol") continue;
            symtab[symcount].name = name;
            symtab[symcount].address = address;
            symcount++;
        }
        ins.close();
    }

    void initializeLiteralTable(){
        ifstream ins("literal.txt");
        string line;
        litcount = 0; 
        while(getline(ins, line)){
            stringstream ss(line);
            string name;
            int address;
            ss >> name >> address;
            if (name.empty()) continue;
            if (name == "Literal") continue;
            littab[litcount].name = name;
            littab[litcount].address = address;
            litcount++;
        }
        ins.close();
    }

    void process(string input, string output){
        ifstream fin(input);
        ofstream fout(output);
        string line;

        while(getline(fin,line)){
            if (line.empty()) continue;

            if(line.find("(AD") != string::npos) continue;

            stringstream ss(line);
            string token;
            string LC = "", opcode = "00", reg = "0", mem = "000";

            
            if(!(ss >> token)) continue;
            if(token.size() >= 3 && token.front() == '(' && token.back() == ')'){
                LC = token.substr(1, token.length()-2);
            } else {
                continue;
            }

            while (ss >> token){

                if(token.find("(IS") != string::npos){

                    size_t comma = token.find(',');
                    size_t close = token.find(')');
                    if (comma != string::npos && close != string::npos && close > comma+1) {
                        opcode = token.substr(comma+1, close - (comma+1));
                    }
                }
                else if (token.find("(RG") != string::npos){
                    size_t comma = token.find(',');
                    size_t close = token.find(')');
                    if (comma != string::npos && close != string::npos && close > comma+1) {
                        reg = token.substr(comma+1, close - (comma+1));
                    }
                }
                else if (token.find("(S") != string::npos){
                    size_t comma = token.find(',');
                    size_t close = token.find(')');
                    if (comma != string::npos && close != string::npos && close > comma+1) {
                        int index = stoi(token.substr(comma+1, close - (comma+1)));
                        if (index > 0 && index <= symcount){
                            mem = to_string(symtab[index-1].address);
                        } else {
                            mem = "000";
                        }
                    }
                }
                else if (token.find("(L") != string::npos){
                    size_t comma = token.find(',');
                    size_t close = token.find(')');
                    if (comma != string::npos && close != string::npos && close > comma+1) {
                        int index = stoi(token.substr(comma+1, close - (comma+1)));
                        if (index >= 0 && index < litcount){
                            mem = to_string(littab[index].address);
                        } else if (index > 0 && index <= litcount) { 
                            mem = to_string(littab[index-1].address);
                        } else {
                            mem = "000";
                        }
                    }
                }
                else if (token.find("(C") != string::npos){
                    size_t comma = token.find(',');
                    size_t close = token.find(')');
                    if (comma != string::npos && close != string::npos && close > comma+1) {
                        mem = token.substr(comma+1, close - (comma+1));
                    }
                }
                else if (token.find("(DL,02)") != string::npos){
                    opcode = "00";
                    reg = "0";
                }
            }

            fout << LC << "\t" << opcode << "\t" << reg << "\t" << mem << endl;
        }
        fin.close();
        fout.close();
    }
};

int main(){
    AssemblerPass2 assembler;
    assembler.process("intermediate.txt", "machine_code.txt");
    cout << "Pass 2 done. Output in machine_code.txt\n";
    return 0;
}