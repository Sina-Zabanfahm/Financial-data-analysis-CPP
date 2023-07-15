# Financial-data-analysis-CPP

General outline of the program:

The TickData data structure, captures the properties of each row of the data set.

The Metric data structure, captures the final analysis of each stock.

Class Reader, receives a filePath and a delimiter to separate each line of the data set. Then it constructs a map,
with  its keys being names of the assets and with values that are vectors of TickData corresponding to each occurrence of that asset.

While reading the dataset, this class ignores each line with unacceptable code or the rows where bid price is bigger or equal than 
the ask price. I included the equality condition, as the mean of bid - ask price would seem skewed towards zero, otherwise.


Class Analyze computes the required metrics for each asset. 
While computing between trades metrics, this class consider orders with the same time stamp as part of the same order.

The Data structure Probabilities, is designed for studying the round number effect. Here is a general outline:
For trade prices, it counts the occurrences of all trade prices that are non - zero, this will be stored in 
 prob.tradePriceC. Then, it counts all the occurrences  of trade prices that are non - zero and end with a zero digit. This will
be stored in prob.tradePriceZ. Now the ratio (prob.tradePriceC/ prob.tradePriceZ) will be used to find the p - value, under the 
 assumption  that each digit is likely to appear with equal probability as the final digit. 


Column orders for the final csv file is as follows:

1) name of the security.
2) mean time elapsed between trades.
3) median time elapsed between trades.
4)longest time between trades.

5) mean time elapsed between tick changes
6) median time elapsed between tick changes
7) longest time between tick changes.

8) mean of bid - ask spread
9) median of bid - ask spread


Last columns shows the round number effects for:

10, 11, 12, 13, 14, 15) bid prices, ask prices, trade prices, bid volumes, ask volumes and trade volumes respectively.



