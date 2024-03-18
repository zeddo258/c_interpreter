#include <cctype>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
/*
 *
 *
 *
 */
typedef enum {
  NUMBER,
  STRING,
  PROGRAM,
} TokenType;

typedef struct {
  TokenType type;
  std::string value;
} Token;
/*
 tokenizer implementation
*/

class Tokenizer {
  std::string str;
  int cursor;

public:
  /*
   initializer
  */
  Tokenizer() : str(""), cursor(0) {}
  Tokenizer(std::string str) : str(str), cursor(0) {}

  /*
  Helper function to check if there are more token to be cut
    */
  bool hasMoreToken() {
    return this->cursor < this->str.length();
  } // hasMoreToken()

  /*
   Get next token */
  Token *getNextToken() {
    if (!hasMoreToken()) {
      return NULL;
    } //

    // Number encountered
    if (isdigit(str.at(cursor))) {
      std::string num = "";
      while (isdigit(str[cursor])) {
        num += str[cursor++];
      } // while
      if (str[cursor] == '.') {
        num += str[cursor++];
        while (isdigit(str[cursor])) {
          num += str[cursor++];
        } // while
      }   // if
      return new Token{NUMBER, num};
    } // if
  }
};

/*
Parser implementation
 */
class Parser {
  std::string str;
  Tokenizer *tokenizer;
  Token *lookahead;

public:
  Parser() : str(""), tokenizer(NULL), lookahead(NULL) {}

  // Parse a string into AS
  Token *parse(std::string str) {
    this->str = str;
    this->tokenizer = new Tokenizer(this->str);
    this->lookahead = tokenizer->getNextToken();
    return this->num();
  } // parse

  Token *num() {
    Token *token = this->eat(NUMBER);
    return token;
  } // num

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
    } catch (const std::runtime_error &e) {
      std::cout << e.what() << std::endl;
      return nullptr; // Or handle the error accordingly
    }
  } // eat
};

int main() {
  std::string str = "1.4";
  Parser *parser = new Parser();
  Token *token = parser->parse(str);
  if (token != NULL)
    std::cout << "Value : " << token->value << '\n'
              << "Type : " << token->type << '\n';
}
