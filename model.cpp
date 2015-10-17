#include "model.h"
#include "serialize.pb.h"

MarkovChain::MarkovChain( uint32_t exorder): order(exorder) {
  generator.seed(9);
}

void MarkovChain::set_order(uint32_t& exorder) {
  order = exorder;
}
void MarkovChain::set_filename(std::string& filename) {
  filename_ = filename;
}


void MarkovChain::Read() {
  std::vector<std::string> data2;  // read to tmp vector
  std::ifstream input_file(filename_.c_str());
  std::copy(std::istream_iterator<std::string>(input_file), {}, back_inserter(data2));

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


void MarkovChain::PrintTable() {
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


void MarkovChain::Fit() {

  if (!data.size())
    throw "Empty data, can't build chain";

  std::list<std::string> last_n_strings;
  std::list<std::string> data_strings;  // uses, when compare last_n_strings 
                                        // and strings in data

  
  last_n_strings.resize(order, "");         // list of empty strings (?)
  std::map<std::string, int> chain_key;
  
  
  chain_key[data.at(0)] = 1;
  norder_chain[last_n_strings] = chain_key; // first element

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
                                   // first n=order elements are
                                   // not complete

    else {
    // under fill a tmp-list of words from data-vector, size of list = order
    // after that, compare this list and last_n_words list
    // if they are equal value of word in chain_map
    // must be increment, id of this word 
    // following the last element in tmp-list
    // 
    // for example: last_n_strings = [another, sentence], order=2
    // data vector = [this, is, another, sentence, this, is, another, sentence]
    // current_position = data_vector.begin(), tmp_list=[this, is]
    // tmp-list and last_n_string are not equal
    // 
    // After two iterations tmp-list would be [another, sentence]
    // tmp_list and last_n_strings are equal. => chain_key["this"]++

    // "this" is a next word in data-vector
    // Next two iterations, tmp_list wolud be also [another, sentence]
    // Next word for sentence is "" => chain_key[""]++
    // 
    // As result for last_n_words = [another, sentence]
    // norder_chain[last_n_words] = {"this"->1, ""->1}

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

  
void MarkovChain::Predict() {
  std::list<std::string> start_word;
  start_word.resize(order, "");
  
  std::map<std::string, int> tmap;
  std::map<std::string, int>::iterator itbt = tmap.begin(), itet = tmap.end();
  std::list<std::string>::iterator itlb;

  

  std::string next;
  do {
    tmap = norder_chain[start_word];
    next = GetNextWord(tmap); ;      
    itlb = start_word.begin();       // left circle-shift for start_word list
    ++itlb;                              
    start_word.splice(itlb, start_word, start_word.end());
    *(start_word.rbegin()) = next; 
    std::cout <<  next  << std::endl;

  } while(!next.empty());




}


std::string MarkovChain::GetNextWord(std::map<std::string, int>& bag) {
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


void PrintList(std::list<std::string> &l) {
  std::list<std::string>::iterator itb = l.begin(), ite = l.end();
  for (;itb != ite; ++itb)
    std::cout << "<" << *itb << "> ";

  std::cout << std::endl;
}



bool MarkovChain::DumpToFile(std::string& filename) {
  std::map<std::list<std::string>, std::map<std::string, int> > ::iterator itb = norder_chain.begin(),
                                                                           ite = norder_chain.end();

  std::fstream outfile(filename.c_str(), std::ios::out|std::ios::binary);
  
  if (norder_chain.empty())
    return false;



  std::list<std::string> nwords_tmp;
  std::list<std::string>::iterator it_words_b, it_words_e;
  
  std::map<std::string, int> markov_key_tmp;
  std::map<std::string, int>::iterator it_mkt_b, it_mkt_e;


// all data for serialize will be prefixed by tos_
  mcserialize::LastNWords *tos_words;
  mcserialize::WordFrequency *tos_wf;
  mcserialize::WordFrequency_Entry *tos_entry;

  mcserialize::Entry *tos_chain_entry;
  mcserialize::NOrderChain *tos_norder_chain = new mcserialize::NOrderChain();

  int i = 0;
  for (; itb != ite; ++itb, ++i) {
    nwords_tmp = itb->first;
    markov_key_tmp = itb->second;

    it_words_b = nwords_tmp.begin();
    it_words_e = nwords_tmp.end();

    tos_words = new mcserialize::LastNWords();
    for (;it_words_b != it_words_e; ++it_words_b) 
      tos_words->add_word(*it_words_b);

    it_mkt_b = markov_key_tmp.begin();
    it_mkt_e = markov_key_tmp.end();
    tos_wf = new mcserialize::WordFrequency();

    for (; it_mkt_b != it_mkt_e; ++it_mkt_b) {
      tos_entry = tos_wf->add_entry();
      tos_entry->set_key(it_mkt_b->first);
      tos_entry->set_val(it_mkt_b->second);
    }

    tos_chain_entry = tos_norder_chain->add_entry();
    tos_chain_entry->set_allocated_word_frequency(tos_wf);
    tos_chain_entry->set_allocated_lastnwords(tos_words);
    
  }

  tos_norder_chain->set_order(order);
  tos_norder_chain->SerializeToOstream(&outfile);

  delete tos_norder_chain;  // other's allocated obj also will be destroyed 

  return true;
}


bool MarkovChain::LoadFromFile(std::string& filename) {
  std::ifstream input_binary(filename, std::ios::binary);
  // All variables which use in deserialize will be prefixed by des
  mcserialize::NOrderChain *des_norder_chain = new mcserialize::NOrderChain();

  std::map<std::string, int> mc_value_tmp;
  std::list<std::string> mc_key_tmp;

  norder_chain.clear();
  des_norder_chain->ParseFromIstream(&input_binary);
  order = des_norder_chain->order(); 

  if (!des_norder_chain->entry_size())
    return false;

  mcserialize::Entry des_chain_entry;
  mcserialize::LastNWords des_lastnwords;
  mcserialize::WordFrequency des_wordfreq;


  for (int i = 0; i < des_norder_chain->entry_size(); ++i) {
    des_chain_entry = des_norder_chain->entry(i);    

    des_wordfreq = des_chain_entry.word_frequency();
    des_lastnwords = des_chain_entry.lastnwords();

    for (int j = 0; j < des_lastnwords.word_size(); ++j)
      mc_key_tmp.push_back(des_lastnwords.word(j));

    for (int j = 0; j < des_wordfreq.entry_size(); ++j)
      mc_value_tmp[des_wordfreq.entry(j).key()] = des_wordfreq.entry(j).val();

    norder_chain[mc_key_tmp] = mc_value_tmp;

    mc_key_tmp.clear();
    mc_value_tmp.clear();
  }

  return true;
}


