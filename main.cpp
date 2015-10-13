
#include <iostream>
#include "model.h"

int main() {
  
  MarkovChain a("input2.txt", 2);

  a.read();
  a.fit();
  a.printTable();
  //a.generate();
  return 0;
}


//  add size type