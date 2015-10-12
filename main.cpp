
#include <iostream>
#include "model.h"

int main() {
  
  MarkovChain a("input2.txt");
  a.read();
  a.buildTable();
  a.generate();
  return 0;
}