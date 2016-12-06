# Artificial-Intelligence-Search

## Overview
The objective is to reach the goal state or reach as close as possible to the goal state. In our case we have been given a list of companies, each one bidding for regions to control for a certain price. Our goal is to allocate the regions based on the bids to different companies in order to maximise our profit.

## Approach
The bids are modified by adding better bids to our chosen set of bids and removing the conflicting ones. This is done by **Greedy hill climbing implementation of Local Search**. Every time we try to climb the hill there is a small probability ~1% of taking a **Random Restart** where we completely pick a new set of chosen bids to start from. There is also a small probability of taking a **Random Walk** where any bid random bid is added to our chosen set instead of choosing comparitively better bid.

## Steps to Run
*Compile* using `compile.sh`  
*Run* using `run.sh` input.txt output.txt  
+ **input.txt** contains the company bids
+ **output.txt** will contain the best found profit

## Authors
* [Nikhil Gupta](https://github.com/NikhilGupta1997)
* [Dhairya Sandhyana](https://github.com/DhairyaSandhyana)

Course Project under [**Prof. Mausam**](http://homes.cs.washington.edu/~mausam)

