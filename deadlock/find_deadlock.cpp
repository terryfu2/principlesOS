// this is the ONLY file you should edit and submit to D2L

#include "find_deadlock.h"
#include "common.h"
#include <iostream>  
#include <string> 

std::unordered_map<int, std::string> dict;

//given in hints
class FastGraph {
public:
    std::vector<std::vector<int>> adj_list;
    std::vector<int> out_counts;

} graph;

//reused from common.cpp
class Word2Int2 {

public:
    int counter = 0;
    std::unordered_map<std::string, int> myset;

    int get(const std::string& w) {
        auto f = myset.find(w);
        if (f == myset.end()) {
            dict[counter] = w;
            myset[w] = counter;
            counter++;
            return counter - 1;
        } 
        return f->second;
    }
    

} dictionary;

//modelled after psuedocode given in assignment file, both algo and topsort
Result find_deadlock(const std::vector<std::string> & edges)
{
    // let's try to guess the results :)
     Result result;
     result.index = -1;
     

     for (int t = 0; t < int(edges.size()); t++) {
        std::string edge = edges[t];
       
        std::vector<std::string> tokens = split(edge);
        
        //std::string t1 = (tokens[0]).c_str();
        
       // std::string t2 = tokens[2];

        //int t1 = atoi(tokens[0].c_str());
        std::string sign = tokens[1];
        //int t2 = atoi(tokens[2].c_str());
       
        int t1 = dictionary.get("[P]" + tokens[0]); 
        int t2 = dictionary.get("[R]" + tokens[2]);

        /*printf("%i",t1);
        printf("%s",sign.c_str());
        printf("%i",t2);
        printf("\n");*/
        int left,right = 0;

        if (sign == "->") {
            
            left = t1;
            right = t2;
        } else if (sign == "<-") {
           
            left = t2;
            right = t1;
        } 

        //set size of vetors to avoid seg fault
        size_t size = dictionary.counter;
        graph.adj_list.resize(size);
        graph.out_counts.resize(size);
        
        graph.adj_list[right].push_back(left);
        graph.out_counts[left]++;

        std::vector<int> outsDegrees = graph.out_counts;
        std::vector<int> zerosOutDegrees;
        for (int i = 0; i < int(graph.out_counts.size()); i++) {
            if (graph.out_counts[i] == 0) {
                zerosOutDegrees.push_back(i);
            }
        }

        while (!zerosOutDegrees.empty()) {
            int curr = zerosOutDegrees.back();
            zerosOutDegrees.pop_back();
            for (int adj : graph.adj_list[curr]) {
                outsDegrees[adj]--;

                if( outsDegrees[adj] != 0){
                    continue;
                }
                zerosOutDegrees.push_back(adj);
                
            }
        }

        for (int j = 0; j < int(outsDegrees.size()); j++) {
            if (outsDegrees[j] != 0) {
                std::string curr = dict[j];
                //if not empty
                if (int(curr.find("[P]")) == -1) {
                    continue;
                }
                
                curr.replace(0, 3, "");
                result.procs.push_back(curr);
                
            }
        }
        
        if (!result.procs.empty()) {
            result.index = t;
            return result;
        }

     }

     return result;

}
 


