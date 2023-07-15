#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <map>
#include<algorithm>
#include <cmath>


/*
 * Data type TickData, captures all the important information from the ticks.
 * The "==" operator is overridden so we can change the program easier in the future,
 * bidPriceLD,... tradePriceLD captures the last digit of the number. It is unnecessary to define a variable for it,
 * but it simplifies the code later on and make it more readable.
 */
struct TickData {
	int index;
	double bidPrice=0.0;
    double askPrice=0.0;
    double tradePrice=0.0;
    double bidVolume=0.0;
    int askVolume=0.0;
    int tradeVolume=0.0;
    std::string updateType;
    double time=0.0;

    char bidPriceLD;
    char askPriceLD;
    char tradePriceLD;
    char bidVolumeLD;
    char askVolumeLD;
    char tradeVolumeLD;
};
std::ostream& operator<<(std::ostream& os, const TickData& tick) {
    os <<" "<< tick.index << " "<<tick.bidPrice<<" "<<" "<<tick.askPrice;
    os <<" "<<tick.tradePrice<<" "<<tick.bidVolume<<" ";
    os <<" "<<tick.askVolume<<" "<<tick.tradeVolume<<" ";
    os <<" "<<tick.updateType<<" "<<tick.time;
    return os;
}
bool operator==(const TickData& tick1, const TickData& tick2) {
	/*if (tick1.bidPrice != tick2.bidPrice) return false;
	if (tick1.askPrice !=tick2.askPrice) return false;*/
	if (tick1.tradePrice != tick2.tradePrice) return false;
	return true;
}
bool operator!=(const TickData& tick1, const TickData& tick2) {
	return !(tick1==tick2);
}

/* Metrics Data captures the desired analysis for each stock.
 * meanBT, mean TC, meanSP are mean time elapsed between trades, between tick changes,
 * and mean of bid-ask spread. Note that these are designed for stock by stock analysis.
 * Same holds for medians.
 * longest BT, longest TC, is the longest time elapsed between trades, tick changes respectively.
 */
struct Metrics{

	double meanBT=0.0;
	double medianBT=0.0;
	double meanTC=0.0;
	double medianTC=0.0;
	double longestBT=0.0;
	double longestTC=0.0;
	double meanSP=0.0;
	double medianSP=0.0;

};


std::ostream& operator<<(std::ostream& os, const Metrics& metric){

	os<<"Mean time between trades is: "<<metric.meanBT<<std::endl;
	os<<"Median time between trades is: "<<metric.medianBT<<std::endl;
	os<<"Longest time between trade changes is: "<<metric.longestBT<<std::endl;
	os<<"Mean time between tick changes is: "<<metric.meanTC<<std::endl;
	os<<"Median time between tick changes is: "<<metric.medianBT<<std::endl;
	os<<"Longest time between tick changes is: "<<metric.longestTC<<std::endl;
	os<<"Mean value for bid - ask spread is: "<<metric.meanSP<<std::endl;
	os<<"Median value for bid - ask spread is: "<<metric.medianSP<<std::endl;
	return os;

}


/* Probabilities data is for capturing the round number effect.
 *  bidPriceC,...,tradeVolumeC counts the total occurrences of metrics that are non-zero
 *  It is reasonable to avoid zero values as there seem to be a lot of occurrences of zero.
 *  (A bid price of 0.0 provides valuable information, but it is certainly different from the price 101.0
 *
 *  bidPriceZ,..., tradeVolumeZ counts the occurrences of nonzero metrics that ends in a round number.
 *
 *  pValBidPrice,...,pValtradeVolume stores the p-value for seeing respective ratios, under the assumption that each
 *  digit is equally likely to appear as the last digit.
 */
struct Probabilities{
public:

	long int bidPriceC = 0, bidPriceZ =0;
	long int askPriceC = 0, askPriceZ =0;
	long int tradePriceC = 0, tradePriceZ=0;
	long int bidVolumeC = 0, bidVolumeZ = 0;
	long int askVolumeC = 0, askVolumeZ = 0;
	long int tradeVolumeC = 0, tradeVolumeZ=0;

	long double pValBidPrice=0.0;
	long double pValAskPrice=0.0;
	long double pValTradePrice=0.0;
	long double pValBidVolume = 0.0;
	long double pValAskVolume = 0.0;
	long double pValTradeVolume=0.0;

};


/*
 * class Reader is initialized by receiving the pathFile, and the delimiter to separate the file.
 * makeMap read the file line by line, skip the lines with the wrong condition code and
 * those with bid price larger than ask price. However, it allows equality between bid and ask price. Then,
 * it converts its rows to TickData by calling getTick() function. Finally stores the data in a map,
 * where its keys are stock identifiers, for each identifier it produces a vector of ticks.
 */
class Reader{
public:

	Reader(std::string filePath, char delimiter):
		filePath(filePath),delimiter(delimiter){
		makeMap();
	}

	std::map<std::string,std::vector<TickData>> getTickMap(){
		//std::cout<<tickMap.size()<<std::endl;
		return tickMap;
	}

private:

	std::string filePath;
	char delimiter;
	std::map<std::string,std::vector<TickData>> tickMap;


	void makeMap(){
		std::ifstream file(filePath);
		std::string line;
		int index=0;
		while(std::getline(file, line)){

			std::vector<std::string> row = parseLine(line);
			if(std::stod(row[2])<std::stod(row[3]) && (row[14]=="" || row[14]=="XT")){
				TickData tick = getTick(row);
				tickMap[row[0]].push_back(tick);
			}
		}
		return;

	}


	TickData getTick(std::vector<std::string> row){
		//Captures tick information.
		TickData tick;
		tick.index = std::stoi(row[1]);
		tick.bidPrice = std::stold(row[2]);
		tick.askPrice = std::stold(row[3]);
		tick.tradePrice = std::stold(row[4]);
		tick.bidVolume = std::stoi(row[5]);
		tick.askVolume = std::stoi(row[6]);
		tick.tradeVolume=std::stoi(row[7]);
		tick.updateType = row[8];
		tick.time = std::stoi(row[11]);

		tick.bidPriceLD = row[2].back();
		tick.askPriceLD = row[3].back();
		tick.tradePriceLD = row[4].back();
		tick.bidVolumeLD = row[5].back();
		tick.askVolumeLD = row[6].back();
		tick.tradeVolumeLD =row[7].back();
		//::cout<<tick<<std::endl;
		return tick;
	}


	std::vector<std::string> parseLine( const std::string& line){
		std:: stringstream ss(line);
		std::string token;
		std::vector<std::string> row;
		while(std::getline(ss,token, delimiter)){
			row.push_back(token);
		}
		return row;
	}

};
/* class Analytics, receives a vector of ticks.
 * tradeChange() computes between the trade metrics.
 * tickerChange() computes the metrics for tick changes.
 * spreadChange() compute the metrics for mean and median of bid - ask spread.
 * zeroCount() computes the metrics for the Probability data.
 * it calls compute pVal functions for computing corresponding p-values, under the assumption that each digit is equally likely
 * to appear as the last digit.
 */
class Analytics{

public:
	Analytics(std::vector<TickData> tickVec):
	tickVec(tickVec){
		tradeChange();
		tickerChange();
		spreadChange();
		zeroCount();
	}

	Metrics getMetric(){
		return metric;
	}

	Probabilities getPvals(){
		return prob;
	}

private:

	std::vector<TickData> tickVec;
	Metrics metric;
	Probabilities prob;

	void tradeChange(){
		long double sum =0.0;
		double last = 0.0;
		std::vector<double> ts;
		int index = 0;
		while(tickVec[index].updateType!="1" && index<tickVec.size()){
			index++;
		}

		if(index < tickVec.size()){
			last = tickVec[index].time;
		}

		for(int i =index+1; i<tickVec.size();i++){
			if(tickVec[i].updateType == "1" && tickVec[i].time!=last){
				sum+=tickVec[i].time - last;
				ts.push_back(tickVec[i].time - last);
				last = tickVec[i].time;
			}
		}

		std::sort(ts.begin(),ts.end());
		if(ts.size()==0){
			return;
		}

		if(ts.size()>=1){
			metric.meanBT= double(sum/ts.size());
		}

		if (ts.size()%2 == 0 && ts.size()>=2){
			metric.medianBT=(long double)((ts[ts.size()/2]+ts[ts.size()/2-1])/2);
		}

		else metric.medianBT = ts[ts.size()/2];
		metric.longestBT = ts.back();

		return;
	}

	void tickerChange(){
		long double sum =0.0;
		double last=0.0;
		std::vector<double> ts;
		last = tickVec[0].time;

		for(int i = 1; i<tickVec.size(); i++){
			if(tickVec[i]!=tickVec[i-1]){
				sum+=tickVec[i].time - last;
				ts.push_back(tickVec[i].time - last);
				last = tickVec[i].time;

			}

		}

		if(ts.size()==0){
			return;
		}

		std::sort(ts.begin(),ts.end());
		if(ts.size()>=1){
			metric.meanTC=double(sum/ts.size());
		}

		if(ts.size()%2 == 0 && ts.size()>=2){
			metric.medianTC=(double)((ts[ts.size()/2]+ts[ts.size()/2-1])/2);
		}

		else metric.medianTC = ts[ts.size()/2];
		metric.longestTC = ts.back();
		return;
	}

	void spreadChange(){
		long double sum = 0.0;
		std::vector<double> ts;
		TickData tick = tickVec[0];

		for(int i = 0; i<tickVec.size(); i++){
			if(tickVec[i].askPrice-tickVec[i].bidPrice == 0){
				std::cout<<tickVec[i].index<<std::endl;
			}
			sum+=tickVec[i].askPrice - tickVec[i].bidPrice;
			ts.push_back(tickVec[i].askPrice - tickVec[i].bidPrice);
		}
		metric.meanSP = (double)(sum/ts.size());
		std::sort(ts.begin(),ts.end());
		if(ts.size()%2 == 0 && ts.size()>=2){
			metric.medianSP=(double)((ts[ts.size()/2]+ts[ts.size()/2-1])/2);
		}
		else metric.medianSP = (double)(ts[ts.size()/2]);
		return;
	}

	void zeroCount(){

		for(int i = 0; i < tickVec.size(); i++){

			prob.bidPriceC += increment(tickVec[i].bidPrice);
			prob.bidPriceZ += increment(tickVec[i].bidPrice, tickVec[i].bidPriceLD);

			prob.askPriceC += increment(tickVec[i].askPrice);
			prob.askPriceZ += increment(tickVec[i].askPrice, tickVec[i].askPriceLD);

			prob.tradePriceC += increment(tickVec[i].tradePrice);
			prob.tradePriceZ += increment(tickVec[i].tradePrice, tickVec[i].tradePriceLD);

			prob.bidVolumeC += increment(tickVec[i].bidVolume);
			prob.bidVolumeZ += increment(tickVec[i].bidVolume, tickVec[i].bidVolumeLD);

			prob.askVolumeC += increment(tickVec[i].askVolume);
			prob.askVolumeZ += increment(tickVec[i].askVolume, tickVec[i].askVolumeLD);

			prob.tradeVolumeC += increment(tickVec[i].tradeVolume);
			prob.tradeVolumeZ += increment(tickVec[i].tradeVolume, tickVec[i].tradeVolumeLD);

		}
			prob.pValBidPrice = computePval(prob.bidPriceC, prob.bidPriceZ);
			prob.pValAskPrice = computePval(prob.askPriceC, prob.askPriceZ);
			prob.pValTradePrice = computePval(prob.tradePriceC, prob.tradePriceZ);
			prob.pValBidVolume = computePval(prob.bidVolumeC, prob.bidVolumeZ);
			prob.pValAskVolume = computePval(prob.askVolumeC, prob.askVolumeZ);
			prob.pValTradeVolume = computePval(prob.tradeVolumeC, prob.tradeVolumeZ);
			//std::cout<<prob.tradePriceC<<" "<<prob.tradePriceZ<<std::endl;


		return;
	}

	double computePval(int n, int x){

		double p = 0.1;
		double mu = 0.1*n;
		double sigma = sqrt(n * p * (1-p));
		double z = (x-mu)/sigma;
		double prob = 0.5 * (1.0 + std::erf(z / std::sqrt(2.0)));
		if (prob<=0.5) return prob;
		return 1-prob;
	}
	int increment(int x){
		return x!=0;
	}
	int increment(int x, char c){
		return x!=0 && c=='0';
	}

};

int main() {

	//std::cout<<computePval(1000000,99999)<<std::endl;
    std::ofstream file("/Users/sinazabanfahm/eclipse-workspace/First/opt.csv");
    file << std::fixed << std::setprecision(7);
    //std::cout << std::fixed << std::setprecision(2);

	std::string filePath = "/Users/sinazabanfahm/eclipse-workspace/First/data/scandi.csv";
		//std::vector <std::vector<std:: string >> data = td.getData();
	Reader reader(filePath,',');
	std::map<std::string,std::vector<TickData>> tickMap = reader.getTickMap();
	//std::cout<<tickMap.size()<<std::endl;
	for(auto it = tickMap.begin(); it !=tickMap.end(); it++){
		Analytics analytic(tickMap[it->first]);
		Metrics metric = analytic.getMetric();
		Probabilities prob = analytic.getPvals();
		std::vector<long double> output = {metric.meanBT, metric.medianBT, metric.longestBT,
				metric.meanTC, metric.medianTC, metric.longestTC, metric.meanSP, metric.medianSP,
				prob.pValBidPrice, prob.pValAskPrice, prob.pValTradePrice, prob.pValBidVolume,
				prob.pValAskVolume, prob.pValTradeVolume};
		if (file.is_open()){
				file<<it->first<<",";
				for(int i =0 ; i<output.size();i++){
					file<<output[i]<<",";
				}
				file<<"\n";
		}
	}
	file.close();

	return 1;

}
