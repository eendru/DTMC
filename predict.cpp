#include <getopt.h>

#include "model.h"


void help(){
  std::cout << "    -f file that stores initial passage of words" << std::endl;
  std::cout << "    -m file that stores trained model" << std::endl;
  std::cout << "    -K numbers of words" << std::endl;
  std::cout << "    -h see this help" << std::endl;
}

void run(std::string& model, std::string& init_passage, uint64_t& K) {

  MarkovChain mc;
  mc.LoadFromFile(model);
  mc.PrintTable();
  mc.Predict();

}

int main(int argc, char **argv) {

  std::string model;
  std::string init_passage;
  uint64_t K = 0;

  if (argc < 4){
    help();
    exit(-1);
  }

  else {
    int opt;
    while((opt = getopt(argc, argv, "f:m:K:h")) != -1) {
      switch(opt){
        
        case 'f':
          init_passage = std::string(optarg);
          break;

        case 'h':
          help();
          break;
        case 'm':
          model = std::string(optarg);
          break;
        case 'K':
          K = std::atol(optarg);
          break;

      }
    }
  }

  if (model.empty() || K == 0)
    exit(-1);
  else {

    try{
      run(model, init_passage, K);
    } catch(std::exception &e) {
        std::cout << "Aborted by exception: " << e.what() << std::endl;
    }

  }



  return 0;
}