#include "Snap.h"

#include "path.h"
#include "io.h"
#include "dlink.h"
#include "shortest_path.h"
#include "realtime_dta.h"
#include "vms.h"

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#define SLEEP_TIME 100000
#define RESET_THRESHOLD 500000

bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

int main()
{

  std::string file_folder = ".";
  std::string instruction_file_name = file_folder + "/instruction";

  MNM_Realtime_Dta* rd = new MNM_Realtime_Dta(file_folder);
  rd -> init_running();

  while(true){
    if (!is_file_exist(instruction_file_name.c_str())){
      usleep( SLEEP_TIME );
      continue;
    }
    std::ifstream instruction_file;
    instruction_file.open(instruction_file_name, std::ios::in);

    std::string line;
    TInt assign_inter;
    try {
      if (instruction_file.is_open()){
        getline(instruction_file, line);
        assign_inter = TInt(std::stoi(line));
        instruction_file.close();
      }
      
      if (assign_inter == 0){
        rd -> reset();
      }
    }
    catch (int e) {
      printf("Can't retrive interval, skip this interval.\n");
    }

    try {
      rd -> one_iteration(assign_inter);
    }
    catch (int e) {
      printf("Skip this interval.\n");
    }

    if (rd -> m_before_shot -> m_veh_factory -> m_veh_map.size() > RESET_THRESHOLD 
          || rd -> m_after_shot -> m_veh_factory -> m_veh_map.size() > RESET_THRESHOLD){
      rd -> reset();
    }

    std::remove(instruction_file_name.c_str());

    system("python /home/hzn/project/dataproject/manage.py import_online_congestion");
  }
  return 0;
}


