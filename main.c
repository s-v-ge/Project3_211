//  bestWordle.c
//  Author: Zach Savignano
//
//  Links to wordle dictionary words at:
//    https://www.reddit.com/r/wordle/comments/s4tcw8/a_note_on_wordles_word_list/
//

#include <stdio.h>
#include <stdlib.h> // for exit( -1)
#include <string.h>
#include <stdbool.h>

struct wordScore {
  char word[6];
  int score;
};

//-------------------------------------------------------------------------------
// Comparator for use in built-in qsort(..) function.  Parameters are declared
// to be a generic type, so they will match with anything. This is a two-part
// comparison.  First the scores are compared.  If they are the same, then the
// words themselves are also compared, so that the results are in descending
// order by score, and within score they are in alphabetic order.
int compareFunction(const void *a, const void *b) {
  // Before using parameters we have cast them into the actual type they are in
  // our program and then extract the numerical value used in comparison
  int firstScore = ((struct wordScore*)a)->score;
  int secondScore = ((struct wordScore*)b)->score;

  // If scores are different, then that's all we need for our comparison.
  if (firstScore != secondScore) {
    // We reverse the values, so the result is in descending vs. the otherwise
    // ascending order return firstScore - secondScore;   // ascending order
    return secondScore - firstScore; // descending order
  } else {
    // Scores are equal, so check words themselves, to put them in alphabetical
    // order Hint: return the value of strcmp( parameter a word,  parameter b
    // word)
    //return 1;
    return strcmp(((struct wordScore*)a)->word, ((struct wordScore*)b)->word);
  }
} // end compareFunction(..)

// prints contents of array of cstrings
void printArr(char** arr, int size) {
  for (int i = 0; i < size; i++) {
    printf("%s\n", arr[i]);
  }
  printf("\n");
}

//-------------------------------------------------------------------------------
// removeChars() : this funtion removes the chars of the top word
//   input: array of words to be updated, the word, the size of the array
//  output: removes matching chars from words, prioitzes chars in same pos
void removeChars(char** answers, int* size1, char word[6]) {
  char wordCopy[6];
  strcpy(wordCopy, word);
  for (int i = 0; i < *size1; i++) {
    // if words are not the same, check each letter against letters
    // in answer words (words[0] - words[size1-1])
    
    // check if match in same position first, then remove char
    for (int m = 0; m < 5; m++) {
      if (wordCopy[m] == answers[i][m]) {
        answers[i][m] = ' ';
        wordCopy[m] = ' ';
      }
    }
    for (int m = 0; m < 5; m++) {
      if (wordCopy[m] != ' ') {
        for (int n = 0; n < 5; n++) {
          if (wordCopy[m] == answers[i][n]) {
            answers[i][n] = ' ';
          }
        }
      }
    }
    strcpy(wordCopy, word);
  }
}

//-------------------------------------------------------------------------------
// computeScores() : this funtion compares the words of the words array
//                   to the words in the answers array char by char
//                   it then adds up a score, +3 if chars match in same pos
//                   +1 if chars match in diff pos
//   input: array of word structs, the words from answers file, size of ansers
//          total size (number of words), bool to write the words to ans arr
//  output: updates scores in word structs
void computeScores(struct wordScore* words, char** answers, 
                   int* size1, int* total, bool write) {
  // create copy of answers+words to update without destroying full words
  char** answersCopy = malloc(*size1 * sizeof(char*));
  char** wordsCopy = malloc(*total * sizeof(char*));
  for (int i = 0; i < *total; i++) {
    wordsCopy[i] = malloc(6 * sizeof(char));
    strcpy(wordsCopy[i], words[i].word);
    // answers are size of answers file (size1)
    if (i < *size1) {  
      answersCopy[i] = malloc(6 * sizeof(char));
      
      if (write)
        strcpy(answers[i], words[i].word);
      strcpy(answersCopy[i], answers[i]);
    }
  }
  for (int i = 0; i < *total; i++) {
    words[i].score = 0;
    for (int j = 0; j < *size1; j++) {
        // check if match in same position first, then omit char
        for (int m = 0; m < 5; m++) {
          if (wordsCopy[i][m] != ' ') {
            if (wordsCopy[i][m] == answersCopy[j][m]) {
              answersCopy[j][m] = ' ';
              wordsCopy[i][m] = ' ';
              words[i].score = words[i].score + 3;
            }
          }
        }
        // if words are not the same, check each letter against letters
        // in answer words (words[0] - words[size1-1])
        for (int m = 0; m < 5; m++) {
          if (wordsCopy[i][m] != ' ') {
            for (int n = 0; n < 5; n++) {
              if (words[i].word[m] == answersCopy[j][n]) {
                words[i].score = words[i].score + 1;
                answersCopy[j][n] = ' ';
                break;
              }
            }
          }
        }
      // reset words
      strcpy(answersCopy[j], answers[j]);
      strcpy(wordsCopy[i], words[i].word);
    } 
  }
}

//-------------------------------------------------------------------------------
// countFile() : reads through file and counts number of words
//   input: name of file
//  output: number of elements is returned
int countFile(char* fileName) {
  // printf(" count\n");
  FILE *inFilePtr = fopen(fileName, "r");
  char inputString[81];
  int count = 0;
  if (inFilePtr == NULL) {
    printf("Error: could not open %s for reading\n", fileName);
    exit(-1); // must include stdlib.h
  }
  // Read each word from file and print it, do stuff
  while (fscanf(inFilePtr, "%s", inputString) != EOF) {
    count++;
  }
  fclose(inFilePtr);
  return count;
}

//-------------------------------------------------------------------------------
// readFiles() : reads through files and stores words in word of each struct
//   input: name of files, array of structs to be updated
//  output: struct array is updated with file(s) contents
void readFiles(char* fileName1, char* fileName2, struct wordScore* words) {
  // printf(" read\n");
  FILE *inFilePtr = fopen(fileName1, "r");
  char inputString[81];
  int i = 0;
  if (inFilePtr == NULL) {
    printf("Error: could not open %s for reading\n", fileName1);
    exit(-1); // must include stdlib.h
  }
  // Read each word store in struct
  while (fscanf(inFilePtr, "%s", inputString) != EOF) {
    strcpy(words[i].word, inputString);
    i++;
  }
  // read second file
  inFilePtr = fopen(fileName2, "r");
  if (inFilePtr == NULL) {
    printf("Error: could not open %s for reading\n", fileName2);
    exit(-1); // must include stdlib.h
  }
  // Read each word from file store in struct.
  while (fscanf(inFilePtr, "%s", inputString) != EOF) {
    strcpy(words[i].word, inputString);
    i++;
  }
  fclose(inFilePtr);
}

//-------------------------------------------------------------------------------
// findFirstScores() : this function finds the top scores of the words array
//   input: name of files, size of files, words array, answers array
//  output: ouptuts top scoring words from words array (sorted)
void findFirstScores(char* fileName1, char* fileName2, int* size1, int* size2,
                     struct wordScore* words) {
  int total = *size1 + *size2;
  printf("%s has %d words\n", fileName1, *size1);
  printf("%s has %d words\n\n", fileName2, *size2);
  char** answers = malloc(*size1 * sizeof(char*));
  for (int i = 0; i < *size1; i++) {
    answers[i] = malloc(6 * sizeof(char));
    strcpy(answers[i], words[i].word);
  }
  computeScores(words, answers, size1, &total, true);
  qsort(words, total, sizeof(struct wordScore), compareFunction);
  printf("Words and scores for top first words:\n");
  for (int i = 0; words[i].score == words[0].score; i++) {
    printf("%s %d\n", words[i].word, words[i].score);
  }
}

//-------------------------------------------------------------------------------
// findSecondScores() : this function finds the top scores of the words array
//                      the matching chars in answers words and top scoring 
//                      words are removed from the answer words
//                      words are re-scored against new answer words
//   input: words array, total size of files, size of answers file, answers arr
//  output: ouptuts top scoring words, scored against new answer words
void findSecondScores(struct wordScore* words, int* total, int* size1) {
  int numTop = 0;
  int maxScore = 0;
  char** answers = malloc(*size1 * sizeof(char*));
  for (int i = 0; i < *size1; i++) {
    answers[i] = malloc(6 * sizeof(char));
    strcpy(answers[i], words[i].word);
  }
  computeScores(words, answers, size1, total, false);
  qsort(words, *total, sizeof(struct wordScore), compareFunction);
  maxScore = words[0].score;
  while (words[numTop].score == maxScore) {
    numTop++;
  }
  struct wordScore *topWords = malloc(numTop * sizeof(struct wordScore));
  for (int i = 0; i < numTop; i++) {
    strcpy(topWords[i].word, words[i].word);
    topWords[i].score = words[i].score;
  }
  char** newAnswers = malloc(*size1 * sizeof(char*));
  for (int i = 0; i < *size1; i++) {
    newAnswers[i] = malloc(6 * sizeof(char));
    strcpy(newAnswers[i], answers[i]);
  }
  printf("Words and scores for top first words and second words:\n");
  for (int i = 0; i < numTop; i++) {
    removeChars(newAnswers, size1, topWords[i].word);
    computeScores(words, newAnswers, size1, total, false);
    qsort(words, *total, sizeof(struct wordScore), compareFunction);
    printf("%s %d\n", topWords[i].word, topWords[i].score);
    for (int j = 0; words[j].score == words[0].score; j++)
      printf("   %s %d", words[j].word, words[j].score);
    printf("\n");
    // reset answer words
    for (int i = 0; i < *size1; i++) {
      strcpy(newAnswers[i], answers[i]);
    }
  }
}

//-------------------------------------------------------------------------------
int main() {
  char answersFileName[81] = "answersTiny.txt";
  char guessesFileName[81] = "guessesTiny.txt";
  int size1; // size of answers file
  int size2; // size of guesses file
  printf("Default file names are %s and %s\n", answersFileName,
         guessesFileName);
  // Display menu, to allow partial credit for different program components
  int menuOption = 0;
  while (menuOption != 4) {
    printf("\n");
    printf("Menu Options:\n");
    printf("  1. Display best first words only\n");
    printf("  2. Display best first and best second words\n");
    printf("  3. Change answers and guesses filenames\n");
    printf("  4. Exit\n");
    printf("Your choice: ");
    scanf("%d", &menuOption);
    size1 = countFile(answersFileName);
    size2 = countFile(guessesFileName);
    struct wordScore *words = 
      (struct wordScore*)malloc((size1 + size2) * sizeof(struct wordScore));
    readFiles(answersFileName, guessesFileName, words);
    if (menuOption == 1) {
      // find scores for each word in both files
      // store in struct
      // display each word how it should be
      findFirstScores(answersFileName, guessesFileName,
                      &size1, &size2, words);
      menuOption = 4;
    } else if (menuOption == 2) {
      // take best words from first pass
      // find scores based on best words
      // store new scores
      // display first and second words scores
      int total = size1 + size2;
      printf("%s has %d words\n", answersFileName, size1);
      printf("%s has %d words\n\n", guessesFileName, size2);
      findSecondScores(words, &total, &size1);
      menuOption = 4;
    } else if (menuOption == 3) {
      // Change file names.  Menu will then be redisplayed.
      printf("Enter new answers and guesses filenames: ");
      scanf("%s %s", answersFileName, guessesFileName);
    } else if (menuOption == 4) {
      exit(1); // Exit the program
    } else if (menuOption == 5) {
      strcpy(answersFileName,"answersLarge.txt");
      strcpy(guessesFileName,"guessesLarge.txt");
      printf("Default file names are %s and %s\n", answersFileName,
        guessesFileName);
    }
    free(words);
  } 
  printf("Done\n");
  return 0;
} // end main()
