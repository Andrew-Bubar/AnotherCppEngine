#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//Defines
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

//Logging
enum TextColor{ //getting settings for all the colors
  TEXT_COLOR_BLACK,
  TEXT_COLOR_RED,
  TEXT_COLOR_GREEN,
  TEXT_COLOR_YELLOW,
  TEXT_COLOR_BLUE,
  TEXT_COLOR_MAGENTA,
  TEXT_COLOR_CYAN,
  TEXT_COLOR_WHITE,
  TEXT_COLOR_BRIGHT_BLACK,
  TEXT_COLOR_BRIGHT_RED,
  TEXT_COLOR_BRIGHT_GREEN,
  TEXT_COLOR_BRIGHT_YELLOW,
  TEXT_COLOR_BRIGHT_BLUE,
  TEXT_COLOR_BRIGHT_MAGENTA,
  TEXT_COLOR_BRIGHT_CYAN,
  TEXT_COLOR_BRIGHT_WHITE,
  TEXT_COLOR_COUNT
};

template <typename ...Args>
void _log(char* prefix, char* msg, TextColor textcolor, Args... args){
    static char* TextColorTable[TEXT_COLOR_COUNT] = 
    {    //alligning the codec for colors with the enum above for easy use!
        "\x1b[30m", // TEXT_COLOR_BLACK
        "\x1b[31m", // TEXT_COLOR_RED
        "\x1b[32m", // TEXT_COLOR_GREEN
        "\x1b[33m", // TEXT_COLOR_YELLOW
        "\x1b[34m", // TEXT_COLOR_BLUE
        "\x1b[35m", // TEXT_COLOR_MAGENTA
        "\x1b[36m", // TEXT_COLOR_CYAN
        "\x1b[37m", // TEXT_COLOR_WHITE
        "\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
        "\x1b[91m", // TEXT_COLOR_BRIGHT_RED
        "\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
        "\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
        "\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
        "\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
        "\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
        "\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
    };

    //preparing the char array for output incuding resetting the color!
    char formatBuffer[8192] = {};
    sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textcolor], prefix, msg);

    char textBuffer[8192] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

//the different options for loggging
#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...) _log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);

#define SM_ASSERT(x, msg, ...) { if(!(x)){ SM_ERROR(msg, ##__VA_ARGS__); DEBUG_BREAK(); SM_ERROR("Assertion Hit!");}}

//Bump Allocator for allocating RAM properly
struct BumpAllocator { //the skeleton to allow for memory + details
  size_t capacity;
  size_t used;
  char* memory;
};

BumpAllocator make_bump_allocator(size_t size){ //creating the BC

  BumpAllocator ba = {};

  ba.memory = (char*)malloc(size); //setting the current memory to the size of the data
  if(ba.memory){ ba.capacity = size; memset(ba.memory, 0, size); } //using memset to determin the size
  else { SM_ASSERT(false, "Failed to allocate memory"); }
  return ba;
}

char* bump_alloc(BumpAllocator* bumpAllocator, size_t size){ //actually allocating the memory!

  char* result = nullptr;

  size_t allignedSize = (size + 7) & ~ 7; //make the first 4 bits are 0

  if(bumpAllocator->used + allignedSize <= bumpAllocator->capacity){ //making sure that all the data is under the capacity of the segment of memory

    result = bumpAllocator->memory + bumpAllocator->used;
    bumpAllocator->used += allignedSize; //move the alligned size to the used data of the BA
  } else { SM_ASSERT(false, "BumpAllocator is full!"); } //crash if memory is full

  return result;
}

//File I/O
// TODO: look into possible json files
long long get_timestamp(const char* file) //when was files opened / created
{
  struct stat file_stat = {};
  stat(file, &file_stat);
  return file_stat.st_mtime;
}

bool file_exists(const char* filePath) //making sure the file is real
{
  SM_ASSERT(filePath, "No filePath supplied!"); //if filePath = false, then crash and send error code

  auto file = fopen(filePath, "rb"); //open the file
  if(!file)
  {
    return false; //running a check
  }
  fclose(file); //close the file

  return true;
}

long get_file_size(const char* filePath) //how big is the file?
{
  SM_ASSERT(filePath, "No filePath supplied!"); //is there a filepath?

  long fileSize = 0;
  auto file = fopen(filePath, "rb");
  if(!file)
  {
    SM_ERROR("Failed opening File: %s", filePath);
    return 0;
  }

  fseek(file, 0, SEEK_END);//see the full file size
  fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);//reset the file
  fclose(file); //close the file

  return fileSize; //return the size
}

/*
* Reads a file into a supplied buffer. We manage our own
* memory and therefore want more control over where it 
* is allocated
*/
char* read_file(const char* filePath, int* fileSize, char* buffer) //simple read file script 
{
  SM_ASSERT(filePath, "No filePath supplied!");
  SM_ASSERT(fileSize, "No fileSize supplied!");
  SM_ASSERT(buffer, "No buffer supplied!");

  *fileSize = 0;
  auto file = fopen(filePath, "rb"); //check the file
  if(!file)
  {
    SM_ERROR("Failed opening File: %s", filePath);
    return nullptr;
  }

  fseek(file, 0, SEEK_END);
  *fileSize = ftell(file); //get the size
  fseek(file, 0, SEEK_SET);

  memset(buffer, 0, *fileSize + 1);
  fread(buffer, sizeof(char), *fileSize, file); //read the file in full

  fclose(file); //close the file

  return buffer; //return the data
}

char* read_file(const char* filePath, int* fileSize, BumpAllocator* bumpAllocator) //a even simpler read file with the BA
{
  char* file = nullptr;
  long fileSize2 = get_file_size(filePath);

  if(fileSize2)
  {
    char* buffer = bump_alloc(bumpAllocator, fileSize2 + 1); //look how simple that is!!

    file = read_file(filePath, fileSize, buffer);
  }

  return file; 
}

void write_file(const char* filePath, char* buffer, int size) //writing to a file
{
  SM_ASSERT(filePath, "No filePath supplied!");
  SM_ASSERT(buffer, "No buffer supplied!");
  auto file = fopen(filePath, "wb"); //opening the file
  if(!file)
  {
    SM_ERROR("Failed opening File: %s", filePath);
    return;
  }

  fwrite(buffer, sizeof(char), size, file); //write the buffer and size to the file
  fclose(file);
}

bool copy_file(const char* fileName, const char* outputName, char* buffer) //simply copying the file
{
  int fileSize = 0;
  char* data = read_file(fileName, &fileSize, buffer); //get the data to copy

  auto outputFile = fopen(outputName, "wb"); //get the file to paste the data
  if(!outputFile)
  {
    SM_ERROR("Failed opening File: %s", outputName);
    return false;
  }

  int result = fwrite(data, sizeof(char), fileSize, outputFile); //pasting the data
  if(!result)
  {
    SM_ERROR("Failed opening File: %s", outputName);
    return false;
  }
  
  fclose(outputFile); //close the paste file

  return true; //it worked!!!
}

bool copy_file(const char* fileName, const char* outputName, BumpAllocator* bumpAllocator) //same as above but even easier with the BA
{
  char* file = 0;
  long fileSize2 = get_file_size(fileName);

  if(fileSize2)
  {
    char* buffer = bump_alloc(bumpAllocator, fileSize2 + 1);

    return copy_file(fileName, outputName, buffer);
  }

  return false;
}