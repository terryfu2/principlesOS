
#pragma once

#include <vector>
#include <string>
#include <utility>

struct ImageInfo {
  std::string path;
  long width, height;
};

struct Results
{
  // path of the largest file in the directory
  std::string largest_file_path;
  // size (in bytes) of the largest file
  long largest_file_size;
  // total number of files in the directory (recursive)
  long n_files;
  // total number of directories in the directory (recursive)
  long n_dirs;
  // cumulative size (in bytes) of all files
  long all_files_size;
  // most common words found in .txt files
  // word = sequence of 5 or more alphabetic characters, converted to lower case
  // sorted by frequency, reported with their counts
  std::vector<std::pair<std::string, int>> most_common_words;
  // largest (in pixels) images found in the directory,
  // sorted by their size (in pixels), reported with their width and height
  std::vector<ImageInfo> largest_images;
  // list of vacant directories
  // vacant directory is one that contains no files anywhere, including in subdirectories (resursive)
  // if a directory is reported vacant, none of its subdirectories should be reported here
  std::vector<std::string> vacant_dirs;
};

Results analyzeDir(int n);
