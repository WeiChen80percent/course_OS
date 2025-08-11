#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <map>
#include <iomanip>
using namespace std;

// Array to store possible arrival times for processes
int arrival_time_array[3] = {0,5,10};
// Array to store lottery tickets based on priority (priority 1: 15, 2: 10, 3: 5)
int lottery_array[3] = {15,10,5};

// Structure to represent a process with its attributes
struct process{ 
    int identification;  
    int arrival_time;  
    int burst_time;  
    int priority;  
};

// Function to generate 10 random processes with ID, arrival time, burst time, and priority
void random_process(vector<process> &process_queue){
    srand(time(nullptr)); // Seed random number generator with current time
    for(int i=0;i<10;i++){
        process new_process;
        new_process.identification = i+1; // Assign process ID (1 to 10)
        new_process.arrival_time = arrival_time_array[rand()%3]; // Random arrival time from {0, 5, 10}
        new_process.burst_time = rand()%23+6; // Random burst time between 6 and 28
        new_process.priority = rand()%3+1; // Random priority between 1 and 3
        process_queue.push_back(new_process); // Add process to queue
        cout<<new_process.identification<<" "<<new_process.arrival_time<<" "<<new_process.burst_time<<" "<<new_process.priority<<endl; // Print process details
    }
}

// Function to print the Gantt chart showing process execution timeline
void print_gantte(vector<int> gantte){
    cout << "Pid : 1 2 3 4 5 6 7 8 9 10" << endl; // Header for process IDs
    cout<<"---------------------------"<<endl; // Separator
    for(int i=0;i<gantte.size();i++){
        if(gantte[i] == 0){
            cout<<setw(3)<<i+1<<" | "; // Print time unit
            cout<<"idle"<<endl; // Indicate idle state
            continue;
        }
        cout<<setw(3)<<i+1<<" | "; // Print time unit
        for(int j=1;j<gantte[i];j++){
            cout<<"  "; // Spacing for visual alignment
        }
        cout<<gantte[i]<<endl; // Print process ID
    }
}

// Function to implement Lottery Scheduling
void customized(){
    vector<process> process_queue; // Initialize process queue
    vector<int> gantte; // Stores Gantt chart entries
    map<int,int> arrival; // Maps process ID to arrival time
    map<int,int> burst; // Maps process ID to burst time
    map<int,int> completion; // Maps process ID to completion time
    map<int,int> lottery; // Maps process ID to number of lottery tickets
    random_process(process_queue); // Generate random processes
    int current_time=0; // Tracks current time in simulation
    for(int i=0;i<10;i++){
        arrival[process_queue[i].identification]=process_queue[i].arrival_time; // Store arrival time
        burst[process_queue[i].identification]=process_queue[i].burst_time; // Store burst time
        lottery[process_queue[i].identification]=lottery_array[process_queue[i].priority-1]; // Assign lottery tickets based on priority
    }
    //
    while(1){
        int flag=0; // Flag to check if any process is active
        for(int i=0;i<10;i++){
            if(process_queue[i].burst_time != 0){
                flag=1; // Set flag if any process has remaining burst time
            }
        }
        if(flag==0){
            break; // Exit loop if all processes are completed
        }
        int total_lottery=0; // Total lottery tickets for eligible processes
        for(int i=0;i<10;i++){
            if(process_queue[i].burst_time != 0 && process_queue[i].arrival_time <= current_time){
                total_lottery+=lottery[process_queue[i].identification]; // Sum tickets for processes that have arrived and are not completed
            }
        }
        if(total_lottery == 0){
            current_time++; // Increment time
            gantte.push_back(0); // Add idle state to Gantt chart
            continue;
        }
        int random_lottery=rand()%total_lottery+1; // Generate random ticket number (1 to total_lottery)
        int selected_process=0; // Index of selected process
        for(int i=0;i<10;i++){
            if(process_queue[i].burst_time != 0 && process_queue[i].arrival_time <= current_time){
                random_lottery-=lottery[process_queue[i].identification]; // Subtract tickets for current process
                if(random_lottery <= 0){
                    selected_process=i; // Select process when ticket count reaches or goes below zero
                    break;
                }
            }
        }
        current_time++; // Increment time
        process_queue[selected_process].burst_time--; // Decrease burst time of selected process
        gantte.push_back(process_queue[selected_process].identification); // Add process ID to Gantt chart
        if(process_queue[selected_process].burst_time == 0){
            completion[process_queue[selected_process].identification]=current_time; // Record completion time
        }
    }
   
    //
    print_gantte(gantte); // Print Gantt chart
    cout<<"All processes are completed."<<endl; // Completion message
    cout<<"Average turnaround time: "<<endl; // Calculate and print average turnaround time
    int total_turnaround_time=0;
    for(int i=0;i<10;i++){
        total_turnaround_time+=(completion[i+1]-arrival[i+1]); // Sum turnaround times
        cout<<"("<<completion[i+1]<<"-"<<arrival[i+1]<<")"; // Print individual turnaround
        if(i==9){
            break;
        }
        cout<<"+"; // Separator
    }
    cout<<"="<<(float)total_turnaround_time/10.0<<endl; // Print average
    cout<<"Average waiting time: "<<endl; // Calculate and print average waiting time
    int total_waiting_time=0;
    for(int i=0;i<10;i++){
        total_waiting_time+=(completion[i+1]-arrival[i+1]-burst[i+1]); // Sum waiting times
        cout<<"("<<completion[i+1]<<"-"<<arrival[i+1]<<"-"<<burst[i+1]<<")"; // Print individual waiting time
        if(i==9){
            break;
        }
        cout<<"+"; // Separator
    }
    cout<<"="<<(float)total_waiting_time/10.0<<endl; // Print average
    int context_switch=0; // Count context switches
    for(int i=0;i<gantte.size()-1;i++){
        if(gantte[i]!=gantte[i+1]){
            context_switch++; // Increment on process change
        }
    }
    cout<<"Total context switch: "<<context_switch<<endl; // Print total context switches
}

// Main function to initialize and run the Lottery Scheduling simulation
int main(){
    customized(); // Run Lottery Scheduling
}