#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class MNTEntry{
    public:
    string name;
    int mdtPtr, pntPtr, kpdtPtr;
};

class KPDTEntry{
    public:
    string param, definition;
};

vector <string> split_comma(const string &s){
    vector<string> result;
    stringstream ss(s);
    string item;

    while(getline(ss, item, ',')){
        result.push_back(item);
    }

    return result;
}

int main(){
    ifstream fin("source.asm");
    ofstream fmdt("MDT.txt"), fpnt("PNT.txt"), fmnt("MNT.txt"), fkpdt("KPDT.txt"), finter("intermediate.txt");

    vector<MNTEntry> MNT;
    vector<KPDTEntry> KPDT;
    vector<vector<string>> PNT;
    vector<string> MDT;

    string line;
    while(getline(fin,line)){
        string t = line;

        if (t == "MACRO"){
            getline(fin,line);
            stringstream ss(line);

            MNTEntry m;
            ss >> m.name;
            m.mdtPtr = (int)MDT.size()+1;
            m.pntPtr = (int)PNT.size()+1;
            m.kpdtPtr = (int)KPDT.size()+1;

            MNT.push_back(m);

            PNT.emplace_back();
            auto &params = PNT.back();
            string rest;
            getline(ss,rest);

            if(!rest.empty()){
                vector<string> parts = split_comma(rest);
                for(string part : parts){
                    if(part[0] == '&'){
                        part = part.substr(1);
                    }
                    if(part.find('=') != string::npos){
                        params.push_back(part);
                    }
                    else{
                        size_t eqPos = part.find('=');
                        string paramName = part.substr(0,eqPos);
                        string defaultValue = part.substr(eqPos+1);
                        KPDT.push_back({paramName, defaultValue});
                        params.push_back(paramName);
                    }
                }
            }

            while (getline(fin,line)){
                if(line == "MEND"){
                    break;
                }
                MDT.push_back(line);
            }
        }

        else{
            finter << line << endl;
        }
    }

    fmnt << "INDEX\tNAME\tMDT_PTR\tPNT_PTR\tKPDT_PTR\n";
    for(size_t i=0;i<MNT.size();++i){
        fmnt << (i+1) << "\t" << MNT[i].name << "\t" << MNT[i].mdtPtr << "\t" << MNT[i].pntPtr << "\t" << MNT[i].kpdtPtr << "\n";
    }

    fmdt << "INDEX\tINSTRUCTION\n";
    for(size_t i=0;i<MDT.size();++i) fmdt << (i+1) << "\t" << MDT[i] << "\n";

    fpnt << "MACRO\tPARAMETERS\n";
    for(size_t i=0;i<PNT.size();++i){
        fpnt << (i+1) << "\t";
        for(size_t j=0;j<PNT[i].size();++j){
            if(j) fpnt << " ";
            fpnt << PNT[i][j];
        }
        fpnt << "\n";
    }

    fkpdt << "INDEX\tPARAM\tDEFAULT\n";
    for(size_t i=0;i<KPDT.size();++i) fkpdt << (i+1) << "\t" << KPDT[i].param << "\t" << KPDT[i].definition << "\n";

    fin.close(); fmnt.close(); fmdt.close(); fpnt.close(); fkpdt.close(); finter.close();
    return 0;
}