#include <iostream>
#include <vector>
using namespace std;

void displayTable(const vector<int>& initialBlocks, const vector<int>& allocation, const vector<int>& processes){
    cout << "\nBlock No.\tBlock Size\tAllocated Process" << endl;
    for(int i = 0; i < initialBlocks.size(); i++){
        bool isAllocated = false;
        int allocatedProcessSize = 0;

        for(int j = 0; j < allocation.size(); j++){
            if(allocation[j] == i){
                allocatedProcessSize = processes[j];
                isAllocated = true;
                break;
            }
        }

        cout << (i+1) << "\t\t" << initialBlocks[i] << "\t\t";
        if (isAllocated){
            cout << allocatedProcessSize;
        }
        else{
            cout << "Free";
        }
        cout << endl;
    }
}

void displayWaitingQueue(const vector<int>& allocation, const vector<int>& processes){
    cout << "Unallocated processes:";
    bool anyWaiting = false;
    for (int i = 0; i < processes.size(); i++){
        if(allocation[i] == -1){
            cout << " P" << (i+1) << ": " << processes[i];
            anyWaiting = true;
        }
    }
    if(!anyWaiting){
        cout << " None";
    }
    cout << endl;
}

void firstFit(vector<int>& blocks, vector<int>& processes){
    cout << "\nFirst Fit Allocation:" << endl;
    vector<int> allocation(processes.size(), -1);
    vector<int> initialBlocks = blocks;
    vector<bool> blocksUsed(blocks.size(), false);

    for(int i = 0; i < processes.size(); i++){
        for(int j = 0; j < blocks.size(); j++){
            if(!blocksUsed[j] && blocks[j] >= processes[i]){
                allocation[i] = j;
                blocksUsed[j] = true;
                break;
            }
        }
    }

    displayTable(initialBlocks, allocation, processes);
    displayWaitingQueue(allocation, processes);
}

void bestFit(vector<int>& blocks, vector<int>& processes){
    cout << "\nBest Fit Allocation:" << endl;
    vector<int> allocation(processes.size(), -1);
    vector<int> initialBlocks = blocks;
    vector<bool> blocksUsed(blocks.size(), false);

    for(int i = 0; i < processes.size(); i++){
        int bestIndex = -1;
        for(int j = 0; j < blocks.size(); j++){
            if(!blocksUsed[j] && blocks[j] >= processes[i]){
                if (bestIndex == -1 || blocks[j] < blocks[bestIndex]){
                    bestIndex = j;
                }
            }
        }
        if(bestIndex != -1){
            allocation[i] = bestIndex;
            blocksUsed[bestIndex] = true;
        }
    }

    displayTable(initialBlocks, allocation, processes);
    displayWaitingQueue(allocation, processes);
}

void worstFit(vector<int>& blocks, vector<int>& processes){
    cout << "\nWorst Fit Allocation:" << endl;
    vector<int> allocation(processes.size(), -1);
    vector<int> initialBlocks = blocks;
    vector<bool> blocksUsed(blocks.size(), false);

    for(int i = 0; i < processes.size(); i++){
        int worstIndex = -1;
        for(int j = 0; j < blocks.size(); j++){
            if(!blocksUsed[j] && blocks[j] >= processes[i]){
                if (worstIndex == -1 || blocks[j] > blocks[worstIndex]){
                    worstIndex = j;
                }
            }
        }
        if(worstIndex != -1){
            allocation[i] = worstIndex;
            blocksUsed[worstIndex] = true;
        }
    }

    displayTable(initialBlocks, allocation, processes);
    displayWaitingQueue(allocation, processes);
}

void nextFit(vector<int>& blocks, vector<int>& processes){
    cout <<"\nNext Fit Allocation:" << endl;
    vector<int> allocation(processes.size(),-1);
    vector<int> initialBlocks = blocks;
    vector<bool> blocksUsed(blocks.size(), false);
    int lastAllocatedIndex = 0;
    int n = blocks.size();

    for(int i = 0; i < processes.size();i++){
        int count = 0;
        int j = lastAllocatedIndex;

        while(count<n){
            if(!blocksUsed[j] && blocks[j] >= processes[i]){
                allocation[i] = j;
                blocksUsed[j] = true;
                lastAllocatedIndex = j;
                break;
            }
            j = (j+1)%n;
            count++;
        }
    }

    displayTable(initialBlocks, allocation, processes);
    displayWaitingQueue(allocation, processes);
}

int main() {
    int blockCount, processCount;

    cout << "Enter number of memory blocks: ";
    cin >> blockCount;
    vector<int> blocks(blockCount);

    cout << "Enter sizes of memory blocks:\n";
    for (int i = 0; i < blockCount; i++)
        cin >> blocks[i];

    cout << "Enter number of processes: ";
    cin >> processCount;
    vector<int> processes(processCount);

    cout << "Enter sizes of processes:\n";
    for (int i = 0; i < processCount; i++)
        cin >> processes[i];

    firstFit(blocks, processes);
    bestFit(blocks, processes);
    worstFit(blocks, processes);
    nextFit(blocks, processes);

    return 0;
}