#include <cctype>
#include <ctype.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

using namespace std;
typedef enum {
  NUMBER,
  STRING,
  ID,
  SIGN,
  PROGRAM,
  NULL_TYPE,
} TokenType;

typedef struct {
  TokenType type;
  string value;
} Token;
/*
 tokenizer implementation
*/

class Tokenizer {
  string str;
  int cursor;

public:
  /*
   initializer
  */
  Tokenizer() : str(""), cursor(0) {}
  Tokenizer(string str) : str(str), cursor(0) {}

  // Helper function to check if there are more token to be cut
  bool hasMoreToken() {
    return this->cursor < this->str.length();
  } // hasMoreToken()

  // Helper function to cut slice num
  Token *sliceNum() {
    string num = "";
    while (isdigit(str[cursor])) {
      num += str[cursor++];
    } // while
    if (str[cursor] == '.') {
      num += str[cursor++];
      while (isdigit(str[cursor])) {
        num += str[cursor++];
      } // while
    }   // if
    Token *tok = new Token{NUMBER, num};
    return tok;
  } // sliceNum

  Token *sliceFloat() {
    Token *tok;
    string num = "";
    num += str[cursor];
    cursor++;
    while (isdigit(str[cursor])) {
      num += str[cursor++];
    } // while
    if (num != ".") {
      tok = new Token{NUMBER, num};
      return tok;
    } // if

    tok = new Token{NULL_TYPE, ""};
    return tok;
  } // sliceFloat

  Token *sliceID() {
    string id = "";
    while (isalnum(str[cursor]) || str[cursor] == '_') {
      id += str[cursor++];
    } // while
    Token *tok = new Token{ID, id};
    return tok;
  }

  // Get next token
  Token *getNextToken() {
    Token *tok;
    if (!hasMoreToken()) {
      return NULL;
    } //

    // Number encountered eg 1.23 .235 251,....
    if (isdigit(str.at(cursor))) {
      return sliceNum();
    } // if

    // Dot encoutered eg .09, .923,...
    else if (str[cursor] == '.') {
      return sliceFloat();
    } // else if

    else if (isspace(str[cursor])) {
      while (isspace(str[cursor])) {
        cursor++;
      } //

      return getNextToken();
    }
    // alphabet character encountered
    // slicing id Hello_world, temp, flag,....
    else if (isalpha(str[cursor])) {
      return sliceID();
    } // else if

    else if (str[cursor] == '+' || str[cursor] == '-' || str[cursor] == '*') {
      string sign = "";
      sign += str[cursor++];
      tok = new Token{SIGN, sign};
      return tok;
    } // else if

    else if (str[cursor] == '/') {
      string cmt = "";
      cmt += str[cursor++];
      if (str[cursor] == '/') {
        while (str[cursor] != '\n') {
          cursor++;
        } // while
      }   // if
      return getNextToken();
    } // else if
    tok = new Token{NULL_TYPE, ""};
    return NULL;
  }
};

/*
Parser implementation
 */
class Parser {
  string str;
  Tokenizer *tokenizer;
  Token *lookahead;

public:
  Parser() : str(""), tokenizer(NULL), lookahead(NULL) {}

  // Parse a string into AS
  map<string, Token *> parse(string str) {
    this->str = str;
    this->tokenizer = new Tokenizer(this->str);
    this->lookahead = tokenizer->getNextToken();
    return program();
  } // parse

  map<string, Token *> program() {
    map<string, Token *> mmap;
    mmap.insert({"PROGRAM", this->literal()});
    return mmap;
  } // program()

  Token *num() {
    Token *token = this->eat(NUMBER);
    return token;
  } // num

  Token *ident() {
    Token *token = this->eat(ID);
    return token;
  }

  Token *literal() {
    if (this->lookahead->type == NUMBER) {
      return this->num();
    } // if
    else if (this->lookahead->type == ID) {
      return this->ident();
    } // else if
  }   // literal

  Token *eat(TokenType type) {
    try {
      Token *token = this->lookahead;
      if (token == nullptr) {
        throw std::runtime_error("End of input reached");
      }
      if (token->type != type) {
        throw std::runtime_error("Unexpected token: " + token->value);
      }
      this->lookahead = this->tokenizer->getNextToken();
      return token;
    } catch (const runtime_error &e) {
      cout << e.what() << std::endl;
      return nullptr; // Or handle the error accordingly
    }
  } // eat
};

int main() {
  string str = "// Hello world \n .42";
  Parser *parser = new Parser();
  map<string, Token *> ast = parser->parse(str);
  for (auto it : ast) {
    cout << it.first << ":\n"
         << "Token type : " << it.second->type
         << "Token value : " << it.second->value;
  }
}
