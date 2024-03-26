#include <cctype>
#include <ctype.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

/*
 * Interface for token type
 */
typedef enum {
  NUMBER,
  STRING,
  ID,
  SIGN,
  EQ,
  SET,
  END,
  CLOSE_PAR,
  OPEN_PAR,
  NULL_TYPE,
} TokenType;

typedef struct {
  TokenType type;
  string value;
} Token;

/*
 * Interfaces for abstract syntax tree
 * */

typedef enum {
  PROGRAM,
  NUMERIC_LITERAL,
  IDENTIFIER,
  BINARY_EXP,
} NodeType;

class Statement {
public:
  virtual void print() = 0;
};

class Program : public Statement {
  NodeType type;
  vector<Statement *> body;

public:
  Program() { type = PROGRAM; } //

  void append(Statement *stmt) { this->body.push_back(stmt); } // append()

  void print() {
    cout << "{ Type : Program "
         << "\n\tBody: \n";
    for (int i = 0; i < body.size(); i++) {
      cout << "\t";
      body[i]->print();
      cout << endl;
    } // for
    cout << "}";
  }
};

class Expr : public Statement {};

class Binary_Expr : public Expr {
  NodeType type;
  Expr *left;
  Expr *right;
  Token *op;

public:
  Binary_Expr(Expr *left, Expr *right, Token *op) {
    this->type = BINARY_EXP;
    this->left = left;
    this->right = right;
    this->op = op;
  } // Binary_Expr()

  void print() {
    cout << "{ Type : Binary_EXP " << endl << "Left : ";
    left->print();
    cout << "OP : " << op->value << endl << "Right : ";
    right->print();
    cout << "}";
  } // print()
};

class Identifier : public Expr {
  NodeType type;
  string symbol;

public:
  Identifier(NodeType tp, string sb) {
    this->type = tp;
    this->symbol = sb;
  } // Identifier

  void print() {
    cout << "{ Type:" << this->type << "Value : " << this->symbol << "}"
         << endl;
  } // print()
};

class NumericLiteral : public Expr {
  NodeType type;
  string value;

public:
  NumericLiteral(NodeType tp, string num) {
    this->type = tp;
    this->value = num;
  } // Numeric
  void print() {
    cout << "{ Type:" << this->type << "; Value : " << this->value << "}"
         << endl;
  } // print()
};

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
  } // sliceID()

  Token *slicePar() {
    string par = "";
    par += str[cursor++];
    Token *tok;
    if (par == ")") {
      tok = new Token{CLOSE_PAR, par};
    } // if

    else if (par == "(") {
      tok = new Token{OPEN_PAR, par};
    } // else if

    return tok;
  } //

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

    else if (str[cursor] == ';') {
      tok = new Token{END, ";"};
      cursor++;
      return tok;
    } // else if

    else if (str[cursor] == '+' || str[cursor] == '-' || str[cursor] == '*') {
      string sign = "";
      sign += str[cursor++];
      tok = new Token{SIGN, sign};
      return tok;
    } // else if

    else if (str[cursor] == '(' || str[cursor] == ')') {
      return slicePar();
    } // else if

    else if (str[cursor] == ':' && str[cursor + 1] == '=') {
      string set = ":=";
      cursor += 2;
      tok = new Token{SET, set};
      return tok;
    } // else if

    else if (str[cursor] == '/') {
      string cmt = "";
      cmt += str[cursor++];
      if (str[cursor] == '/') {
        while (str[cursor] != '\n') {
          cursor++;
        } // while
        return getNextToken();
      } // if
      else {
        tok = new Token{SIGN, cmt};
        return tok;
      } // else
    }   // else if
    string unknown = "";
    unknown += str[cursor];
    tok = new Token{NULL_TYPE, unknown};
    return tok;
  }
};

class Parser {
  string str;
  Tokenizer *tokenizer;
  Token *lookahead;
  vector<Token *> tokens;

  class Expr *parse_primary_expression() {
    class Expr *expr = NULL;

    if (this->lookahead->type == NUMBER) {
      expr = new NumericLiteral(NUMERIC_LITERAL, this->lookahead->value);
      this->eat(NUMBER);
    } // if
    else if (this->lookahead->type == ID) {
      expr = new Identifier(IDENTIFIER, this->lookahead->value);
      this->eat(ID);
    } // else if

    else if (this->lookahead->type == OPEN_PAR) {
      this->eat(OPEN_PAR);
      expr = this->parseExpression();
      this->eat(CLOSE_PAR);
    } // else if

    else if (this->lookahead->type == NULL_TYPE) {
      cout << "Unrecognized token with first char : " << this->lookahead->value
           << endl;
      this->lookahead = NULL;
    } // else if
    return expr;
  } // parse_primary_expression()

  class Expr *parse_multi_exp() {
    class Expr *left = this->parse_primary_expression();
    if (this->lookahead != NULL && this->lookahead->type != SIGN) {
      cout << "Unexpected token : " << this->lookahead->value << endl;
      return NULL;
    } // if

    while (this->lookahead != NULL &&
           (this->lookahead->value == "*" || this->lookahead->value == "/" ||
            this->lookahead->value == "%")) {
      Token *op = this->eat(SIGN);
      class Expr *right = this->parse_primary_expression();
      left = new Binary_Expr(left, right, op);
    } // while()

    return left;
  } // parse_multi_exp()

  class Expr *parse_additive_exp() {
    class Expr *left = this->parse_multi_exp();
    class Expr *right;
    while (this->lookahead != NULL &&
           (this->lookahead->value == "+" || this->lookahead->value == "-")) {
      Token *op = this->eat(SIGN);
      right = this->parse_multi_exp();
      left = new Binary_Expr(left, right, op);
    } // while

    return left;
  } // parse_additive_exp()

  class Expr *parseExpression() {
    Expr *expr = this->parse_additive_exp();
    return expr;
  } // parseExpression

  class Statement *parseStmt() {
    Statement *stmt = this->parseExpression();
    return stmt;
  }

public:
  Parser() : str(""), tokenizer(NULL), lookahead(NULL) {}

  // Parse a string into AST
  class Program *parse(string str) {
    class Program *program = new class Program();
    this->str = str;
    this->tokenizer = new Tokenizer(this->str);
    this->lookahead = tokenizer->getNextToken();
    class Statement *stmt = parseStmt();
    while (stmt != NULL) {
      program->append(stmt);
      stmt = parseStmt();
    } // while
    return program;
  } // parse

  Token *eat(TokenType type) {
    try {
      Token *token = this->lookahead;
      if (token == NULL) {
        throw runtime_error("End of input. ");
      }
      if (token->type != type) {
        throw runtime_error("Unexpected token: " + token->value);
      }
      this->lookahead = this->tokenizer->getNextToken();
      return token;
    } catch (const runtime_error &e) {
      cout << e.what() << endl;
      return NULL; // Or handle the error accordingly
    }
  } // eat
};

int main() {
  string str = "";
  Parser *parser = new Parser();
  cout << "> ";
  getline(cin, str);
  while (str != "quit") {
    Program *program = parser->parse(str);
    if (program != NULL)
      program->print();
    cout << "> ";
    getline(cin, str);
  } // while
} // main();
