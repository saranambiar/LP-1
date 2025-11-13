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
    string params, definition;
};

vector<string> split_comma(const string &s){
    vector<string> result;
    stringstream ss(s);
    string item;

    while(getline(ss, item, ',')){
        result.push_back(item);
    }

    return result;
}

int main(){
    vector<MNTEntry> MNT;
    vector<KPDTEntry> KPDT;
    vector<vector<string>> PNTAB;
    vector<string> MDT;

    ifstream fMNT("MNT.txt");
    string line;
    getline(fMNT, line);
    while(getline(fMNT, line)){
        if (line.empty()) continue;
        stringstream ss(line);
        MNTEntry e;
        int idx;
        ss >> idx >> e.name >> e.mdtPtr >> e.pntPtr >> e.kpdtPtr;
        MNT.push_back(e);
    }
    fMNT.close();

    ifstream fMDT("MDT.txt");
    getline(fMDT, line);
    while(getline(fMDT, line)){
        if (line.empty()) continue;
        stringstream ss(line);
        int idx;
        string instr;
        ss >> idx;
        getline(ss, instr);
        MDT.push_back(instr);
    }
    fMDT.close();

    ifstream fPNT("PNT.txt");
    getline(fPNT, line);
    while(getline(fPNT, line)){
        if (line.empty()) continue;
        stringstream ss(line);
        int idx;
        string param;
        vector<string> params;
        ss >> idx;
        getline(ss, param);
        while(ss >> param) {
            params.push_back(param);
        }
        PNTAB.push_back(params);
    }
    fPNT.close();

    ifstream fKPDT("KPDT.txt");
    getline(fKPDT, line);
    while(getline(fKPDT, line)){
        if (line.empty()) continue;
        stringstream ss(line);
        int idx;
        KPDTEntry e;
        ss >> idx >> e.params >> e.definition;
        KPDT.push_back(e);
    }
    fKPDT.close();

    ifstream fInter("intermediate.txt");
    ofstream fOut("output.txt");

    while((getline(fInter, line))){
        string t = line;
        if(t.empty()) continue;

        stringstream ss(t);
        string word;
        ss >> word;

        bool isMacro = false;
        int midx = -1;
        for(int i=0; i<MNT.size(); ++i){
            if(MNT[i].name == word){
                isMacro = true;
                midx = i;
                break;
            }
        }

        if(!isMacro){
            fOut << t << endl;
        }
        else{
            string rest;
            getline(ss, rest);
            vector<string> args = split_comma(rest);

            auto macro = MNT[midx];
            auto formals = PNTAB[midx];

            unordered_map<string, string> ALA;
            int kpStart = macro.kpdtPtr - 1;
            int kpEnd;
            if (midx + 1 < (int)MNT.size()) {
                kpEnd = MNT[midx + 1].kpdtPtr - 2;
            } else {
                kpEnd = (int)KPDT.size() - 1;
            }

            for(int i = kpStart; i<=kpEnd && i >=0 && i < (int)KPDT.size(); ++i){
                string param = KPDT[i].params;
                string def = KPDT[i].definition;
                ALA[param] = def;
            }

            for(size_t i=0;i<args.size() && i<formals.size();++i){
                ALA[formals[i]] = args[i];
            }

            int mdtp = macro.mdtPtr - 1;
            while(mdtp < (int)MDT.size() && MDT[mdtp] != "MEND"){
                string expline = MDT[mdtp];
                for(auto &p : formals){
                    string key = "&" + p;
                    size_t pos = expline.find(key);
                    if(pos != string::npos && ALA.count(p))
                        expline.replace(pos, key.size(), ALA[p]);
                }
                fOut << expline << endl;
                mdtp++;
            }
        }
    }

    fInter.close();
    fOut.close();
    return 0;
}