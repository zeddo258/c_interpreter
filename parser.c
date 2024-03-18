#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIZE 256

// Token types
typedef enum {
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_MULTIPLY,
  TOKEN_DIVIDE,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LESSEQ,
  TOKEN_MOREEQ,
  TOKEN_LESS,
  TOKEN_MORE,
  TOKEN_EQ,
  TOKEN_ASSIGN,
  TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
  TokenType type;
  char *value;
} Token;

// Function to tokenize input
Token tokenize(char *input) {

  bool reScan = false;
  char ch = *input;
  Token token;
  // Skip white space
  while (isspace(ch)) {
    scanf("%c", &ch);
  } // while

  // Identifiers ( check for the first character in token is alpha )
  if (isalpha(ch)) {
    char *buffer = malloc(sizeof(char) * INPUT_SIZE);
    int input_index = 0;
    bool underFlag = false;

    buffer[input_index++] = ch;
    scanf("%c", &ch);
    while (isalnum(ch) || ch == '_') {
      buffer[input_index++] = ch;
      scanf("%c", &ch);
    } // while
    buffer[input_index] = '\0';
    char *id = malloc(input_index + 1);
    strncpy(id, buffer, input_index + 1);

    token.type = TOKEN_IDENTIFIER;
    token.value = id;
  } // if

  // Number
  else if (isdigit(ch) || ch == '.') {
    char *buffer = malloc(sizeof(char) * INPUT_SIZE);
    int input_index = 0;
    bool floatFlag = (ch == '.') ? true : false;
    buffer[input_index++] = ch;

    scanf("%c", &ch);
    // If the first char is dot
    if (floatFlag == true) {
      while (isalnum(ch)) {
        buffer[input_index++] = ch;
        scanf("%c", &ch);
      } // while
    }   // if

    // if the first char is number
    else {
      bool quit = false;
      while (!quit) {

        if (ch == '.') {
          floatFlag = true;
        } // if

        if ((ch == '.' && floatFlag) || !isdigit(ch)) {
          quit = true;
        } // if

        if (!quit) {
          buffer[input_index++] = ch;
          scanf("%c", &ch);
        } // if

      } // while

    } // else

    buffer[input_index] = '\0';
    char *num = malloc(input_index + 1);
    strncpy(num, buffer, input_index + 1);

    // Create space for one token
    token.type = TOKEN_NUMBER;
    token.value = num;
  } // else

  // Operator cutting
  else if (ch == '/' || ch == '*' || ch == '+' || ch == '-' || ch == '(' ||
           ch == ')') {
    bool commentFlag = false;
    char operator= ch;
    if (ch == '/') {
      scanf("%c", &ch);
      // Command encouter
      if (ch == '/') {
        commentFlag = true;
        while (ch != '\n') {
          scanf("%c", &ch);
        } // skip comment
      }   // if
      // Is divide
      else {
        token.type = TOKEN_DIVIDE;
      } // else
    }   // if

    else if (ch == '*') {
      token.type = TOKEN_MULTIPLY;
    } // else if

    else if (ch == '+') {
      token.type = TOKEN_PLUS;
    } // else if

    else if (ch == '-') {
      token.type = TOKEN_MINUS;
    } // else if

    else if (ch == ')') {
      token.type = TOKEN_RPAREN;
    } // else if

    else if (ch == '(') {
      token.type = TOKEN_LPAREN;
    } // else if

    else if (ch == ';') {
      token.type = TOKEN_EOF;
    } // else if

    if (!commentFlag) {
      char *punct = malloc(3);
      punct[1] = operator;
      punct[2] = '\0';
      token.value = punct;
      reScan = true;
    } // if
  }   // else if

  else if (ch == ':') {
    scanf("%c", &ch);
    if (ch != '=') {
      printf("Unrecognize token with first char : %c\n", ch);
      token.value = NULL;
      token.type = TOKEN_EOF;
    } // if

    else {
      token.value = ":=";
      token.type = TOKEN_ASSIGN;
      reScan = true;
    } // else
  }   // else if

  else if (ch == '<' || ch == '>' || ch == '=') {
    // if less sign

    if (ch == '<') {
      scanf("%c", &ch);
      if (ch == '=') {
        token.type = TOKEN_LESSEQ;
        token.value = "<=";
        reScan = true;
      } // if
      else {
        token.type = TOKEN_LESS;
        token.value = "<";
      } // else
    }   // if

    else if (ch == '=') {
      token.type = TOKEN_EQ;
      token.value = "=";
      reScan = true;
    } // else if

    else if (ch == '>') {
      scanf("%c", &ch);
      if (ch == '=') {
        token.type = TOKEN_MOREEQ;
        token.value = ">=";
        reScan = true;
      } // if
      else {
        token.type = TOKEN_MORE;
        token.value = ">";
      } // else
    }   // else if
  }     // else if

  else if (ch == ';') {
    char *punct = malloc(2);
    punct[0] = ch;
    punct[1] = '\0';
    token.value = punct;
    token.type = TOKEN_EOF;
  }

  else {
    printf("Unrecognize token with first char : %c\n", ch);
    token.value = NULL;
    token.type = TOKEN_EOF;
  } // else

  if (!reScan) {
    *input = ch;
  } //
  //
  else {
    scanf("%c", &ch);
    *input = ch;
  } // else
  return token;
}

int main() {

  int token_index = 0;
  bool quit = false;
  char ch;
  Token token;

  printf(">");
  scanf("%c", &ch);
  while (!quit) {
    token = tokenize(&ch);

    if (token.value != NULL && strcmp(token.value, "quit") == 0 &&
        token_index == 0) {
      quit = true;
    } // if

    if (token.value == NULL && token.type == TOKEN_EOF) {
      printf(">");
      scanf("%c", &ch);
    } // if

    if (token.value != NULL && strcmp(token.value, ";") == 0) {
      printf(">");
      scanf("%c", &ch);
    } // if

    printf("%s\n", token.value);
  } // while

  printf("Program exits...");
  return 0;
}
