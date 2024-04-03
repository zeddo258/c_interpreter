#include <cctype>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

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
  EOF_TYPE,
  LESS,
  MORE
} TokenType;

struct Token {
  TokenType type;
  string value;
};

/*
 * Interfaces for abstract syntax tree
 * */

typedef enum {
  PROGRAM,
  NUMERIC_LITERAL,
  IDENTIFIER,
  BINARY_EXP,
  UNARY_EXP,
  NULL_LITERAL
} NodeType;

typedef enum { NUMBER_VAL, FLOAT_VAL, BOOLEAN } ValueType;

class Statement {
  NodeType type;

public:
  virtual json print() = 0;
  virtual NodeType getType() = 0;
  virtual Token *getToken() = 0;
};

class Program : public Statement {
  NodeType type;
  vector<Statement *> body;

public:
  Program() { type = PROGRAM; } //

  NodeType getType() { return this->type; } // getType()

  Token *getToken() { return NULL; } //

  void append(Statement *stmt) { this->body.push_back(stmt); } // append()

  vector<Statement *> getBody() { return this->body; } // getBody()

  json print() {
    for (auto e : body) {
      cout << setw(4) << e->print() << endl;
    }
    return NULL;
  } // print()
};

class Expr : public Statement {};

class Null_Expr : public Expr {
  NodeType *type;

public:
  json print() { return {{"Type", "Null type"}, {"value", "NULL"}}; } // print()
};

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

  NodeType getType() { return this->type; } // getType

  Expr *getLeft() { return this->left; } // getLeft()

  Expr *getRight() { return this->right; } // getRight()

  Token *getOp() { return this->op; } // getOp()

  Token *getToken() { return NULL; } //

  json print() {
    json js = {{"Type", "BINARY_EXP"},
               {"OP", this->op->value},
               {"Left", left->print()},
               {"Right", right->print()}};
    return js;
  } // print()
};

class Unary_Expr : public Expr {
  NodeType type;
  Expr *argument;
  string op;

public:
  Unary_Expr(Expr *arg, string op) {
    this->argument = arg;
    this->op = op;
    this->type = UNARY_EXP;
  } // Unary_Expr()

  NodeType getType() { return this->type; } // getType

  Token *getToken() { return NULL; } //

  string getOp() { return this->op; } // getOp()

  Expr *getArgs() { return this->argument; } // get argument

  json print() {
    return {{"Type ", "Unary"},
            {"OP ", this->op},
            {"Arg", this->argument->print()}};
  }
};

class Identifier : public Expr {
  NodeType type;
  Token *token;

public:
  Identifier(NodeType tp, Token *token) {
    this->type = tp;
    this->token = token;
  } // Identifier

  NodeType getType() { return this->type; } // getType

  Token *getToken() { return this->token; } // getValue()

  json print() {
    return {{"Type:", this->type}, {"Value:", this->token->value}};
  } // print()
};

class NumericLiteral : public Expr {
  NodeType type;
  Token *token;

public:
  NumericLiteral(NodeType tp, Token *token) {
    this->type = tp;
    this->token = token;
  } // Numeric

  NodeType getType() { return this->type; } // getType

  Token *getToken() { return this->token; } // getValue()

  json print() {
    return {{"Type", this->type}, {"Value", this->token->value}};
  } // print()
};

class RuntimeVal {
public:
  virtual ValueType getValueType() = 0;
  virtual string getValue() = 0;
};

class NumberVal : public RuntimeVal {
  ValueType type;
  int value;

public:
  NumberVal(ValueType tp, int val) {
    this->value = val;
    this->type = tp;
  } // NumberVal()

  string getValue() {
    stringstream ss;
    ss << value;
    string str = ss.str();
    return str;
  } // getValue()

  ValueType getValueType() { return this->type; } // getValueType();
};

class FloatVal : public RuntimeVal {
  ValueType type;
  float value;

public:
  FloatVal(ValueType tp, float val) {
    this->value = val;
    this->type = tp;
  } // NumberVal()

  string getValue() {
    string str = to_string(value);
    return str;
  } // getValue()

  ValueType getValueType() { return this->type; } // getValueType();
};

class BoolVal : public RuntimeVal {
  ValueType type;
  bool value;

public:
  BoolVal(ValueType tp, bool val) : type(tp), value(val) {}

  string getValue() {
    return this->value == true ? "true" : "false";
  } // getValue()

  ValueType getValueType() { return this->type; } // getValueType();
};

class Interpreter {
  Program *program;

public:
  Interpreter(Program *pr) { this->program = pr; };

  void evaluate_program() {
    vector<Statement *> body = program->getBody();
    for (auto e : body) {
      cout << this->evaluate(e)->getValue() << endl;
    } // for
  }   // evaluate_program()

  RuntimeVal *calculate_int(RuntimeVal *left, Token *op, RuntimeVal *right) {
    RuntimeVal *result_val = NULL;
    string left_str = left->getValue();
    string right_str = right->getValue();

    int result;
    int left_int = atoi(&left_str[0]);
    int right_int = atoi(&right_str[0]);
    if (op->value == "+") {
      result = left_int + right_int;
    } // if

    else if (op->value == "-") {
      result = left_int - right_int;
    } // else if

    else if (op->value == "*") {
      result = left_int * right_int;
    } // else if

    else if (op->value == "/") {
      result = left_int / right_int;
    } // else if

    else if (op->value == ">") {
      if (left_int > right_int) {
        result_val = new BoolVal(BOOLEAN, true);
        return result_val;
      } // if

      else {
        result_val = new BoolVal(BOOLEAN, false);
        return result_val;
      } // else
    }   // else if

    else if (op->value == "<") {
      if (left_int < right_int) {
        result_val = new BoolVal(BOOLEAN, true);
        return result_val;
      } // if

      else {
        result_val = new BoolVal(BOOLEAN, false);
        return result_val;
      } // else
    }   // else if

    result_val = new NumberVal(NUMBER_VAL, result);
    return result_val;
  } // calculate()

  RuntimeVal *calculate_float(RuntimeVal *left, Token *op, RuntimeVal *right) {
    RuntimeVal *result_val = NULL;
    string left_str = left->getValue();
    string right_str = right->getValue();

    float result;
    float left_int = atof(&left_str[0]);
    float right_int = atof(&right_str[0]);

    if (op->value == "+") {
      result = left_int + right_int;
    } // if

    else if (op->value == "-") {
      result = left_int - right_int;
    } // else if

    else if (op->value == "*") {
      result = left_int * right_int;
    } // else if

    else if (op->value == "/") {
      result = left_int / right_int;
    } // else if

    else if (op->value == ">") {
      if (left_int > right_int) {
        result_val = new BoolVal(BOOLEAN, true);
        return result_val;
      } // if

      else {
        result_val = new BoolVal(BOOLEAN, false);
        return result_val;
      } // else
    }   // else if

    else if (op->value == "<") {
      if (left_int < right_int) {
        result_val = new BoolVal(BOOLEAN, true);
        return result_val;
      } // if

      else {
        result_val = new BoolVal(BOOLEAN, false);
        return result_val;
      } // else
    }   // else if

    result_val = new NumberVal(FLOAT_VAL, result);
    return result_val;
  } // calculate()

  RuntimeVal *evaluate_binary_num(RuntimeVal *left, Token *op,
                                  RuntimeVal *right) {
    RuntimeVal *result = NULL;
    // if one of them are float
    if (left->getValueType() == FLOAT_VAL ||
        right->getValueType() == FLOAT_VAL) {
      result = calculate_float(left, op, right);
    } // if

    else if (left->getValueType() == NUMBER_VAL &&
             right->getValueType() == NUMBER_VAL) {
      result = calculate_int(left, op, right);
    } // else if

    return result;
  } // evaluate_binary_num()

  RuntimeVal *evaluate_binary(class Binary_Expr *binary_expr) {
    Statement *left_statement =
        dynamic_cast<Statement *>(binary_expr->getLeft());
    RuntimeVal *left_value = this->evaluate(left_statement);
    Statement *right_statement =
        dynamic_cast<Statement *>(binary_expr->getRight());
    RuntimeVal *right_value = this->evaluate(right_statement);
    Token *op = binary_expr->getOp();
    RuntimeVal *result = NULL;
    if ((left_value->getValueType() == NUMBER_VAL ||
         left_value->getValueType() == FLOAT_VAL) &&
        (right_value->getValueType() == NUMBER_VAL ||
         right_value->getValueType() == FLOAT_VAL)) {
      result = this->evaluate_binary_num(left_value, op, right_value);
    } // if

    return result;
  } // evaluate_binary

  class RuntimeVal *evaluate_unary(class Unary_Expr *unary) {
    string sign = unary->getOp();
    Statement *args = unary->getArgs();
    RuntimeVal *right_arg_value = this->evaluate(args);
    string result_val = sign + right_arg_value->getValue();
    RuntimeVal *result = NULL;
    if (right_arg_value->getValueType() == FLOAT_VAL) {
      float num = atof(&result_val[0]);
      result = new FloatVal(FLOAT_VAL, num);
    } // if

    else if (right_arg_value->getValueType() == NUMBER_VAL) {
      int num = atoi(&result_val[0]);
      result = new NumberVal(NUMBER_VAL, num);
    } // else if

    return result;
  } // evaluate_unary()

  /*Function to evaluate statement*/
  class RuntimeVal *evaluate(class Statement *stmt) {
    RuntimeVal *value = NULL;
    // if statement is num
    if (stmt->getType() == NUMERIC_LITERAL) {
      Token *tok = stmt->getToken();
      // Tok return null when it not NUMERIC or IDENTIFIER
      if (tok != NULL) {
        string val = tok->value;
        // process whole number
        if (val.find('.') == string::npos) {
          int num = atoi(val.c_str());
          value = new NumberVal(NUMBER_VAL, num);

          return value;
        } // if
        else {
          float num = atof(val.c_str());
          value = new FloatVal(FLOAT_VAL, num);
          return value;
        } // else
      }   // if
    }     // if

    /*
      If statement is identifier
    */

    /*
      If statement is unary
    */
    else if (stmt->getType() == UNARY_EXP) {
      Unary_Expr *unary = dynamic_cast<Unary_Expr *>(stmt);
      value = evaluate_unary(unary);

    } // else if
    /*
      If statement is binary
    */

    else if (stmt->getType() == BINARY_EXP) {
      Binary_Expr *binary_exp = dynamic_cast<Binary_Expr *>(stmt);
      value = evaluate_binary(binary_exp);
    } // else if

    return value;
  } // evaluate
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
    Token *tok = new Token;
    tok->type = NUMBER;
    tok->value = num;
    return tok;
  } // sliceNum

  Token *sliceFloat() {
    Token *tok = new Token;
    string num = "";
    num += str[cursor];
    cursor++;
    while (isdigit(str[cursor])) {
      num += str[cursor++];
    } // while
    if (num != ".") {
      tok->type = NUMBER;
      tok->value = num;
      return tok;
    } // if

    tok->type = NULL_TYPE;
    tok->value = "";
    return tok;
  } // sliceFloat

  Token *sliceID() {
    string id = "";
    while (isalnum(str[cursor]) || str[cursor] == '_') {
      id += str[cursor++];
    } // while
    Token *tok = new Token;
    tok->type = ID;
    tok->value = id;
    return tok;
  } // sliceID()

  Token *slicePar() {
    string par = "";
    par += str[cursor++];
    Token *tok = new Token;
    if (par == ")") {
      tok->type = CLOSE_PAR;
      tok->value = par;
    } // if

    else if (par == "(") {
      tok->type = OPEN_PAR;
      tok->value = par;
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
      tok = new Token;
      tok->type = END;
      tok->value = ";";
      cursor++;
      return tok;
    } // else if

    else if (str[cursor] == '+' || str[cursor] == '-' || str[cursor] == '*') {
      string sign = "";
      sign += str[cursor++];
      tok = new Token;
      tok->type = SIGN;
      tok->value = sign;
      return tok;
    } // else if

    else if (str[cursor] == '(' || str[cursor] == ')') {
      return slicePar();
    } // else if

    else if (str[cursor] == ':' && str[cursor + 1] == '=') {
      string set = ":=";
      cursor += 2;
      tok = new Token;
      tok->type = SET;
      tok->value = set;
      return tok;
    } // else if

    else if (str[cursor] == '/') {
      string cmt = "";
      cmt += str[cursor++];
      cout << cursor;
      // skip comment
      if (str[cursor] == '/') {
        while (str[cursor++] != '\n') {
        } // while
        return getNextToken();
      } // if

      else {
        tok = new Token;
        tok->type = SIGN;
        tok->value = cmt;
        return tok;
      } // else
    }   // else if

    else if (str[cursor] == '<') {
      string less = "<";
      cursor++;
      tok = new Token;
      tok->type = LESS;
      tok->value = less;
      return tok;
    } // else if

    else if (str[cursor] == '>') {
      string more = ">";
      cursor++;
      tok = new Token;
      tok->type = MORE;
      tok->value = more;
      return tok;
    } // else if

    string unknown = "";
    unknown += str[cursor];
    tok = new Token;
    tok->type = NULL_TYPE;
    tok->value = unknown;
    return tok;
  } // getNextToken()
};

class Parser {
  string str;
  Tokenizer *tokenizer;
  Token *lookahead;
  vector<Token *> tokens;
  // 2 + -(432+2)
  class Expr *parse_unary_expression() {
    Token *op = this->eat(SIGN);
    class Expr *arg = this->parse_primary_expression();
    class Expr *unary = new Unary_Expr(arg, op->value);
    return unary;
  } // parse_unary_expression()

  class Expr *parse_primary_expression() {
    class Expr *expr = NULL;

    if (this->lookahead->type == NUMBER) {
      expr = new NumericLiteral(NUMERIC_LITERAL, this->lookahead);
      this->eat(NUMBER);
    } // if

    else if (this->lookahead->type == ID) {
      expr = new Identifier(IDENTIFIER, this->lookahead);
      this->eat(ID);
    } // else if

    else if (this->lookahead->type == SIGN) {
      expr = this->parse_unary_expression();
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
    while (this->lookahead != NULL &&
           (this->lookahead->value == "+" || this->lookahead->value == "-")) {
      Token *op = this->eat(SIGN);
      class Expr *right = this->parse_multi_exp();
      left = new Binary_Expr(left, right, op);
    } // while

    return left;
  } // parse_additive_exp()

  class Expr *parse_compare_exp() {
    class Expr *left = this->parse_additive_exp();
    while (this->lookahead != NULL &&
           (this->lookahead->value == "<" || this->lookahead->value == ">")) {
      TokenType type = this->lookahead->type;
      Token *op = this->eat(type);
      class Expr *right = this->parse_additive_exp();
      left = new Binary_Expr(left, right, op);
    } // while

    return left;
  } // parse_compare_exp()

  class Expr *parseExpression() {
    Expr *expr = this->parse_compare_exp();
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
    while (this->lookahead == NULL) {
      getline(cin, str);

      free(this->tokenizer);
      free(this->lookahead);

      this->str = str;
      this->tokenizer = new Tokenizer(this->str);
      this->lookahead = this->tokenizer->getNextToken();

    } // while
    if (this->lookahead->value == "quit") {
      return NULL;
    }
    class Statement *stmt = parseStmt();
    program->append(stmt);
    while (this->lookahead->type != END) {
      stmt = parseStmt();
      program->append(stmt);
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

      if (this->lookahead == NULL) {
        string str;
        getline(cin, str);
        free(this->tokenizer);
        free(this->lookahead);

        this->str = str;
        this->tokenizer = new Tokenizer(this->str);
        this->lookahead = tokenizer->getNextToken();
      } // if

      return token;
    } catch (const runtime_error &e) {
      cout << e.what() << endl;
      return NULL; // Or handle the error accordingly
    }
  } // eat
};

int main() {
  string str = "";
  bool quit = false;
  Parser *parser = new Parser();
  cout << "> ";
  getline(cin, str);
  while (quit == false) {
    Program *program = parser->parse(str);
    if (program == NULL) {
      quit = true;
    } // if
    else {
      json js = program->print();
      Interpreter *inter = new Interpreter(program);
      inter->evaluate_program();
      cout << "> ";
      getline(cin, str);
    } // else
  }   // while
} // main();
