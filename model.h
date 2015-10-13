#include <string>
#include <list>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <locale>
#include <iterator>
#include <iomanip>

using std::string;
using std::vector;


class MarkovChain {

public:

  MarkovChain(std::string exfilename = std::string("input.txt"), uint32_t exorder = 2);
  ~MarkovChain();

  void setOrder(uint32_t exorder);


  void read();
  void fit();

  void buildTable();
  void printTable();
  void generate();

  std::string getNextWord( std::map<std::string, int>&);

private:
  std::string filename;
  std::vector<std::string> data;
  std::map<std::string, int> word_frequency;

  uint32_t order;

  std::map<std::list<std::string>, std::map<std::string, int> > norder_chain;
  
  std::map< std::pair<std::string, std::string>, 
            std::map<std::string, int> > 
          two_order_chain;
  
  std::default_random_engine generator; // for uniform distribution
};



//
//Get next word with according to the distribution of objects in map
