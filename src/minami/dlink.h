#ifndef DLINK_H
#define DLINK_H

#include "Snap.h"

#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

#include <vector>
#include <deque>
#include <map>

class MNM_Veh;

class MNM_Dlink
{
public:
  MNM_Dlink( TInt number_of_lane,
             TFlt length,
             TFlt ffs );
  ~MNM_Dlink();
  int virtual evolve(TInt timestamp){return 0;};
  TFlt virtual get_link_supply(){return TFlt(0);};
  int virtual clear_incoming_array(){return 0;};
  TInt m_number_of_lane;
  TFlt m_length;
  TFlt m_ffs;

  std::map<TInt, std::deque<MNM_Veh*>*> m_finished_array;
  std::deque<MNM_Veh *> m_incoming_array;
};


class MNM_Dlink_Ctm : MNM_Dlink
{
public:
  MNM_Dlink_Ctm(TFlt lane_hold_cap, 
                TFlt lane_flow_cap, 
                TInt number_of_lane,
                TFlt length,
                TFlt ffs,
                TFlt unit_time,
                TFlt flow_scalar);
  ~MNM_Dlink_Ctm();
  int virtual evolve(TInt timestamp);
  TFlt virtual get_link_supply();
  int virtual clear_incoming_array();
  void print_info();

private:
  class Ctm_Cell;
  int init_cell_array(TFlt unit_time, TFlt std_cell_length, TFlt lane_hold_cap_last_cell);
  int update_out_veh();
  int move_last_cell();
  int move_veh_queue(std::deque<MNM_Veh*> from_queue, std::deque<MNM_Veh*> to_queue, TInt number_tomove);
  TInt m_num_cells;
  TFlt m_lane_hold_cap;
  TFlt m_lane_flow_cap;
  TFlt m_flow_scalar;
  TFlt m_wave_ratio;
  TFlt m_last_wave_ratio;
  std::vector<Ctm_Cell*> m_cell_array;
};


class MNM_Dlink_Ctm::Ctm_Cell
{
public:
  Ctm_Cell(TFlt hold_cap, TFlt flow_cap, TFlt flow_scalar, TFlt wave_ratio);
  ~Ctm_Cell();
  TFlt get_demand();
  TFlt get_supply();

  TInt m_volume;
  TFlt m_flow_scalar;
  TFlt m_hold_cap;
  TFlt m_flow_cap;
  TFlt m_wave_ratio;
  TInt m_out_veh;
  std::deque<MNM_Veh*>  m_veh_queue; 
};


class MNM_Veh
{
public:
  MNM_Veh();
  ~MNM_Veh();
  TInt get_next_link();
private:
  TInt m_ID;
  TInt m_current_link;
  std::vector<TInt> m_future_links;
};
#endif