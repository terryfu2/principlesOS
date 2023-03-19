
#include "analyzeDir.h"

#include <cassert>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <sys/resource.h>
#include <sys/time.h>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <cstring>

//global variables
Results res;
Results all;
std::unordered_map<std::string,int> hist;
std::set<std::pair<int,std::string>> mmq;
int count;
int numI;
//four functions below were given either through stater code, or gitlab
/// check if path refers to a directory
static bool is_dir(const std::string & path)
{
  struct stat buff;
  if (0 != stat(path.c_str(), &buff)) return false;
  return S_ISDIR(buff.st_mode);
}

/// check if path refers to a file
static bool is_file(const std::string & path)
{
  struct stat buff;
  if (0 != stat(path.c_str(), &buff)) return false;
  return S_ISREG(buff.st_mode);
}

/// check if string ends with another string
static bool ends_with(const std::string & str, const std::string & suffix)
{
  if (str.size() < suffix.size()) return false;
  else
    return 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

static bool starts_with(const std::string & str, const std::string & suffix)
{
  if (str.size() < suffix.size()) return false;
  else
    return 0 == str.compare(0, suffix.size(), suffix);
}

//main function, structure was given from starter code
Results analyzeDir(int n)
{ 
  //initialize values in output structure
  res.largest_file_path = "";
  res.largest_file_size = -1;
  res.n_files = 0;
  res.n_dirs = 1;
  res.all_files_size = 0;
  
  int run = 0;
  //go through every dir and file, given from gitlab
  std::vector<std::string> stack;
  stack.push_back(".");
  while( ! stack.empty()) {
    auto dirname = stack.back();
    stack.pop_back();

    //printf("%s\n", dirname.c_str());

    DIR * dir = opendir( dirname.c_str());
    if( dir) {
      while(1) {
        dirent * de = readdir( dir);
        if( ! de) break;
        std::string name = de-> d_name;
        if( name == "." || name == "..") continue;
        std::string path = dirname + "/" + de-> d_name;
        stack.push_back( path);
        
        //path is a directory
        if (is_dir(path)) {
          
          all.vacant_dirs.push_back(path);
          res.n_dirs++;
          continue;
        }
      // make sure this is a file
        if(not is_file(path)) {
          continue;
        }
      // report size of the file
        res.n_files++;
        for (auto & ii : all.vacant_dirs){
          //printf("%s %s\n",path.c_str(), ii.c_str());
          if(starts_with(path,ii) ){
            ii = "NO";
            
          }
        } 
        //printf("%s\n", path.c_str());
        {
        struct stat buff;
          if (0 != stat(path.c_str(), &buff)) {
            count ++;
          }
            //printf("    - could not determine size of file\n");
          else{
            //printf("    - file size: %ld bytes\n", long(buff.st_size));
            //printf("%s\n", path.c_str());
            if(res.largest_file_size <= long(buff.st_size)){
              if(starts_with(path,"./.")){
                res.largest_file_path = path.substr(3);
              }
              else{
                res.largest_file_path = path.substr(2);

              }
              res.largest_file_size = long(buff.st_size);
            }
            
            res.all_files_size += long(buff.st_size);
          }
        }

        // check if filename ends with .txt
        if (ends_with(name, ".txt")) {
          
          //read file by line, and split words, add to hist
          std::ifstream file(path.c_str());
          std::string line;

          if (!file.is_open()) return res;

          while (std::getline(file, line)) {
              std::string word;
              line.push_back(' ');
              std::transform(line.begin(), line.end(), line.begin(), ::tolower);

              for (char& each : line) {
                  if (std::isalpha(each)) {
                      word.push_back(each);
                  } else {
                      if (word.size() >= 5){
                        hist[word]++;
                      } 
                      word.clear();
                  }
              }
            }
            file.close();
        }

        //get image height and weight from terminal cmd, and format to add to res
        std::string cmd = "identify -format '%w %h' " + path + " 2>&1" ;
        std::string width_size;
        std::string height_size;
        auto fp = popen(cmd.c_str(), "r");
        assert(fp);
        char buff[PATH_MAX];
        if( fgets(buff, PATH_MAX, fp) != NULL) {

          width_size = buff;
        }
        int status = pclose(fp);
        if( status != 0 or width_size[0] == '0'){
          width_size = "";
          continue;

        }
        //printf("%s\n", width_size.c_str());
        int counter = 0;
        std::string word;
        std::string::size_type sz; 
        long h = 0;
        long w = 0;
        for(auto letter:buff){
            //check if there is a blank sapce
          if(isspace(letter)){
              //printf("%s\n",word.c_str());
              if(counter == 0){
                h = std::stol (word.c_str(),&sz);
              }
              else{
                w = std::stol (word.c_str(),&sz);
              }
              word = "";
              counter ++;
          }
          else{
                //add letter to word
            word.push_back(letter);
          }
           if(counter >=2){
            break;
           }
        }
        ImageInfo info1 { path, h,w};
        all.largest_images.push_back(info1);
        numI ++;
        
        run++;
      }
    closedir( dir);
    }
  }

  //get only top n biggest images and add to res
  ImageInfo info2;
  int runs = n;
  if(numI < n){
    runs = numI;
  }
  for( int i = 0; i<runs;i++){
    int max = 0;
    for (auto & ii : all.largest_images){
      if(ii.width * ii.height >= max){
        if(ii.path == ""){
          continue;
        }
        max = ii.width * ii.height;
        info2.path = ii.path.substr(2);
        info2.width = ii.width;
        info2.height = ii.height;
      }
    }
    
    res.largest_images.push_back(info2);

    for (auto & ii : all.largest_images){
      if(ii.width * ii.height == max){
        if(ii.path.substr(2) == info2.path ){
          ii.path = "";
          break;
        }
      }
    }
  }
  
  //add top n count words to res
  for(auto & h : hist) {
    // insert an entry
    mmq.emplace(-h.second, h.first);
    // if we have more than n, get rid of the smallest one
    if(mmq.size() > size_t(n))
      mmq.erase(std::prev(mmq.end()));
  }
  for(auto & a : mmq){
    res.most_common_words.push_back({ a.second.c_str(),  -a.first});
  }
  
  //add all vacant dirs, remove those with same start
  for (auto & ii : all.vacant_dirs){
    for (auto & cc : all.vacant_dirs){
      if(ii == cc){
        continue;
      }
      if(starts_with(ii,cc)){
        ii = "NO";
      }
    }
    if(ii == "NO"){
      continue;
    }
    else{
      res.vacant_dirs.push_back(ii.substr(2));
    }
  }
  
  return res;
}

     



  

