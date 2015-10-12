#include "model.h"

MarkovChain::MarkovChain(std::string exfilename, uint32_t exorder):filename(exfilename), order(exorder) {  
  generator.seed(9);
}

void MarkovChain::setOrder(uint32_t exorder){
  order = exorder;
}



void MarkovChain::read() {
  std::vector<std::string> data2; //read to tmp vector

  std::ifstream readFile(filename.c_str());
  std::copy(std::istream_iterator<std::string>(readFile), {}, back_inserter(data2));
  

  // post processing
  std::string result;
  
  for (uint32_t i = 0; i < data2.size(); ++i) {
  
  // punctuation mark can be without space after   << FIXME
  // Remove all punct
    data2.at(i).erase( std::remove_if(data2.at(i).begin(), data2.at(i).end(), &ispunct), 
                      data2.at(i).end()
                     );
  
  //Remove all numbers

    data2.at(i).erase ( std::remove_if(data2.at(i).begin(), data2.at(i).end(), &isdigit), 
                      data2.at(i).end()
                    );
    result = data2.at(i);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);  // it's not working with russian symbols
    if (!result.empty()) {
      data.push_back(result);    
      word_frequency[result] ++;  // also count freq
    }
    
    result.clear();

  }
  
}


MarkovChain::~MarkovChain() {

}


void MarkovChain::fit() {
  
}


void MarkovChain::buildTable() { 

  // now only two-order chain
  // 
  
  uint32_t i = 0, j = 0;
  std::string tmp1 = "", tmp2 = "";
  std::list<std::string> fill;
  std::map<std::string, int> tmap;


  for (i = 0; i < data.size()-1; ++i) {
    tmp1 = data.at(i);
    tmp2 = data.at(i+1);

    for (j = 0; j < data.size()-2; ++j) {
      if ((data.at(j) == tmp1) && (data.at(j+1) == tmp2)) {
        tmap[data.at(j+2)]++;
      }  
    }

    std::map<std::string, int>::iterator itb = tmap.begin(), ite = tmap.end();
    for (; itb != ite; ++itb){ 
      if (itb->second == 0){

        std::cout << "ololo " << itb -> first << " " << itb -> second  << std::endl;
      }
    }

    two_order_chain[std::make_pair(tmp1, tmp2)] = tmap;
    tmap.clear();
  }

    
}


void MarkovChain::generate() {
  std::map< std::pair<std::string, std::string>, 
            std::map<std::string, int> > ::iterator itb = two_order_chain.begin(), 
                                                    ite = two_order_chain.end();


  std::string start_word1 = data.at(1);
  std::string start_word2 = data.at(2);

  std::map<std::string, int> tmap;

  for(; itb != ite; ++itb) {
    tmap = two_order_chain[std::make_pair(start_word1, start_word2)];
    //std::cout << tmap.begin()->first;
    getNextWord(tmap);
  }

}



std::string MarkovChain::getNextWord(std::map<std::string, int>& bag) {
  
  int sum = 0;
  std::string s;
  std::list <int> values;
  for (std::map<std::string, int>::iterator itb = bag.begin(), ite = bag.end(); itb != ite; ++itb ){
    sum += itb->second;
    values.push_back(itb->second);
  }

  
  std::uniform_real_distribution<double> distribution(0, 1);
  for (std::list<int>::iterator itb = values.begin(), ite = values.end(); itb != ite; ++itb ){
    *itb = (*itb)/sum; // normalize list
  }


}