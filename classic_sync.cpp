#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>

using namespace std;

class Semaphore{
    int count;
    mutex m;
    condition_variable cv;

    public:
    Semaphore(int init = 0){
        count = init;
    }

    void wait(){
    unique_lock<mutex> lock(m);
    count--;
    if (count < 0){
        cv.wait(lock);
        }
    }

    void signal(){
        unique_lock<mutex> lock(m);
        count++;
        if(count <= 0){
            cv.notify_one();
        }
    }    
};

class ClassicalSynchronization{
    vector <int> buffer;
    int N;
    int in, out;

    Semaphore(mutex);
    Semaphore(full);
    Semaphore(empty);

    atomic<int> produced_count;
    atomic<int> consumed_count;
    int total_items;

    public:
    ClassicalSynchronization(int buffer_size, int process_items):
    buffer(buffer_size), N(buffer_size), in(0), out(0),
    mutex(1), full(0), empty(buffer_size),
    produced_count(0), consumed_count(0),
    total_items(process_items){}

    void producer(int id, int producers_total){
        while(true){
            int item_no = produced_count.fetch_add(1) + 1;
            if (item_no > total_items) break;

            cout << "P" << id << " Produced item: " << item_no << endl;

            empty.wait();
            mutex.wait();

            buffer[in] = item_no;
            cout << "[P" << id << "] insert_item: inserted " << item_no << " into slot " << in << endl;
            in = (in +1)%N;

            mutex.signal();
            full.signal();

            this_thread::sleep_for(chrono::milliseconds(10));
        }
        cout << "[P" << id << "] finished producing." << endl;
    }

    void consumer(int id){
        while(true){
            if (consumed_count.load() >= total_items) break;

            full.wait();
            mutex.wait();

            int item = buffer[out];
            cout << "[C" << id << "] remove_item: removed " << item << " from slot " << out << endl;
            out = (out+1) % N;
            consumed_count.fetch_add(1);

            mutex.signal();
            empty.signal();

            cout << "[C" << id << "] Consumed item " << item << endl;

            if (consumed_count.load() >= total_items) break;

            this_thread::sleep_for(chrono::milliseconds(80));
        } 
        cout << "[C" << id << "] finished consuming." << endl;   
    }
};

int main(){
    cout << "=== Classical Producer-Consumer (Semaphore + Mutex) ===" << endl;
    int buffer_size = 0, total_items = 0, num_producers = 1, num_consumers = 1;

    cout << "Enter buffer size (N > 0): ";
    while (!(cin >> buffer_size) || buffer_size <= 0) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a positive integer for buffer size: ";
    }

    cout << "Enter total number of items to produce/consume (> 0): ";
    while (!(cin >> total_items) || total_items <= 0) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a positive integer for total items: ";
    }

    cout << "\nStarting with buffer_size=" << buffer_size
         << ", total_items=" << total_items
         << ", producers=" << num_producers
         << ", consumers=" << num_consumers << "\n\n";

    ClassicalSynchronization cs(buffer_size, total_items);

    vector<thread> producers;
    for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back(&ClassicalSynchronization::producer, &cs, i+1, num_producers);
    }

    vector<thread> consumers;
    for (int i = 0; i < num_consumers; ++i) {
        consumers.emplace_back(&ClassicalSynchronization::consumer, &cs, i+1);
    }

    for (auto& p : producers) p.join();
    for (auto& c : consumers) c.join();

    cout << "All producers and consumers have finished.\n";
    return 0;
}