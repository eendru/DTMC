
#include <iostream>
#include <getopt.h>
#include "model.h"



void help() {
  std::cout << " -f file with data for Markov Chain" << std::endl;
  std::cout << " -m file for store model" << std::endl;
  std::cout << " -n order of chain" << std::endl;


}

void run (std::string filename, int order, std::string output_model) {


  try {

    MarkovChain a(order);
    a.set_filename(filename);
    a.Read();
    a.Fit();

    if (!a.DumpToFile(output_model))
      throw std::string("Empty chain");
    a.PrintTable();
  }
  catch (std::exception &e) {
      std::cout << "Aborted: " << e.what() << std::endl;
  }

}

int main(int argc, char **argv) {

  std::string filename;
  std::string output_model;
  int order = 0;

  if(argc < 3 ) {
    help();
    exit(-1);
  }
  else {
    int opt;
    while((opt = getopt(argc, argv, "f:n:m:h")) != -1) {
      switch(opt){
        
        case 'f':
          filename = optarg;
          break;

        case 'n':
          
          try{
            order = std::atoi(optarg);
          }catch(...){
              throw "Something bad with conversation from string to int for -n option" ; // pass to next catch
          }

          break;

        case 'm':

          output_model = std::string(optarg);
          break;

        case 'h':
          help();
          break;

        default:
          std::cout << "type -h for help" << std::endl;
          exit(-1);
      }
    }
  }


  if(output_model.empty())
    output_model = std::string("out.bin");

  if (!filename.empty())
    run(filename, order, output_model);
    
  

  return 0;
}


//  add size type