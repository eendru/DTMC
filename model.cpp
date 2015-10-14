#include "model.h"

MarkovChain::MarkovChain(std::string exfilename, uint32_t exorder):filename(exfilename), order(exorder) {
  generator.seed(9);
}

void MarkovChain::setOrder(uint32_t exorder) {
  order = exorder;
}


void MarkovChain::read() {
  std::vector<std::string> data2;  // read to tmp vector

  std::ifstream readFile(filename.c_str());
  std::copy(std::istream_iterator<std::string>(readFile), {}, back_inserter(data2));

  // post processing
  std::string result;
  for (uint32_t i = 0; i < data2.size(); ++i) {
    // punctuation mark can be without space after   << FIXME
    // Remove all punct
    data2.at(i).erase(std::remove_if(data2.at(i).begin(), data2.at(i).end(), &ispunct),
                      data2.at(i).end() );

    // Remove all numbers
    data2.at(i).erase(std::remove_if(data2.at(i).begin(), data2.at(i).end(), &isdigit),
                      data2.at(i).end() );

    result = data2.at(i);
    // convert to lower case
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);  // it's not working with russian symbols
    if (!result.empty()) {
      data.push_back(result);
      word_frequency[result]++;  // also count freq
    }
    result.clear();
  }
}


MarkovChain::~MarkovChain() {
}


void MarkovChain::printTable() {
  std::map<std::list<std::string>, std::map<std::string, int> >::iterator itb, ite;

  itb = norder_chain.begin();
  ite = norder_chain.end(); 

  std::map<std::string, int>::iterator itmb, itme;
  std::list<std::string>::iterator itlb, itle;
  std::list<std::string> tmp;

  for (; itb != ite; ++itb) {
    tmp = itb->first;
    itlb = tmp.begin();
    itle = tmp.end();
    
    std::cout << "[";
    for (; itlb != itle; ++itlb) 
      std::cout << *itlb << ", ";
    
    std::cout << "] = {";

    for (itmb = itb->second.begin(), itme = itb->second.end(); itmb != itme; ++itmb) 
      std::cout  <<"\"" << itmb->first <<"\"" << "=" << itmb->second << ", ";
    
    std::cout << "}" << std::endl;
  }
}

void MarkovChain::fit() {
  
  std::list<std::string> last_n_strings;
  std::list<std::string> data_strings;

  //  fill first element in norder_chain
  last_n_strings.resize(order, "");         // list of empty strings (?)
  std::map<std::string, int> chain_key;
  chain_key[data.at(0)] = 1;
  norder_chain[last_n_strings] = chain_key;

  chain_key.clear();
  uint64_t i = 0, j = 0, k = 0;


  
  std::list<std::string>::iterator itlb;
  std::vector<std::string>::iterator itc, itvcurrent;

  for (i = 0; i < data.size(); ++i) {
    
    itlb = last_n_strings.begin();       // left circle-shift for last_n_strings list
    ++itlb;                              
    last_n_strings.splice(itlb, last_n_strings, last_n_strings.end());
    
    *(last_n_strings.rbegin()) = data.at(i); // last element must be update

    if (i < order-1)
      chain_key[data.at(i+1)] = 1; // if it's first n=order elements
                                   //

    else {
    // under fill a tmp-list of words from data-vector, size of list = order
    // after that, compare this list and last_n_words list
    // if they are equal value of word in chain_map
    // must be increment, id of this word - next from lat element in tmp-list
    // 
    // for example: last_n_strings = [another, sentence], order=2
    // data vector = [this, is, another, sentence, this, is, another, sentence]
    // current_position = data_vector.begin(), tmp-list=[this, is]
    // as result in norder_chain for [another, sentence]
    // tmp-list and last_n_string are not equal
    // 
    // After two iterations tmp-list would be [another, sentence]
    // tmp_list and last_n_strings are equal. => chain_key["this"]++
    // "this" is a next word in data-vector after "sentence"

      for (itc = data.begin(); itc != data.end() - order + 1; ++itc) {
        itvcurrent = itc;  // from current position get n=order-elements
        for (k = 0; k < order; ++itvcurrent, ++k )
          data_strings.push_back(*itvcurrent);
        
        if(std::equal(data_strings.begin(), data_strings.end(), last_n_strings.begin()))
          // if it's not last occurence
          // else, need map empty string ""
          // FIXME
          if (std::distance(itc, data.end() - order) > 0)
            chain_key[*(itvcurrent)]++;
          else
            chain_key[""]++;

        data_strings.clear();
      }
    }

    if(!norder_chain.count(last_n_strings))
      norder_chain[last_n_strings] = chain_key;

    chain_key.clear();
  }

}

  
void MarkovChain::generate() {
  std::map< std::pair<std::string, std::string>,
            std::map<std::string, int> > ::iterator itb = two_order_chain.begin(),
                                                    ite = two_order_chain.end();

  //  std::cout << std::endl<< std::endl;
  //  printTable();

  std::string start_word1 = "";
  std::string start_word2 = "";

  std::map<std::string, int> tmap;
  std::map<std::string, int>::iterator itbt = tmap.begin(), itet = tmap.end();

  do {
    tmap = two_order_chain[std::make_pair(start_word1, start_word2)];
    start_word1 = start_word2;
    start_word2 = getNextWord(tmap);
    std::cout <<  start_word2 << " ";
  } while (!start_word2.empty());

  std::cout << std::endl;
}

void MarkovChain::predict() {
  std::list<std::string> start_word;
  start_word.resize(order, "");
  
  std::map<std::string, int> tmap;
  std::map<std::string, int>::iterator itbt = tmap.begin(), itet = tmap.end();
  std::list<std::string>::iterator itlb;

  std::string next;
  do {
    tmap = norder_chain[start_word];
    next = getNextWord(tmap); ;      
    itlb = start_word.begin();       // left circle-shift for start_word list
    ++itlb;                              
    start_word.splice(itlb, start_word, start_word.end());
    *(start_word.rbegin()) = next; 
    
    
    std::cout <<  next  << std::endl;


  } while(!next.empty());


}


std::string MarkovChain::getNextWord(std::map<std::string, int>& bag) {
  int sum = 0;  // sum all elements in map
  for (std::map<std::string, int>::iterator itb = bag.begin(), ite = bag.end(); itb != ite; ++itb )
    sum += itb->second;

  std::map <std::string, double> normalize_values;  // normalized map
  for (std::map<std::string, int>::iterator itb = bag.begin(), ite = bag.end(); itb != ite; ++itb )
    normalize_values[itb->first] = (itb->second)/static_cast<double>(sum);

  std::uniform_real_distribution<double> distribution(0, 1);
  double random_value = distribution(generator);  // random value from 0, 1 with uniform dist
  std::map<std::string, double>::iterator itbm = normalize_values.begin();

  do {
    random_value -= itbm->second;
    if (random_value > 0.0)
      ++itbm;     // What's about out of range?
  } while (random_value > 0.0);


  return itbm->first;
}


void printList(std::list<std::string> &l) {
  std::list<std::string>::iterator itb = l.begin(), ite = l.end();
  for (;itb != ite; ++itb)
    std::cout << "<" << *itb << "> ";

  std::cout << std::endl;
}