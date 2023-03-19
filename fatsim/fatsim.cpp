
//function to find largest path in a graph - or file structure

#include "fatsim.h"
#include <cstdio>
#include <stack>
#include <iostream>
#include <map>
#include <algorithm>

std::vector <long> fat_check(const std::vector <long> & fat) {

  //ds to keep track if a value has been visited, the length to get to it, and max length
  //vectors for larger inputs (ie 10mill run slower, hence the maps)
  std::vector <bool> visited = std::vector < bool > (fat.size());
  std::vector <long> length = std::vector < long > (fat.size());
  std::map <long,long> maxLength;

  //needed to keep track of nodes that are in a cycle, in order to speed up test10
  std::vector<bool> cycle = std::vector<bool>(fat.size());

  //see if fat has anything pointing to -1, if not, then break and return empty
  bool start = false;
  for(long int index = 0;index<long(fat.size());index++){
    if(fat[index] == -1){
      start = true;
      break;
    }
  }
  if(!start){
    length.clear();
    return length;
  }
  //loop through every in fat
  for (long int index = 0; index < long(fat.size()); index++) {

    //skip if done
    if (visited[index]) {
      continue;
    }

    //variables to keep track
    std::vector <long> currSearch;
    std::map <long,bool> currVisited;
    long curr = index;
    long finish = -1;
    long next = 0;
    long pathCount = 0;
    bool isCycle = false;
    

    //loop until path ends, or already visited
    while (true) {
      //mark as visited
      currSearch.push_back(curr);
      currVisited[curr] = true;
      next = fat[curr];

      //points to a null file
      if (next == -1) {

        pathCount = 0;
        finish = curr;
        break;
      } 
      //points to an already visited node within this loop -  a cycle, or points to a node that has been determined
      //is in a cycle
      else if (currVisited[next] || cycle[next]) {

        isCycle = true;
        break;
      } 
      //points to a node that already has a max length found
      else if (visited[next]) {

        pathCount = length[next];
        break;
      }

      curr = next;
    }
    //go to next node when cycle is found
    if (isCycle) {
      cycle[index] = true;
      continue;
    }

    //backtrack current node to find length of path
    for (long t = currSearch.size() - 1; t >= 0; t--) {

      long index = currSearch[t];
      visited[index] = true;
      pathCount++;
      length[index] = pathCount;
    }
    //is path is greater than stored, switch
    if (length[index] > maxLength[finish]) {

      maxLength[index] = length[index];
    }

  }

  //prepare maxLength for output specifications
  std::vector <long> output;
  long max = 0;
  for (auto index = maxLength.begin(); index != maxLength.end(); index++) {

    if (index -> second > max) {

      output.clear();
      output.push_back(index -> first);
      max = index -> second;
    } else if (index -> second == max) {

      output.push_back(index -> first);
    }
  }

  std::sort(output.begin(), output.end());

  return output;

}

//was my first attempt, it was less than O(n^2) but closer to n^2 than n and still not effiecent enough for larger values of n
/*std::vector<long> distance(fat.size());
    std::vector<long> used;
    
    auto counter = 0;
    for(auto & a: fat){

        if(a == -1){
            distance[counter] = 1;
            used.push_back(counter);
        }
        else{
            distance[counter] = 0;
        }
        counter++;
    }

    std::vector<long> output;
    long max = 0;

    auto c = 0;

    while(used.size() < distance.size()){
        auto count = 0;
        bool changed = false;
        for(auto & a: distance){
            if(a != 0){
                count++;
                continue;
            }

            if(std::count(used.begin(), used.end(), fat[count])){
                distance[count] += distance[fat[count]] + 1;
                used.push_back(count);
                changed = true;
            }

            if(a > max){
                output.clear();
                output.push_back(count);
                max = a;
            }
            else if(a == max){
                output.push_back(count);

            }
            count++;
        }
        if(!changed){
            break;
        }

    }

    
    return output; */