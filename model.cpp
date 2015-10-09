#include "model.h"

MarkovChain::MarkovChain(std::string exfilename, uint32_t exorder):filename(exfilename), order(exorder) {  
}

void MarkovChain::setOrder(uint32_t exorder){
  order = exorder;
}


void MarkovChain::Read() {

  std::ifstream readFile(filename.c_str());
  std::copy(std::istream_iterator<std::string>(readFile), {}, back_inserter(data));
  std::cout << "Vector Size is now " << data.size() << std::endl;
  std::cout << data.at(1);
}
MarkovChain::~MarkovChain() {

}

