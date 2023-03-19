// this is the only file you should modify and submit for grading

#include "scheduler.h"
#include "common.h"
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <climits>

//follows hints given, tried to implement optimization, but kept getting type errors
void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) {
    seq.clear();

    //if empty, no need to change anything
    if (processes.size() == 0) {
        return;
    }

    //variables
    int64_t curr_time = 0;
    int id_counter = 0;
    std::vector<int> jq;
    std::vector<int64_t> remaining_bursts(processes.size());

    while (1) {
        //add all processes to queue if has arrived
        while (id_counter < int(processes.size()) ) {

            if(processes[id_counter].arrival > curr_time){
                break;
            }
            jq.push_back(id_counter);
            remaining_bursts[id_counter] = processes[id_counter].burst;
            id_counter++;
        }
        //quque is not empty
        if (jq.empty() == false) {
            int current_id = jq.front();
            int queue_size = int(jq.size());
            jq.pop_back();

            //"condesnded seq"
            if (seq.back() != current_id || seq.empty()) {
                seq.push_back(current_id);
            }

            //start process if not already
            if (processes[current_id].start_time == -1) {
                processes[current_id].start_time = curr_time;
            }
            
            //last in queue
            if (queue_size == 1 && id_counter == int(processes.size())) {
                curr_time += remaining_bursts[current_id];
                remaining_bursts[current_id] = 0;
            } //last in queue, done before next arrives
            else if (queue_size == 1 && curr_time + remaining_bursts[current_id] <= processes[id_counter].arrival) {
                processes[current_id].finish_time = curr_time + remaining_bursts[current_id];
                remaining_bursts[current_id] = 0;
            } //last in queue,next comes before done
            else if (queue_size == 1 && quantum <= processes[id_counter].arrival - curr_time) {
                int64_t jump = ((processes[id_counter].arrival - curr_time) / quantum + 1) * quantum;
                remaining_bursts[current_id] -= jump;
                curr_time += jump;
              
            } 
            else {
                //move quantum is avaliable
                if (remaining_bursts[current_id] >= quantum) {
                    remaining_bursts[current_id] -= quantum;
                    curr_time += quantum;
                } 
                //quantum is too big, we can just finish and move to that time
                else { 
                    curr_time += remaining_bursts[current_id];
                    remaining_bursts[current_id] = 0;
                }
                
            }
            //mark as finished
            if (remaining_bursts[current_id] == 0 && processes[current_id].finish_time == -1) {
                processes[current_id].finish_time = curr_time;
                
            } 
            //not finished
            else {
                
                while (id_counter < int(processes.size()) && processes[id_counter].arrival < curr_time) {
                    jq.push_back(id_counter);
                    remaining_bursts[id_counter] = processes[id_counter].burst;
                    
                    id_counter++;
                }

                if (remaining_bursts[current_id] != 0) {
                    jq.push_back(current_id);
                }
            }
        } 

        else {
            seq.push_back(-1);
            if (id_counter < int(processes.size())) {
                curr_time = processes[id_counter].arrival;
            }
        }
        //everything is processed
        if (jq.empty() && int(processes.size()) == id_counter) {
            break;
        }

    }
    //change output size
    seq.resize(std::min(max_seq_len, int64_t(seq.size())));
}