#include "routing.h"
#include <omp.h>

MNM_Routing::MNM_Routing(PNEGraph &graph,
                          MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
{
  m_graph = graph;
  m_od_factory = od_factory;
  m_node_factory = node_factory;
  m_link_factory = link_factory;

}

MNM_Routing::~MNM_Routing()
{

}

/**************************************************************************
                          Random rouing
**************************************************************************/
/* assign each vehicle a random link ahead of it, only used for testing */

MNM_Routing_Random::MNM_Routing_Random(PNEGraph &graph, MNM_OD_Factory *od_factory, 
                                          MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
  : MNM_Routing::MNM_Routing(graph, od_factory, node_factory, link_factory)
{

}


MNM_Routing_Random::~MNM_Routing_Random()
{

}

int MNM_Routing_Random::init_routing(Path_Table *path_table)
{
  return 0;
}


int MNM_Routing_Random::update_routing(TInt timestamp)
{
  MNM_Origin *_origin;
  MNM_DMOND *_origin_node;
  TInt _node_ID;
  TNEGraph::TNodeI _node_I;
  TInt _out_ID;
  MNM_Dlink *_next_link;
  MNM_Dlink* _link;
  // printf("MNM_Routing: route the origin vehciles.\n");
  /* route the vehicle in Origin nodes */
  for (auto _origin_it = m_od_factory->m_origin_map.begin(); _origin_it != m_od_factory->m_origin_map.end(); _origin_it++){
    _origin = _origin_it -> second;
    _origin_node = _origin -> m_origin_node;
    _node_ID = _origin_node -> m_node_ID;
    for (auto _veh_it = _origin_node -> m_in_veh_queue.begin(); _veh_it!=_origin_node -> m_in_veh_queue.end(); _veh_it++){
      _node_I = m_graph -> GetNI(_node_ID);
      _out_ID = _node_I.GetOutNId(MNM_Ults::mod(rand(), _node_I.GetOutDeg()));
      _next_link = m_link_factory -> get_link(m_graph -> GetEI(_node_ID, _out_ID).GetId());
      (*_veh_it) -> set_next_link(_next_link);
    }
  }
  // printf("MNM_Routing: route the link vehciles.\n");
  TInt _link_ID;
  /* route the vehicles at the end of each link */
  for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it ++){
    _link = _link_it -> second;
    _node_ID = _link -> m_to_node -> m_node_ID;
    for (auto _veh_it = _link -> m_finished_array.begin(); _veh_it!=_link -> m_finished_array.end(); _veh_it++){
      _node_I = m_graph -> GetNI(_node_ID);
      if (_node_I.GetOutDeg() > 0){
        _link_ID = _node_I.GetOutEId(MNM_Ults::mod(rand(), _node_I.GetOutDeg()));
        _next_link = m_link_factory -> get_link(_link_ID);
        (*_veh_it) -> set_next_link(_next_link);
      }
      else{
        (*_veh_it) -> set_next_link(NULL);
      }
    }
  }
  // printf("MNM_Routing: Finished.\n");
  return 0;
}

/**************************************************************************
                          Hybrid rouing
**************************************************************************/

MNM_Routing_Hybrid::MNM_Routing_Hybrid(std::string file_folder, PNEGraph &graph, MNM_Statistics* statistics, 
                  MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
  : MNM_Routing::MNM_Routing(graph, od_factory, node_factory, link_factory)
{
  m_statistics = statistics;
  m_self_config = new MNM_ConfReader(file_folder + "/config.conf", "HYBRID");
  m_routing_freq = m_self_config -> get_int("route_frq");
  m_table = new Routing_Table();
}

MNM_Routing_Hybrid::~MNM_Routing_Hybrid()
{
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
    m_table -> find(_it -> second) -> second -> clear();
    delete m_table -> find(_it -> second) -> second;
  }
  m_table -> clear();
  delete m_table;
}

int MNM_Routing_Hybrid::init_routing(Path_Table *path_table)
{
  std::unordered_map<TInt, TInt> *_shortest_path_tree;
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
    _shortest_path_tree = new std::unordered_map<TInt, TInt>();
    m_table -> insert(std::pair<MNM_Destination*, std::unordered_map<TInt, TInt>*>(_it -> second, _shortest_path_tree));
    // for (auto _node_it = m_node_factory -> m_node_map.begin(); _node_it != m_node_factory -> m_node_map.end(); _node_it++){
    //   _shortest_path_tree -> insert(std::pair<TInt, TInt>(_node_it -> first, -1));
    // }
  }
  return 0;
}


int MNM_Routing_Hybrid::update_routing(TInt timestamp)
{
  MNM_Destination *_dest;
  TInt _dest_node_ID;
  std::unordered_map<TInt, TInt> *_shortest_path_tree;
  if ((timestamp) % m_routing_freq  == 0 || timestamp == 0) {
    printf("Calculating the shortest path trees!\n");
    for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
    // #pragma omp task firstprivate(_it)
      // {
        _dest = _it -> second;
        _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
        // printf("Destination ID: %d\n", (int) _dest_node_ID);
        _shortest_path_tree = m_table -> find(_dest) -> second;
        // MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, m_graph, m_statistics -> m_record_interval_tt, *_shortest_path_tree);
        MNM_Shortest_Path::all_to_one_Dijkstra(_dest_node_ID, m_graph, m_statistics -> m_record_interval_tt, *_shortest_path_tree);
      // } 
    }
  }


  /* route the vehicle in Origin nodes */
  printf("Routing the vehicle!\n");
  MNM_Origin *_origin;
  MNM_DMOND *_origin_node;
  TInt _node_ID, _next_link_ID;
  MNM_Dlink *_next_link;
  MNM_Veh *_veh;
  for (auto _origin_it = m_od_factory->m_origin_map.begin(); _origin_it != m_od_factory->m_origin_map.end(); _origin_it++){
    _origin = _origin_it -> second;
    _origin_node = _origin -> m_origin_node;
    _node_ID = _origin_node -> m_node_ID;
    for (auto _veh_it = _origin_node -> m_in_veh_queue.begin(); _veh_it!=_origin_node -> m_in_veh_queue.end(); _veh_it++){
      _veh = *_veh_it;
      _next_link_ID = m_table -> find(_veh -> get_destination()) -> second -> find(_node_ID) -> second;
      if (_next_link_ID < 0){
        printf("Something wrong in routing, wrong next link 1\n");
        exit(-1);
      }
      // printf("From origin, The next link ID will be %d\n", _next_link_ID());
      _next_link = m_link_factory -> get_link(_next_link_ID);
      _veh -> set_next_link(_next_link);
      // printf("The next link now it's %d\n", _veh -> get_next_link() -> m_link_ID());
    }
  }  

  MNM_Destination *_veh_dest;
  MNM_Dlink *_link;
  for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it ++){
    _link = _link_it -> second;
    _node_ID = _link -> m_to_node -> m_node_ID;
    for (auto _veh_it = _link -> m_finished_array.begin(); _veh_it!=_link -> m_finished_array.end(); _veh_it++){
      _veh = *_veh_it;
      if (_link != _veh -> get_current_link()){
        printf("Wrong current link!\n");
        exit(-1);
      }
      // if (_link != _veh -> get_next_link()){
      //   printf("Wrong node allocation!\n");
      //   printf("The next link should be %d, but now it's %d, current link is %d\n",_link -> m_link_ID(), 
      //                 _veh -> get_next_link() -> m_link_ID(), _veh -> get_current_link() -> m_link_ID());
      //   _next_link_ID = m_table -> find(_veh -> get_destination()) -> second -> find(_node_ID) -> second;
      //   _node_ID = m_table -> find(_veh -> get_destination()) -> second -> find(1) -> second;
      //   printf("The next link ID will be %d\n", _next_link_ID());
      //   printf("From origin, The next link ID will be %d\n", _node_ID());
      //   exit(-1);
      // }
      _veh_dest = _veh -> get_destination();
      if (_veh_dest -> m_dest_node -> m_node_ID == _node_ID){
        _veh -> set_next_link(NULL);
      }
      else{
        _next_link_ID = m_table -> find(_veh -> get_destination()) -> second -> find(_node_ID) -> second;
        if (_next_link_ID == -1){
          printf("Something wrong in routing, wrong next link 2\n");
          printf("The node is %d, the vehicle should head to %d\n", (int)_node_ID, (int)_veh_dest -> m_dest_node -> m_node_ID);
          // exit(-1);
          auto _node_I = m_graph -> GetNI(_node_ID);
          if (_node_I.GetOutDeg() > 0){
            printf("Assign randomly!\n");
            _next_link_ID = _node_I.GetOutEId(MNM_Ults::mod(rand(), _node_I.GetOutDeg()));
          }
          else
          {
            printf("Can't do anything!\n");
          }
        }
        _next_link = m_link_factory -> get_link(_next_link_ID);
        if (_next_link != NULL) {
          // printf("Checking future\n");
          TInt _next_node_ID = _next_link -> m_to_node -> m_node_ID;
          if (_next_node_ID != _veh -> get_destination() -> m_dest_node -> m_node_ID){
            // printf("Destination node is %d\n", _veh -> get_destination() -> m_dest_node -> m_node_ID());
            if (m_table -> find(_veh -> get_destination()) == m_table -> end()){
              printf("Cant'f find Destination\n");
            }
            if (m_table -> find(_veh -> get_destination()) -> second -> find(_next_node_ID) == m_table -> find(_veh -> get_destination()) -> second -> end()){
              printf("can't find _next_node_ID\n");
            }
            if (m_table -> find(_veh -> get_destination()) -> second -> find(_next_node_ID) -> second == -1){
              printf("Something wrong for the future node!\n");
              exit(-1);
            }
            // printf("Pass checking\n");
          }
        }
        _veh -> set_next_link(_next_link);
      }
    }
  }
  printf("Finished Routing\n");
  return 0;
}


/**************************************************************************
                          fixed rouing
**************************************************************************/

MNM_Routing_Fixed::MNM_Routing_Fixed(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory)
 : MNM_Routing::MNM_Routing(graph, od_factory, node_factory, link_factory)
{
  m_tracker = std::unordered_map<MNM_Veh*, std::deque<TInt>*>();
}

MNM_Routing_Fixed::~MNM_Routing_Fixed()
{
  for (auto _map_it : m_tracker){
    _map_it.second -> clear();
    delete _map_it.second;
  }
  m_tracker.clear();
}

int MNM_Routing_Fixed::init_routing(Path_Table *path_table)
{
  if (path_table == NULL){
    printf("Path table need to be set in Fixed routing.\n");
    exit(-1);
  }
  set_path_table(path_table);
  return 0;
}

int MNM_Routing_Fixed::update_routing(TInt timestamp)
{
  MNM_Origin *_origin;
  MNM_DMOND *_origin_node;
  TInt _node_ID, _next_link_ID;
  MNM_Dlink *_next_link;
  MNM_Veh *_veh;
  for (auto _origin_it = m_od_factory->m_origin_map.begin(); _origin_it != m_od_factory->m_origin_map.end(); _origin_it++){
    // printf("1.1\n");
    _origin = _origin_it -> second;
    _origin_node = _origin -> m_origin_node;
    _node_ID = _origin_node -> m_node_ID;
    for (auto _veh_it = _origin_node -> m_in_veh_queue.begin(); _veh_it!=_origin_node -> m_in_veh_queue.end(); _veh_it++){
      _veh = *_veh_it;
      // printf("1.2\n");
      if (_veh -> m_type != MNM_TYPE_STATIC){
        printf("Wrong vehicle type in routing!\n");
        exit(-1);
      }
      if (m_tracker.find(_veh) == m_tracker.end()){
        // printf("Registering!\n");
        register_veh(_veh);
        _next_link_ID = m_tracker.find(_veh) -> second -> front();
        _next_link = m_link_factory -> get_link(_next_link_ID);
        _veh -> set_next_link(_next_link);
        m_tracker.find(_veh) -> second -> pop_front();
      }
    }
  }
  MNM_Destination *_veh_dest;
  MNM_Dlink *_link;
  for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it ++){
    _link = _link_it -> second;
    _node_ID = _link -> m_to_node -> m_node_ID;
    // printf("2.1\n");
    for (auto _veh_it = _link -> m_finished_array.begin(); _veh_it!=_link -> m_finished_array.end(); _veh_it++){
      _veh = *_veh_it;
      _veh_dest = _veh -> get_destination();
      // printf("2.2\n");
      if (_veh_dest -> m_dest_node -> m_node_ID == _node_ID){
        if (m_tracker.find(_veh) -> second -> size() != 0){
          printf("Something wrong in fixed routing!\n");
          exit(-1);
        }
        _veh -> set_next_link(NULL);
        // m_tracker.erase(m_tracker.find(_veh));
      }
      else{
        // printf("2.3\n");
        if (m_tracker.find(_veh) == m_tracker.end()){
          printf("Vehicle not registered in link, impossible!\n");
          exit(-1);
        }
        if(_veh -> get_current_link() == _veh -> get_next_link()){
          _next_link_ID = m_tracker.find(_veh) -> second -> front();
          if (_next_link_ID == -1){
            printf("Something wrong in routing, wrong next link 2\n");
            printf("The node is %d, the vehicle should head to %d\n", (int)_node_ID, (int)_veh_dest -> m_dest_node -> m_node_ID);
            exit(-1);
          }
          _next_link = m_link_factory -> get_link(_next_link_ID);
          _veh -> set_next_link(_next_link);
          m_tracker.find(_veh) -> second -> pop_front();
        }
      }
    }
  }

  return 0;
}


int MNM_Routing_Fixed::register_veh(MNM_Veh* veh)
{
  TFlt _r = MNM_Ults::rand_flt();
  MNM_Pathset *_pathset = m_path_table -> find(veh -> get_origin() -> m_origin_node  -> m_node_ID) -> second
                        -> find(veh -> get_destination() -> m_dest_node  -> m_node_ID) -> second;
  MNM_Path *_route_path = NULL;
  // printf("1\n");
  for (MNM_Path *_path : _pathset -> m_path_vec){
    // printf("2\n");
    if (_path -> m_p >= _r) {
      _route_path = _path;
      break;
    }
    else{
      _r -= _path -> m_p;
    }
  }
  // printf("3\n");
  if (_route_path == NULL){
    printf("Wrong prabability!\n");
    exit(-1);
  }
  std::deque<TInt> *_link_queue = new std::deque<TInt>();
  std::copy(_route_path -> m_link_vec.begin(), _route_path -> m_link_vec.end(), std::back_inserter(*_link_queue));
  // printf("old link q is %d, New link queuq is %d\n", _route_path -> m_link_vec.size(), _link_queue -> size());
  m_tracker.insert(std::pair<MNM_Veh*, std::deque<TInt>*>(veh, _link_queue));
  return 0;
}

int MNM_Routing_Fixed::set_path_table(Path_Table *path_table)
{
  m_path_table = path_table;
  return 0;
}

int MNM_Routing_Fixed::add_veh_path(MNM_Veh* veh, std::deque<TInt> *link_que)
{
  std::deque<TInt> *_new_link_que = new std::deque<TInt>();
  std::copy(link_que->begin(), link_que->end(), std::back_inserter(*_new_link_que));
  m_tracker.insert(std::pair<MNM_Veh*, std::deque<TInt>*>(veh, _new_link_que));
  return 0;
}
