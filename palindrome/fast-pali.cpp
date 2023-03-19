

//Longest palidrome optimized for 2 GB input

//Some interesting notes i had.
//  Using word.push_back(letter) gave a significatly shorter time as opposed to doing word += letter
// checking isPalindrome after checking size of both also gave significatly shorter time as opposed to checking before

#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>

//global variables for reading the input
char buffer[1024*1024*10]; 
int buff_size = 0;      
int buff_pos = 0;      


//function to read a character from stdin, 
//taken from given fast-int.cpp
int
fast_read_one_character_from_stdin()
{

  if( buff_pos >= buff_size) {
    
    buff_size = read( STDIN_FILENO, buffer, sizeof(buffer));
    
    if(buff_size <= 0) return -1;
    
    buff_pos = 0;
  }
  
  return buffer[buff_pos++];
}


//function to take each character read from stdin function, and add them all together, as long as they
//are from the same line
//taken and modified from fast-int.cpp
std::string
stdin_read_line()
{
  std::string result;
  while( true) {
    int c = fast_read_one_character_from_stdin();
    if( c == -1){
        break;
    }
    
    result.push_back(c);
    
  }
  
  return result;
}

//function that checks if given string is a palindrome, and returns true/false accordingly
//taken from slow-pali.cpp
bool is_palindrome(const std::string &s)
{
    for (size_t i = 0; i < s.size() / 2; i++)
        if (tolower(s[i]) != tolower(s[s.size() - i - 1]))
            return false;
    return true;
}

//function that goes through each line, and runs through every character
//when there is a "blank space" we know that the characters between are a word, and we check
//if said word is a palindrome
//we then compare the plaindrome to the previous max, and swap if nessecary
//returns the max size plaindrome
std::string
get_longest_palindrome(){
    std::string max_pali = "";
    while(1){
        std::string line = stdin_read_line();
        std::string word = "";
        if(line.size() == 0){
            break;
        }
        //printf("%s\n",line.c_str());
        //run through every character in the line
        for(auto letter:line){
            
            //check if there is a blank sapce
            
            if(isspace(letter)){
                //printf("%s\n",word.c_str());
                //check if size of max plaidrome stored is smaller than currernt word
                if(max_pali.size() <=word.size()){
                    if(is_palindrome(word)){
                        //printf("%s\n",word.c_str());
                        max_pali = word;
                    }
                }
                word = "";
            }
            else{
                //add letter to word
            word.push_back(letter);
            }
           
        }
        if(max_pali.size() <=word.size()){
            if(is_palindrome(word)){
                //printf("%s\n",word.c_str());
                max_pali = word;
            }
        }
    }
    return max_pali;
}


//main function to run and print statement
//taken from slow-pali
int
main()
{
  std::string max_int = get_longest_palindrome();
  printf("Longest palindrome: %s\n", max_int.c_str());
  return 0;
}