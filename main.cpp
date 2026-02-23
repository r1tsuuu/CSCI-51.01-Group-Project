#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <deque>

using namespace std;

// --- STRUCTS ---
struct Process {
    int id;            
    int arrival_time;
    int burst_time;
    int nice_level;    
    
    // Dynamic fields
    int remaining_time; 
    int start_time;        
    int completion_time;    
    int waiting_time;
    int turnaround_time;
    int response_time;
    bool is_started;
};

struct Block {
    int start_time;
    int process_id;
    int duration;
    bool is_completed; 
};


// --- HELPER FUNCTION ---
void print_results(int test_case_number, const string& algo_name, 
                   const vector<Block>& gantt_chart, 
                   const vector<Process>& processes, 
                   int total_time_elapsed, int total_burst_time, int idle_time) {
    
    cout << test_case_number << " " << algo_name << "\n";
    
    // Print Gantt Chart
    for (const auto& block : gantt_chart) {
        cout << block.start_time << " " << block.process_id << " " << block.duration;
        if (block.is_completed) cout << "X";
        cout << "\n";
    }
    
    // System Metrics
    cout << "Total time elapsed: " << total_time_elapsed << "ns\n";
    cout << "Total CPU burst time: " << total_burst_time << "ns\n";
    
    // CPU Utilization 
    double utilization = 0.0;
    if (total_time_elapsed > 0) {
        utilization = ((double)(total_time_elapsed - idle_time) / total_time_elapsed) * 100.0;
    }
    cout << "CPU Utilization: " << (int)utilization << "%\n";
    
    // Throughput
    double throughput = 0.0;
    if (total_time_elapsed > 0) {
        throughput = (double)processes.size() / total_time_elapsed;
    }

    cout << "Throughput: " << setprecision(15) << throughput << " processes/ns\n";
    
    // Metrics
    double total_wait = 0, total_turnaround = 0, total_response = 0;
    
    cout << "Waiting times:\n";
    for (const auto& p : processes) {
        cout << " Process " << p.id << ": " << p.waiting_time << "ns\n";
        total_wait += p.waiting_time;
    }
    cout << "Average waiting time: " << (total_wait / processes.size()) << "ns\n";
    
    cout << "Turnaround times:\n";
    for (const auto& p : processes) {
        cout << " Process " << p.id << ": " << p.turnaround_time << "ns\n";
        total_turnaround += p.turnaround_time;
    }
    cout << "Average turnaround time: " << (total_turnaround / processes.size()) << "ns\n";
    
    cout << "Response times:\n";
    for (const auto& p : processes) {
        cout << " Process " << p.id << ": " << p.response_time << "ns\n";
        total_response += p.response_time;
    }
    cout << "Average response time: " << (total_response / processes.size()) << "ns\n";
}


// --- ALGORITHMS ---

void simulate_FCFS(vector<Process>& processes, int test_case_number) {
    // TODO: Implement First Come First Served
    // Hint: Sort processes by arrival time first, then execute sequentially.
}

void simulate_SJF(vector<Process>& processes, int test_case_number) {
    int current_time = 0;
    int completed_count = 0;
    int n = processes.size();
    
    vector<Block> gantt_chart;
    int total_burst_time = 0;
    int idle_time = 0;
    
    // Loop until all processes are marked as completed
    while (completed_count < n) {
        int shortest_index = -1;
        int min_burst = 1e9; 
        
        // Check arrivals and find the shortest burst time
        for (int i = 0; i < n; i++) {
            // Process must have arrived and not yet be completed
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].burst_time < min_burst) {
                    min_burst = processes[i].burst_time;
                    shortest_index = i;
                } 
                // TIEBREAKER via FCFS: If two processes have the same burst time, choose the one that arrived first
                else if (processes[i].burst_time == min_burst) {
                    if (processes[i].arrival_time < processes[shortest_index].arrival_time) {
                        shortest_index = i;
                    }
                }
            }
        }

        // Process Execution
        if (shortest_index != -1) {
            if (!processes[shortest_index].is_started) {
                processes[shortest_index].start_time = current_time;
                processes[shortest_index].is_started = true;
            }
    
            int duration = processes[shortest_index].burst_time;
            
            // Add block to Gantt Chart
            gantt_chart.push_back({current_time, processes[shortest_index].id, duration, true});
            
            // Allow the current process to finish entirely
            // since SJF is non-preemptive
            current_time += duration;
            total_burst_time += duration;
            processes[shortest_index].remaining_time = 0;
            
            // Calculate metrics
            processes[shortest_index].completion_time = current_time;
            processes[shortest_index].turnaround_time = processes[shortest_index].completion_time - processes[shortest_index].arrival_time;
            processes[shortest_index].waiting_time = processes[shortest_index].turnaround_time - processes[shortest_index].burst_time;
            processes[shortest_index].response_time = processes[shortest_index].start_time - processes[shortest_index].arrival_time;
            
            completed_count++;
        } else {
            current_time++;
            idle_time++;
        }
    }
    
    print_results(test_case_number, "SJF", gantt_chart, processes, current_time, total_burst_time, idle_time);
}

void simulate_SRTF(vector<Process>& processes, int test_case_number) {
    int current_time = 0;
    int completed_count = 0;
    int n = processes.size();
    
    vector<Block> gantt_chart;
    int total_burst_time = 0;
    int idle_time = 0;
    
    int prev_process_index = -1;
    int block_start_time = 0;
    
    // Loop until all processes are marked as completed
    while (completed_count < n) {
        int shortest_index = -1;
        int min_remaining = 1e9; // Start with an arbitrarily large number
        
        // Check queue to find the process with the shortest REMAINING time
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time < min_remaining) {
                    min_remaining = processes[i].remaining_time;
                    shortest_index = i;
                } 
                // TIEBREAKER via FCFS: If two processes have the same remaining time, choose the one that arrived first
                else if (processes[i].remaining_time == min_remaining) {
                    if (shortest_index == -1 || processes[i].arrival_time < processes[shortest_index].arrival_time) {
                        shortest_index = i;
                    }
                }
            }
        }
        
        // Preemption Handling and Process Execution
        if (shortest_index != -1) {
            // If a Context Switch occurs
            if (prev_process_index != shortest_index) {
                // Save the progress of the preempted process into the Gantt chart
                if (prev_process_index != -1 && processes[prev_process_index].remaining_time > 0) {
                    gantt_chart.push_back({block_start_time, processes[prev_process_index].id, current_time - block_start_time, false});
                }
                
                block_start_time = current_time;
                prev_process_index = shortest_index;
                
                // Record start time if this is the first time the process is executing
                if (!processes[shortest_index].is_started) {
                    processes[shortest_index].start_time = current_time;
                    processes[shortest_index].is_started = true;
                }
            }
            
            // Execute the process for 1 unit of time
            processes[shortest_index].remaining_time--;
            current_time++;
            total_burst_time++;
            
            // Check if the current process just finished
            if (processes[shortest_index].remaining_time == 0) {
                // Add the completed block to the Gantt chart
                gantt_chart.push_back({block_start_time, processes[shortest_index].id, current_time - block_start_time, true});
                
                // Calculate metrics
                processes[shortest_index].completion_time = current_time;
                processes[shortest_index].turnaround_time = processes[shortest_index].completion_time - processes[shortest_index].arrival_time;
                processes[shortest_index].waiting_time = processes[shortest_index].turnaround_time - processes[shortest_index].burst_time;
                processes[shortest_index].response_time = processes[shortest_index].start_time - processes[shortest_index].arrival_time;
                
                completed_count++;
                prev_process_index = -1; // Reset so the next selected process triggers a new block
            }
        } else {
            current_time++;
            idle_time++;
            block_start_time = current_time; // Keep moving the start time until a process arrives
        }
    }
    
    print_results(test_case_number, "SRTF", gantt_chart, processes, current_time, total_burst_time, idle_time);
}

void simulate_Priority(vector<Process>& processes, int test_case_number) {
    // TODO: Implement Priority (Preemptive)
    // Hint: Look at nice_level instead of remaining_time. Remember it's preemptive.
}

void simulate_RR(vector<Process>& processes, int quantum, int test_case_number) {
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrival_time != b.arrival_time) return a.arrival_time < b.arrival_time;
        return a.nice_level < b.nice_level; 
    });

    deque<Process*> ready_q;
    vector<Block> gantt_chart;
    
    // Setup metrics
    int elapsed_time = 0;
    int completed_count = 0;
    int process_idx = 0;
    int n = processes.size();

    int total_burst_time = 0;
    int idle_time = 0;

    while (completed_count < n) {
        // Calculate next arrival time if no processes are ready
        if (ready_q.empty() && process_idx < n && processes[process_idx].arrival_time > elapsed_time) {
            idle_time += (processes[process_idx].arrival_time - elapsed_time);
            elapsed_time = processes[process_idx].arrival_time;
        }
        // Get new arrivals
        vector<Process*> new_arrivals;
        while (process_idx < n && processes[process_idx].arrival_time <= elapsed_time) {
            new_arrivals.push_back(&processes[process_idx]);
            process_idx++;
        }
        // Add new arrivals to ready queue
        for (auto it = new_arrivals.rbegin(); it != new_arrivals.rend(); ++it) {
            ready_q.push_front(*it);
        }

        if (!ready_q.empty()) {
            Process* p = ready_q.front();
            ready_q.pop_front();

            int time_spent = min(quantum, p->remaining_time);
            gantt_chart.push_back({elapsed_time, p->id, time_spent, (p->remaining_time - time_spent <= 0)});
            
            p->start_time = (p->is_started) ? p->start_time : elapsed_time;
            p->remaining_time -= time_spent;
            elapsed_time += time_spent;
            total_burst_time += time_spent;

            bool is_complete = (p->remaining_time <= 0);
            vector<Process*> mid_run_arrivals;
            while (process_idx < n && processes[process_idx].arrival_time <= elapsed_time) {
                mid_run_arrivals.push_back(&processes[process_idx]);
                process_idx++;
            }
            for (auto it = mid_run_arrivals.rbegin(); it != mid_run_arrivals.rend(); ++it) {
                ready_q.push_front(*it);
            }
            if (is_complete) {
                completed_count++;
                // Calculate metrics
                p->completion_time = elapsed_time;
                p->turnaround_time = p->completion_time - p->arrival_time;
                p->waiting_time = p->turnaround_time - p->burst_time;
                p->response_time = p->start_time - p->arrival_time;
            } else {
                p->is_started = true;
                ready_q.push_back(p);
            }
        }
    }
    // Sort by ID for final output
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });
    print_results(test_case_number, "RR", gantt_chart, processes, elapsed_time, total_burst_time, idle_time);
}

// --- MAIN FUNCTION & PARSER ---

int main() {
    int num_test_cases;
    
    // Read the number of test cases. 
    // If we reach End-Of-File unexpectedly, this prevents infinite loops.
    if (!(cin >> num_test_cases)) return 0;

    for (int t = 1; t <= num_test_cases; ++t) {
        int num_processes;
        string algo_name;
        int quantum = 0;

        cin >> num_processes >> algo_name;
        
        if (algo_name == "RR") {
            cin >> quantum;
        }

        vector<Process> processes(num_processes);
        for (int i = 0; i < num_processes; ++i) {
            processes[i].id = i + 1;
            cin >> processes[i].arrival_time >> processes[i].burst_time >> processes[i].nice_level;
            
            // Initialize dynamic states
            processes[i].remaining_time = processes[i].burst_time;
            processes[i].is_started = false;
            processes[i].start_time = -1;
            processes[i].completion_time = 0;
            processes[i].waiting_time = 0;
            processes[i].turnaround_time = 0;
            processes[i].response_time = 0;
        }

        // Routing
        if (algo_name == "FCFS") {
            simulate_FCFS(processes, t);
        } else if (algo_name == "SJF") {
            simulate_SJF(processes, t);
        } else if (algo_name == "SRTF") {
            simulate_SRTF(processes, t);
        } else if (algo_name == "P") {
            simulate_Priority(processes, t);
        } else if (algo_name == "RR") {
            simulate_RR(processes, quantum, t);
        }
    }

    return 0;
}