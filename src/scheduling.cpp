#include <scheduling.h>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string>

using namespace std;

pqueue_arrival read_workload(string filename) {
  pqueue_arrival workload;
  fstream inFile;
  inFile.open(filename, fstream::in);
  int arrival;
  int duration;
  while (inFile >> arrival >> duration) {
    Process p;
    p.arrival = arrival;
    p.duration = duration;
    p.first_run = -1;
    p.completion = -1;
    workload.push(p);
  }
  return workload;
}

void show_workload(pqueue_arrival workload) {
  pqueue_arrival xs = workload;
  cout << "Workload:" << endl;
  while (!xs.empty()) {
    Process p = xs.top();
    cout << '\t' << p.arrival << ' ' << p.duration << endl;
    xs.pop();
  }
}

void show_processes(list<Process> processes) {
  list<Process> xs = processes;
  cout << "Processes:" << endl;
  while (!xs.empty()) {
    Process p = xs.front();
    cout << "\tarrival=" << p.arrival << ", duration=" << p.duration
         << ", first_run=" << p.first_run << ", completion=" << p.completion
         << endl;
    xs.pop_front();
  }
}

list<Process> fifo(pqueue_arrival workload) {
  list<Process> complete;
  int time = 0;
  while (!workload.empty()) {
    Process p = workload.top();
    if (p.arrival <= time) {
      p.first_run = time;
      time += p.duration;
      p.completion = time;
      p.duration = 0;
      complete.push_back(p);
      workload.pop();
    }
    else {
      time++;
    }
  }
  return complete;
}

list<Process> sjf(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_duration byDuration;
  int time = 0;
  while (!workload.empty()) {
    while (!workload.empty() && workload.top().arrival <= time) {
      byDuration.push(workload.top());
      workload.pop();
    }
    if (!byDuration.empty()) {
      while (!byDuration.empty()) {
        Process p = byDuration.top();
        p.first_run = time;
        time += p.duration;
        p.completion = time;
        p.duration = 0;        
        complete.push_back(p);
        byDuration.pop();
      }
    }
    else {
      time++;
    }
  }
  return complete;
}

list<Process> stcf(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_duration byDuration;
  int time = 0;
  while (!workload.empty()) {
    while (!workload.empty() && workload.top().arrival <= time) {
      byDuration.push(workload.top());
      workload.pop();
    }
    while (!byDuration.empty()) {
      //arrived? can run? finished?
      //time++
      //-- top of byDuration
      Process p = byDuration.top();
      if (p.first_run < 0)
        p.first_run = time;

      //OPTION 1
      //check next job
      //if arrived, add to FRONT of queue
      //increase time

      //OPTION2
      //check when next job will arrive
      bool flag = true;
      while (!workload.empty()) {
        Process next = workload.top();
        if (p.duration > next.arrival + next.duration) {  //if p goes on longer than it takes next to
          if (flag)
            p.duration -= next.arrival;                               //run p until next arrives     
          flag = false;   
          if (next.arrival > time)
            time += next.arrival;                                       //fast forward
          next.first_run = time;                                      //run next
          time += next.duration;                                      //finish running next
          next.duration = 0;
          next.completion = time;
          complete.push_back(next);
          workload.pop();
        }
        else {
          break;
        }
      }
      //complete current job (up until next job)
      time += p.duration;
      p.duration = 0;
      p.completion = time;
      complete.push_back(p);
      byDuration.pop();
    }
  }
  return complete;
}

list<Process> rr(pqueue_arrival workload) {
  list<Process> complete;
  pqueue_duration byDuration;
  int time = 0;
  while (!workload.empty()) {
    while (!workload.empty() && workload.top().arrival <= time) {
      byDuration.push(workload.top());
      workload.pop();
    }
    if (!byDuration.empty()) {
      while (!byDuration.empty()) {
        Process p = byDuration.top();
        if (p.first_run < 0) {
          p.first_run = time;
        }
        time++;
        p.duration--;
        if (p.duration == 0) {
          p.completion = time;
          complete.push_back(p);
        }
        else {
          if (!workload.empty()) {
            Process next = workload.top();
            if (next.arrival == time) {
              p.duration--;
              time++;
            }
          }
          workload.push(p);
        }
        byDuration.pop();          
      }
    }
    else {
      time++;
    }
  }
  return complete;
}

float avg_turnaround(list<Process> processes) {
  float sum = 0;
  int count = 0;
  list<Process> xs = processes;

  while (!xs.empty()) {
    Process p = xs.front();
    sum += p.completion - p.arrival;
    count++;
    xs.pop_front();
  }
  return sum/count;
}

float avg_response(list<Process> processes) {
  float sum = 0;
  int count = 0;
  list<Process> xs = processes;

  while (!xs.empty()) {
    Process p = xs.front();
    sum += p.first_run - p.arrival;
    count++;
    xs.pop_front();
  }
  return sum/count;
}

void show_metrics(list<Process> processes) {
  float avg_t = avg_turnaround(processes);
  float avg_r = avg_response(processes);
  show_processes(processes);
  cout << '\n';
  cout << "Average Turnaround Time: " << avg_t << endl;
  cout << "Average Response Time:   " << avg_r << endl;
}
