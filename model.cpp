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
  std::map< std::pair<std::string, std::string>,
            std::map<std::string, int> >  :: iterator itb = two_order_chain.begin(),
                                                      ite = two_order_chain.end();
  std::map<std::string, int>::iterator itmb, itme;

  for (; itb != ite; ++itb) {
    std::cout << "[" << itb->first.first << ", " << itb->first.second << "] = {";
    for (itmb = itb->second.begin(), itme = itb->second.end(); itmb != itme; ++itmb) {
      std::cout  << itmb->first << "=" << itmb->second << ", ";
    }
    std::cout << "}" << std::endl;
  }
}

void MarkovChain::fit() {
}


void MarkovChain::buildTable() {
// now only two-order chain

  uint32_t i = 0, j = 0;
  std::map<std::string, int> tmap;

  tmap[data.at(0)] = 1;
  std::string tmp1 = "", tmp2 = "";
  two_order_chain[std::make_pair(tmp1, tmp2)] = tmap;  // the first map entry will have
                                                       // a list of n empty strings
                                                       // mapped to the first word
                                                       // in the training sequence.

  tmap.clear();

  tmp2 = data.at(0);
  tmp1 = "";

  tmap[data.at(1)] = 1;
  two_order_chain[std::make_pair(tmp1, tmp2)] = tmap;  // second element
                                                       // non-full
                                                       // like "", "__"
  tmap.clear();

  std::map<std::string, int>::iterator itb, ite;
  for (i = 1; i < data.size(); ++i) {
    tmp1 = tmp2;
    tmp2 = data.at(i);

    for (j = 0; j < data.size()-1; ++j) {
      if ((data.at(j) == tmp1) && (data.at(j+1) == tmp2)) {
        if ( j + 2 < data.size() )
          tmap[data.at(j+2)]++;

        else
          tmap[""]++;
      }
    }

    if (!two_order_chain.count(std::make_pair(tmp1, tmp2)))  // check, if we already count
      two_order_chain[std::make_pair(tmp1, tmp2)] = tmap;

    tmap.clear();
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


std::string MarkovChain::getNextWord(std::map<std::string, int>& bag) {
  int sum = 0;  // sum all elements in map
  for (std::map<std::string, int>::iterator itb = bag.begin(), ite = bag.end(); itb != ite; ++itb )
    sum += itb->second;

  std::map <std::string, double> normalize_values;  // normalized elements
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
