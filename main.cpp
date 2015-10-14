
#include <iostream>
#include "model.h"

int main() {
  
  MarkovChain a("input2.txt", 2);

  a.read();
  a.fit();
  //a.printTable();
  a.predict();
  
  MarkovChain b("input2.txt", 12);


  b.read();
  b.fit();
  //.printTable();
  b.predict();

  return 0;
}


//  add size type