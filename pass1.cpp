#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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

class AssemblerPass1{
    string opcodeTable[18][3];
    string regTable[4][2];
    Symbol symtab[20];
    Literal littab[20];
    int pooltab[10];
    int symcount, litcount, poolcount = 0;
    int LC;

    public:

    AssemblerPass1(){
        initializeOpcodeTable();
        initializeRegTable();
        symcount = 0;
        litcount = 0;
        pooltab[poolcount] = 0;
        poolcount = 1;
        LC = 0;
    }

    void initializeOpcodeTable() {
    opcodeTable[0][0] = "START";   opcodeTable[0][1] = "AD"; opcodeTable[0][2] = "01";
    opcodeTable[1][0] = "END";     opcodeTable[1][1] = "AD"; opcodeTable[1][2] = "02";
    opcodeTable[2][0] = "ORIGIN";  opcodeTable[2][1] = "AD"; opcodeTable[2][2] = "03";
    opcodeTable[3][0] = "EQU";     opcodeTable[3][1] = "AD"; opcodeTable[3][2] = "04";
    opcodeTable[4][0] = "LTORG";   opcodeTable[4][1] = "AD"; opcodeTable[4][2] = "05";

    opcodeTable[5][0] = "STOP";    opcodeTable[5][1] = "IS"; opcodeTable[5][2] = "00";
    opcodeTable[6][0] = "ADD";     opcodeTable[6][1] = "IS"; opcodeTable[6][2] = "01";
    opcodeTable[7][0] = "SUB";     opcodeTable[7][1] = "IS"; opcodeTable[7][2] = "02";
    opcodeTable[8][0] = "MULT";    opcodeTable[8][1] = "IS"; opcodeTable[8][2] = "03";
    opcodeTable[9][0]  = "MOVER";  opcodeTable[9][1] = "IS"; opcodeTable[9][2] = "04";
    opcodeTable[10][0] = "MOVEM";  opcodeTable[10][1] = "IS"; opcodeTable[10][2] = "05";
    opcodeTable[11][0] = "COMP";   opcodeTable[11][1] = "IS"; opcodeTable[11][2] = "06";
    opcodeTable[12][0] = "BC";     opcodeTable[12][1] = "IS"; opcodeTable[12][2] = "07";
    opcodeTable[13][0] = "DIV";    opcodeTable[13][1] = "IS"; opcodeTable[13][2] = "08";
    opcodeTable[14][0] = "READ";   opcodeTable[14][1] = "IS"; opcodeTable[14][2] = "09";
    opcodeTable[15][0] = "PRINT";  opcodeTable[15][1] = "IS"; opcodeTable[15][2] = "10";

    opcodeTable[16][0] = "DS";     opcodeTable[16][1] = "DL"; opcodeTable[16][2] = "01";
    opcodeTable[17][0] = "DC";     opcodeTable[17][1] = "DL"; opcodeTable[17][2] = "02";
}


    void initializeRegTable(){
        regTable[0][0] = "AREG", regTable[0][1] = "1";
        regTable[1][0] = "BREG", regTable[1][1] = "2";
        regTable[2][0] = "CREG", regTable[2][1] = "3";
        regTable[3][0] = "DREG", regTable[3][1] = "4";
    };

    string getOpClass(string code){
        for (int i = 0; i < 18; i++){
            if(opcodeTable[i][0] == code){
                return opcodeTable[i][1];
            }
        }
        return "";
    }

    string getOpcode(string code){
        for (int i = 0; i < 18; i++){
            if(opcodeTable[i][0] == code){
                return opcodeTable[i][2];
            }
        }
        return "";
    }

    string getRegcode(string reg){
        for (int i = 0; i < 4; i++){
            if (regTable[i][0] == reg){
                return regTable[i][1];
            }
        }
        return "00";
    }

    void addSymbol(string name, int address){
        for (int i = 0; i < symcount; i++){
            if(symtab[i].name == name){
                symtab[i].address = address;
                return;
            }
        }

        symtab[symcount].name = name;
        symtab[symcount].address = address;
        symcount++;
    }

    void addLiteral(string name, int address){
        for (int i = 0; i < litcount; i++){
            if(littab[i].name == name){
                return;
            }
        }
        littab[litcount].name = name;
        littab[litcount].address = address;
        litcount++;
    }

    bool isOpcode(string word){
        for (int i = 0; i<10; i++){
            if (opcodeTable[i][0] == word)
                return true;
        }
        return false;
    }

    void assignLiterals(ofstream &out){
        out << "(AD, 03) " << " (DL,02) " << endl;
        for (int i = pooltab[poolcount -1]; i < litcount; i++){
            if (littab[i].address == -1){
                littab[i].address = LC;

                out << "(" << LC << ") ";
                out << "(C," << littab[i].name.substr(2, littab[i].name.length()-3) << ")" << endl;
                LC++;
            }
        }
        pooltab[poolcount] = litcount;
        poolcount++;
    }

    void processLine(string line, ofstream &out){
        if (line.empty()) return;

        string label, opcode, op1, op2;
        stringstream ss(line);
        ss >> label;

        if (isOpcode(label)){
            opcode = label;
            label = "";
        }
        else{
            ss >> opcode;
        }

        ss >> op1 >> op2;

        if (opcode == "START"){
            LC = stoi(op1);

            if(!label.empty()){
                addSymbol(label, LC);
                out << "(S," << (symcount-1) << ") ";
            }

            out << "(AD," << getOpcode(opcode) << ") (C," << LC << ")" << endl;
        }

        if(opcode == "ADD" || opcode == "SUB" || opcode == "MULT" || opcode == "MOVER" || opcode == "MOVEM"){
            string regcode = getRegcode(op1.substr(0, op1.length() -1));

            out << "(" << LC << ") ";
            if(!label.empty()) {
                addSymbol(label, LC);
                out << "(S," << (symcount - 1) << ") ";
            }

            if(op2[0] == '=') {
                addLiteral(op2, -1);
            } else {
                addSymbol(op2, -1);
            }

            out << "(IS," << getOpcode(opcode) << ") "
                << "(RG," << regcode << ")";
            
            if (op2[0] == '=')
                out << " (L," << (litcount - 1) << ")";
            else
                out << " (S," << (symcount - 1) << ")";
            out << "\n";
            LC++;

        }

        else if(opcode == "LTORG"){
            assignLiterals(out);
        }

        else if(opcode == "END"){
            assignLiterals(out);
        }

        else if(opcode == "DC"){
            addSymbol(label, LC);
            out << "(" << LC << ") ";

            if(!label.empty()){
                addSymbol(label, LC);
                out << "(S," << (symcount-1) << ") ";
            }
            out << "(DL," << getOpcode(opcode) << ") " << "(C," << op1 << ")" << endl;
            LC++;
        }

        else if(opcode == "DS"){
            addSymbol(label, LC);
            out << "(" << LC << ") ";
            
            if(!label.empty()) {
                addSymbol(label, LC);
                out << "(S," << (symcount - 1) << ") ";
            }

            out << "(DL," << getOpcode(opcode) << ") "
                << "(C," << op1 << ")\n";
            LC += stoi(op1);
        }
    }

    void displayTables(){
        ofstream outs("symbol.txt");
        outs << "\nSymbol Table:" << endl;
        for (int i = 0; i < symcount; i++){
            outs << symtab[i].name << "\t" << symtab[i].address << endl;
        }

        ofstream outl("literal.txt");
        outl << "\nLiteral Table:" << endl;
        for (int i = 0; i < litcount; i++){
            outl << littab[i].name << "\t" << littab[i].address << endl;
        }

        ofstream outp("pool.txt");
        outp << "\nPool Table:\n";
        for(int i = 0 ; i < poolcount ; i++) {
            outp << pooltab[i] << "\n";
        }
    }

    void assemble(string inputFile, string outputFile){
        ifstream in(inputFile);
        ofstream out(outputFile);
        string line;
        while(getline(in, line)) {
            processLine(line, out);
        }

        displayTables();

        in.close();
        out.close();
    }
};

int main() {
    AssemblerPass1 assembler;
    assembler.assemble("example1.asm", "intermediate.txt");
    return 0;
}