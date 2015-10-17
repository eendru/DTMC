#include <string>
#include <list>
#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <locale>
#include <iterator>
#include <iomanip>
#include <array>

using std::string;
using std::vector;


class MarkovChain {
 public:
  explicit MarkovChain(uint32_t exorder = 2);
  ~MarkovChain();

  void set_order(uint32_t exorder);
  void set_filename(std::string filename);

  void Read();
  void Fit();
  void Predict(std::string init_passage, uint64_t K);

  std::string GetNextWord(std::map<std::string, int>&);
  void PrintTable();

  bool DumpToFile(std::string);
  bool LoadFromFile(std::string);

 private:
  std::string filename_;
  std::vector<std::string> data_;
  std::map<std::string, int> word_frequency;
  uint32_t order;
  std::map<std::list<std::string>, std::map<std::string, int> > norder_chain;
  std::default_random_engine generator;   // for uniform distribution
};

void printList(std::list<std::string> &l);
