#ifndef IO_H
#define IO_H

#include "Snap.h"
#include "ults.h"
#include "factory.h"

#include <string>
#include <vector>

class MNM_Node_Factory;

class MNM_IO
{
public:
  static int  build_node_factory(std::string file_folder, MNM_ConfReader *conf_reader, MNM_Node_Factory *node_factory);
private:
  static std::vector<std::string> split(const std::string &text, char sep);
};

#endif