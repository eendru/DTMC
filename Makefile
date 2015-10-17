.PHONY: all clean 

SRC = model.h model.cpp serialize.pb.cc serialize.pb.h


all:
	clear
	g++ -std=c++11 train.cpp $(SRC) -l protobuf -o markov_chain_train
	g++ -std=c++11 predict.cpp $(SRC) -l protobuf -o markov_chain_predict

train:
	clear
	g++ -std=c++11 train.cpp $(SRC) -l protobuf -o markov_chain_train

predict:
	clear
	g++ -std=c++11 predict.cpp $(SRC) -l protobuf -o markov_chain_predict

clean:
	rm markov_chain_predict
	rm markov_chain_train
