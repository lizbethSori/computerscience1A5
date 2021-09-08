//COP 3502C Programming Assignment 5//
//This program is written by: Lizbeth Ramirez Soriano// 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define MAX 26

//struct of tries
typedef struct tries {
  int times;
  int size;
  struct tries* next[MAX];
} tries;

//set everything to zero and using malloc for struct
tries* newtries() {

  tries* temp = malloc(sizeof(tries));
  temp->times = 0;
  temp->size = 0;

  //since there is 26 we have to make sure that each of them are set to NULL;
  for (int i=0; i<MAX; i++)
  {
    temp->next[i] = NULL;
  }

  return temp;
}

// Inserts to the tries if it a 1 command
void insert(tries* root, char* word, int k, int stringlenght, int times)
{
  int sizes = word[k] - 'a';
  //we would insert the count of copies here 
  root->times += times;

  // base case
  if (k == stringlenght)
  {
    return;
  }

  // if there is no node then we would create one
  if (root->next[sizes] == NULL)
  {
    root->next[sizes] = newtries();
  }

  // number of copies plus the struct times
  int newsize = root->next[sizes]->times + times;
  int oldsize = root->size;
  
  //if the new size is bigger then the old size we would make the old size the new size
  if (newsize > oldsize){
    root->size = newsize;
  }

  insert(root->next[sizes], word, k+1, stringlenght, times);
}


//if the comand is 2 we would put it here
void predict(tries* root, char* word, int k, int stringcount, FILE* outfile) {

  int size = word[k] - 'a';
  // Got to the end.
  if (k == stringcount) {

    // is the size is 0 then we would put unknown
    if (root->size == 0) {
      printf("unknown word\n");
      fprintf(outfile,"unknown word\n");
      return;
    }

    // print all the max
    for (int i=0; i<MAX; i++) {
      //if the next node is NULL we would make it continue the program
      if (root->next[i] == NULL)
      {
        continue;
      }

      //print out the predicted character
      if (root->next[i]->times == root->size)
      {
        char word = 'a' + i;
        printf("%c", word);
        fprintf(outfile,"%c", word);
      } 
    }
    printf("\n");
    fprintf(outfile,"\n");
    return;
  }

  // if next is NULL then we would print unknown
  if (root->next[size] == NULL) {
    printf("unknown word\n");
    fprintf(outfile, "unknown word\n");
    return;
  }

  predict(root->next[size], word, k+1, stringcount, outfile);
}

//freeing the tries
void Free(tries* root) {

  for (int i=0; i<MAX; i++) {

    if (root->next[i] != NULL)
    { 
      Free(root->next[i]);//free the subtree
    }
  }

  //free the root
  free(root);
}

int main(void) {
  atexit(report_mem_leak);
  //make the root
  tries* root = newtries();

  int wordnumbers;
  int time;
  int command;
  char* string;

  FILE* infile;
  FILE* outfile;
  infile = fopen("in.txt", "r");
  outfile = fopen("out.txt", "w");

  //how many words
  fscanf(infile,"%d", &wordnumbers);

  //for loop with the amount of words
  for (int i=0; i<wordnumbers; i++) {

    string = malloc(sizeof(char)*4000000);

    //since the infile all has 2 words we can scan the two words normally
    fscanf(infile,"%d %s", &command, string);

    int count = strlen(string);

    //if it is one
    if (command == 1) {
      fscanf(infile,"%d", &time);
      insert(root, string, 0, count, time);
    }
    //if they put 2
    else if (command == 2){
      predict(root, string ,0, count, outfile);
    }
    //if they put something other then 2
    else {
      printf("not a real command");
      fprintf(outfile,"not a real command");
    }

    //free the string since we use malloc
    free(string);
  }
  // Free the tries
  Free(root);
}
