#include "scaffold.h"

scaffold::Node **scaffold::newArray(int size) {
  scaffold::Node **out = new scaffold::Node*[size];
  for (int i = 0;  i < size;  i++)
    out[i] = new scaffold::InertNode;
  return out;
}

std::string scaffold::header(scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  for (int i = 0;  i < scaffoldSize;  i++)
    out += scaffoldArray[i]->header(2);
  return out;
}

std::string scaffold::init(scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  for (int i = 0;  i < scaffoldSize;  i++)
    out += scaffoldArray[i]->init(2);
  return out;
}
