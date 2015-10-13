
#include <iostream>
#include "model.h"

int main() {
  
  MarkovChain a("input.txt");
  a.read();
  a.buildTable();
  a.generate();
  return 0;
}