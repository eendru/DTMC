#include "model.h"
#include "serialize.pb.h"


MarkovChain::MarkovChain(uint32_t order): order_(order) {
  generator.seed(6);   // initial value for random engine
}

void MarkovChain::set_order(uint32_t order) {
  order_ = order;
}

uint32_t MarkovChain::order()const {
  return order_;
}



void MarkovChain::Read(std::string filename) {
  std::list<std::string> data2;  // read to tmp vector

  std::ifstream input_file(filename.c_str());
  if (!input_file.good()) {
    std::cout << "Troubles with file, maybe file doesn't exist" << std::endl;
    throw;
  }
  //  read data from file to data2
  try {
    std::copy(std::istream_iterator<std::string>(input_file), {}, back_inserter(data2));
  } catch(...) {
    std::cout << "Something bad with copy data from file to vector" << std::endl;
    throw;
  }

  RemoveNumbers(data2);
  ToLowerCase(data2);
  RemovePunct(data2);

  std::string cleaned_word;
  std::list<std::string>::iterator itb = data2.begin(), ite = data2.end();
  for (; itb != ite; ++itb) {
    cleaned_word = *itb;
    if (!cleaned_word.empty())
      data_.push_back(cleaned_word);
    cleaned_word.clear();
  }
  input_file.close();
}


MarkovChain::~MarkovChain() {
}


void MarkovChain::PrintTable() const {
  std::map<std::list<std::string>, std::map<std::string, uint32_t> >::const_iterator itb, ite;

  itb = norder_chain.cbegin();
  ite = norder_chain.cend();

  std::map<std::string, uint32_t>::const_iterator itmb, itme;
  std::list<std::string>::const_iterator itlb, itle;

  std::list<std::string> tmp;

  for (; itb != ite; ++itb) {
    tmp = itb->first;
    itlb = tmp.cbegin();
    itle = tmp.cend();

    std::cout << "[";
    for (; itlb != itle; ++itlb)
      std::cout << *itlb << ", ";

    std::cout << "] = {";

    for (itmb = itb->second.cbegin(), itme = itb->second.cend(); itmb != itme; ++itmb)
      std::cout  <<"\"" << itmb->first <<"\"" << "=" << itmb->second << ", ";

    std::cout << "}" << std::endl;
  }
}


void MarkovChain::Fit() {
  if (!data_.size()) {
    std::cout << "Empty data, can't build chain"<< std::endl;
    throw;
  }

  std::list<std::string> last_n_strings;
  std::list<std::string> data_strings;       // uses, when compare last_n_strings
                                             // and strings in data

  last_n_strings.resize(order_, "");          // list of empty strings 
  std::map<std::string, uint32_t> chain_key;

  // create first element
  chain_key[data_.at(0)] = 1;
  norder_chain[last_n_strings] = chain_key;

  chain_key.clear();

  uint64_t i = 0, j = 0, k = 0;
  std::list<std::string>::iterator itlb;
  std::vector<std::string>::iterator itc, itvcurrent;

  for (i = 0; i < data_.size(); ++i) {
    itlb = last_n_strings.begin();       // left circle-shift for last_n_strings list
    ++itlb;
    last_n_strings.splice(itlb, last_n_strings, last_n_strings.end());

    *(last_n_strings.rbegin()) = data_.at(i);  // last element must be update

    if (i < order_ - 1) {
      chain_key[data_.at(i+1)] = 1;  // if it's first n=order elements
                                     // first n=order elements are
                                     // not complete
    } else {
    // under, fill a tmp-list of words from data-vector, size of list = order
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
    //
    // "this" is a next word in data-vector
    // Next two iterations, tmp_list wolud be also [another, sentence]
    // Next word for sentence is "" => chain_key[""]++
    //
    // As result for last_n_words = [another, sentence]
    // norder_chain[last_n_words] = {"this"->1, ""->1}

      for (itc = data_.begin(); itc != data_.end() - order_ + 1; ++itc) {
        itvcurrent = itc;  // from current position get n=order-elements
        for (k = 0; k < order_; ++itvcurrent, ++k )
          data_strings.push_back(*itvcurrent);

        if (std::equal(data_strings.begin(), data_strings.end(), last_n_strings.begin())) {
          if (std::distance(itc, data_.end() - order_) > 0)
            chain_key[*(itvcurrent)]++;
          else
            chain_key[""]++;
        }

        data_strings.clear();
      }
    }
    // if last_n_strings haven't been register in map
    if (!norder_chain.count(last_n_strings))
      norder_chain[last_n_strings] = chain_key;

    chain_key.clear();
  }
}


void MarkovChain::Predict(std::string init_passage_filename, uint64_t K)  {
  std::vector<std::string> init_passage;
  std::ifstream input_file(init_passage_filename.c_str());

  if (!input_file.good()) {
    std::cout << "Troubles with file, maybe file doesn't exist" << std::endl;
    throw;
  }

  std::copy(std::istream_iterator<std::string>(input_file), {}, back_inserter(init_passage));

  std::list<std::string> start_word;
  start_word.resize(order_, "");
  if (!init_passage.empty())   // otherwise empty list will be used
    std::copy_n(init_passage.begin(), order_, start_word.begin());

  // to lookup throw markov chain need cleaned lowercase text
  RemovePunct(start_word);
  RemoveNumbers(start_word);
  ToLowerCase(start_word);


  std::map<std::string, uint32_t> markov_chain_key;
  std::list<std::string>::iterator itlb;
  std::string next = "";

  try {
    for (uint64_t i = 0; i < K; ++i) {
      markov_chain_key = norder_chain.at(start_word);
      next = GetNextWord(markov_chain_key);
      if (next.empty()) {
        start_word.clear();
        start_word.resize(order_, "");
        std::cout << std::endl;
      } else {
        itlb = start_word.begin();       // left circle-shift for start_word list
        ++itlb;
        start_word.splice(itlb, start_word, start_word.end());
        *(start_word.rbegin()) = next;
        std::cout <<  next  << " ";
      }
    }
    std::cout << std::endl;
  }
  catch(std::out_of_range &e) {
    std::cout << "It seems, that i can't find in transition table similar words" << std::endl;
    throw;
  }
}

std::string MarkovChain::GetNextWord(std::map<std::string, uint32_t>& bag) {
  uint64_t sum = 0;
  std::map <std::string, double> normalize_values = NormalizeMap(bag);

  std::uniform_real_distribution<double> distribution(0, 1);
  double random_value = distribution(generator);  // random value from 0, 1 with uniform dist
  std::map<std::string, double>::iterator itb_nv = normalize_values.begin();

  do {
    random_value -= itb_nv->second;
    if (random_value > 0.0)
      ++itb_nv;
  } while (random_value > 0.0);

  return itb_nv->first;
}


bool MarkovChain::DumpToFile(std::string filename) {
  std::map<std::list<std::string>, std::map<std::string, uint32_t> >::iterator itb = norder_chain.begin(),
                                                                                ite = norder_chain.end();

  std::fstream outfile(filename.c_str(), std::ios::out|std::ios::binary);

  if (norder_chain.empty())
    return false;

  std::list<std::string> nwords_tmp;
  std::list<std::string>::iterator it_words_b, it_words_e;

  std::map<std::string, uint32_t> markov_key_tmp;
  std::map<std::string, uint32_t>::iterator it_mkt_b, it_mkt_e;

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
    for (; it_words_b != it_words_e; ++it_words_b)
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

  tos_norder_chain->set_order(order_);
  tos_norder_chain->SerializeToOstream(&outfile);
  outfile.close();

  delete tos_norder_chain;  // other's allocated obj also will be destroyed

  return true;
}


bool MarkovChain::LoadFromFile(std::string filename) {
  std::ifstream input_binary(filename, std::ios::binary);
  // All variables which use in deserialize will be prefixed by des
  mcserialize::NOrderChain *des_norder_chain = new mcserialize::NOrderChain();

  std::map<std::string, uint32_t> mc_value_tmp;
  std::list<std::string> mc_key_tmp;

  norder_chain.clear();
  des_norder_chain->ParseFromIstream(&input_binary);
  set_order(des_norder_chain->order());

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

  input_binary.close();
  return true;
}


void RemoveNumbers(std::list<std::string>& input) {
  std::list<std::string>::iterator itb = input.begin(), ite = input.end();
  for (; itb != ite; ++itb)
    itb->erase(std::remove_if(itb->begin(), itb->end(), isdigit), itb->end());
}

void RemovePunct(std::list<std::string>& input) {
  std::list<std::string>::iterator itb = input.begin(), ite = input.end();
  for (; itb != ite; ++itb)
    itb->erase(std::remove_if(itb->begin(), itb->end(), ispunct), itb->end());
}

void ToLowerCase(std::list<std::string>& input) {
  std::list<std::string>::iterator itb = input.begin(), ite = input.end();
  for (; itb != ite; ++itb)
    std::transform(itb->begin(), itb->end(), itb->begin(), ::tolower);
}

uint64_t SumOfMap(std::map<std::string, uint32_t>& input) {
  uint64_t result = 0;
  for (std::map<std::string, uint32_t>::const_iterator itb = input.cbegin(), ite = input.cend(); itb != ite; ++itb )
    result += itb->second;
  return result;
}

std::map<std::string, double> NormalizeMap(std::map<std::string, uint32_t> &input) {
  uint64_t sum = SumOfMap(input);
  std::map <std::string, double> normalize_values;  // normalized map
  for (std::map<std::string, uint32_t>::const_iterator itb = input.cbegin(), ite = input.cend(); itb != ite; ++itb )
    normalize_values[itb->first] = (itb->second)/static_cast<double>(sum);

  return normalize_values;
}
