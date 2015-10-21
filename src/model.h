#include <string>
#include <list>
#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <locale>
#include <iterator>
#include <iomanip>

using std::string;
using std::vector;


/*!
 * @brief MarkovChain class provides the creation of Markov Chain and 
 * provides the use of Markov Chain to generate human-like sentence and text   
 */
class MarkovChain {
 public:
  explicit MarkovChain(uint32_t order = 2);
  ~MarkovChain();

  void set_order(uint32_t order);
  uint32_t order() const;

  /*!
   * Read file in ASCII encode and store it in std::vector<std::string> data_
   * @param std::string  filename
   */
  void Read(std::string);

  /*!
   * Buld n-order markov chain
   */
  void Fit();

  /*!
   * function which uses the previously generated markov chain model to 
   * build next word. The result is printed to std::cout
   * @param init_passage_file file conataining the initial passage of n words
   * @param K                 numbers of words that need to build
   */
  void Predict(const std::string init_passage_file, uint64_t K);
  /*!
   * Print transition table of markov-chain 
   */
  void PrintTable() const;

  /*!
   * Save markov chain model to file
   * This function uses protobuf
   * @param  filename name of the file where to save chain
   * @return          true if everything okay
   */
  bool DumpToFile(const std::string filename);
  /*!
   * Load markov chain from file
   * This function uses protobuf
   * @param  filename name of the file where to load chain
   * @return          true if everything okay
   */
  bool LoadFromFile(const std::string filename);

 private:
  std::string GetNextWord(std::map<std::string, uint32_t>&);

  std::vector<std::string> data_;
  uint32_t order_;
  std::map<std::list<std::string>, std::map<std::string, uint32_t> > norder_chain;

  uint32_t seed_;
  std::default_random_engine generator;   // for uniform distribution
};

std::map<std::string, double> NormalizeMap(std::map<std::string, uint32_t> &input);
uint64_t SumOfMap(std::map<std::string, uint32_t>& input);

void RemoveNumbers(std::list<std::string> &);
void RemovePunct(std::list<std::string> &);
void ToLowerCase(std::list<std::string> &);

