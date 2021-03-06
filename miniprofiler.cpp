// Copyright 2012, Olav Stetter
// 
// This file is part of TE-Causality.
// 
// TE-Causality is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// TE-Causality is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with TE-Causality.  If not, see <http://www.gnu.org/licenses/>.

#include "miniprofiler.h"

MiniProfiler::MiniProfiler() {
  // std::cout <<"DEBUG: MiniProfiler#init."<<std::endl;
}

MiniProfiler::~MiniProfiler() {
  // std::cout <<"DEBUG: MiniProfiler#delete."<<std::endl;
}

int MiniProfiler::number_of_registered_tasks() const {
  return tasks.size();
}

void MiniProfiler::register_task(const std::string& t_name) {
  struct MiniProfilerTask* new_task = new struct MiniProfilerTask;
  new_task->name = t_name;
  new_task->elapsed_clock_ticks = 0;
  new_task->currently_active = false;

  tasks.push_back( *new_task );
}

int MiniProfiler::search(const std::string& t_name) const {
  // This should really be implemented using a hash table, but as long as we one
  // use very few tasks this should work
  for(int i = 0; i < tasks.size(); i++) {
    if( tasks[i].name == t_name ) {
      // std::cout <<"DEBUG: found the task."<<std::endl;
      return i;
    }
  }
  return -1;
}

float MiniProfiler::get_current_time(int task_index) const {
  if( tasks[task_index].currently_active ) {
    clock_t now = clock();
    return float(tasks[task_index].elapsed_clock_ticks + \
      (now - tasks[task_index].clock_tick_of_activation))/CLOCKS_PER_SEC;
  }
  // or, if the task is inactive then all previous intervals have already been added up
  return float(tasks[task_index].elapsed_clock_ticks)/CLOCKS_PER_SEC;
}

void MiniProfiler::resuming_task(const std::string& t_name) {
  int t_index = search(t_name);
  if( t_index == -1 ) {
    std::cout <<"Error in MiniProfiler#resuming_task: Invalid task key."<<std::endl;
    exit(1);
  }
  if( tasks[t_index].currently_active ) {
    std::cout <<"Warning in MiniProfiler#resuming_task: task '"<<t_name<<"' is already running!"<<std::endl;
  } else {
    tasks[t_index].clock_tick_of_activation = clock();
    tasks[t_index].currently_active = true;
  }
}

void MiniProfiler::stopping_task(const std::string& t_name) {
  int t_index = search(t_name);
  if( t_index == -1 ) {
    std::cout <<"Error in MiniProfiler#stopping_task: Invalid task key."<<std::endl;
    exit(1);
  }
  if( !tasks[t_index].currently_active ) {
    std::cout <<"Warning in MiniProfiler#stopping_task: task '"<<t_name<<"' is not running!"<<std::endl;
  } else {
    tasks[t_index].elapsed_clock_ticks += clock() - tasks[t_index].clock_tick_of_activation;
    tasks[t_index].currently_active = false;
  }
}

float MiniProfiler::get_current_time(const std::string& t_name) const {
  int task_index = search(t_name);
  
  if( task_index >= 0 ) { // if such a task was found
    return get_current_time(task_index);
  }
  return 0.0;
}

std::string MiniProfiler::summary() {
  clock_t now = clock();
  std::stringstream summ;
  for(int i = 0; i < tasks.size(); i++) {
    summ  <<"- "<<tasks[i].name<<": "<<get_current_time(i)<<"s\n";
  }
  return summ.str();
}
