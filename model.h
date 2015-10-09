#include <string>
#include <list>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>

using std::string;
using std::vector;

class MarkovChain {

public:

  MarkovChain(std::string exfilename = std::string("input.txt"), uint32_t exorder = 2);
  ~MarkovChain();

  void setOrder(uint32_t exorder);

  void Read();
private:
  std::string filename;
  std::vector<std::string> data;

  uint32_t order;
};