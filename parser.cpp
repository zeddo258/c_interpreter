#include <cctype>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

/*
 * Interface for token type
 */
typedef enum {
  ILLEGAL,
  EOFF,
  IDENT,
  FLOAT,
  INT,
  ASSIGN,
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE,
  COMMA,
  SEMICOLON,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LT,
  GT,
  EQ,
  NOT_EQ,
  NOT,

  // Keywords
  KEY_INT,
  KEY_STRING,
  KEY_FLOAT,
  KEY_BOOL,
  KEY_VOID,
  KEY_CUSTOM_TYPE
} TokenType;

struct Token {
  TokenType type;
  string value;
};

/*********************************************************************
 * ************************LEXER**************************************
 *********************************************************************/
class Lexer {
  string mStr;
  size_t mCurrent;
  size_t mPeek;
  char mCh;

public:
  Lexer( string str ) {
    this->mStr = str;
    this->mPeek = 0;
    ReadChar( );
  } // Tokenizer()

  void ReadChar( );

  bool HasMoreToken( );
  void SkipComment( );
  void SkipWhiteSpace( );

  Token *ReadDivide( );
  Token *ReadNot( );
  Token *ReadNumber( );
  Token *ReadID( );
  Token *ReadAssign( );
  Token *ReadPar( );
  Token *ReadOperator( );
  Token *ReadNextToken( );
  char PeekChar( );

  Token *SetNewToken( string value, TokenType type );
};

char Lexer::PeekChar( ) {
  if ( !HasMoreToken( ) )
    return '\0';
  return mStr[mPeek];
} // PeekChar()

// Read and advance
void Lexer::ReadChar( ) {
  if ( !HasMoreToken( ) )
    mCh = '\0';
  else
    mCh = mStr[mPeek];
  mCurrent = mPeek;
  mPeek++;
} // ReadChar

// Helper function to check if there are more token to be cut
bool Lexer::HasMoreToken( ) {
  return this->mPeek < this->mStr.length( );
} // hasMoreToken()

Token *Lexer::ReadAssign( ) {
  Token *tok = NULL;
  if ( PeekChar( ) == '=' ) {
    tok = SetNewToken( ":=", ASSIGN );
    ReadChar( );
  } // if
  else
    tok = SetNewToken( ":", ILLEGAL );
  return tok;
} // ReadAssign()

Token *Lexer::ReadNot( ) {
  Token *tok = NULL;
  if ( PeekChar( ) == '=' ) {
    tok = SetNewToken( "!=", NOT_EQ );
    ReadChar( );
  } // if
  else
    tok = SetNewToken( "!", NOT );
  return tok;
} // ReadNot()

Token *Lexer::ReadDivide( ) {
  Token *tok = NULL;
  if ( PeekChar( ) == '/' ) {
    SkipComment( );
    tok = ReadNextToken( );
  } // if
  else
    tok = SetNewToken( "/", DIVIDE );
  return tok;
} // ReadDivide()

Token *Lexer::ReadNumber( ) {
  Token *tok = NULL;
  string number = "";
  // if it start with an.
  // .122 .22122 .5125.2512 .2124.helloworld
  number += mCh;
  if ( mCh == '.' ) {
    ReadChar( );
    while ( isdigit( mCh ) ) {
      number += mCh;
      ReadChar( );
    } // while

    if ( number.length( ) == 1 )
      tok = SetNewToken( number, ILLEGAL );
    else
      tok = SetNewToken( number, FLOAT );
  } // if

  // 1.231 3.1415 2.19 2 23 1.
  else if ( isdigit( mCh ) ) {
    ReadChar( );
    while ( isdigit( mCh ) || mCh == '.' ) {
      number += mCh;
      // if dot is encoutered
      if ( mCh == '.' ) {
        ReadChar( );
        while ( isdigit( mCh ) ) {
          number += mCh;
          ReadChar( );
        } // while
        tok = SetNewToken( number, FLOAT );
        return tok;
      } // if
      ReadChar( );
    } // while

    tok = SetNewToken( number, INT );
  } // else if

  return tok;
} // GetNumber()

Token *Lexer::ReadID( ) {
  string id = "";
  while ( isalnum( mCh ) || mCh == '_' ) {
    id += mCh;
    ReadChar( );
  } // while
  Token *tok = SetNewToken( id, IDENT );
  return tok;
} // sliceID()

Token *Lexer::SetNewToken( string val, TokenType type ) {
  Token *tok = new Token;
  tok->value = val;
  tok->type = type;
  return tok;
} // SetNewToken()

// ReadOperator
Token *Lexer::ReadOperator( ) {
  Token *tok = new Token;
  string sign = "";
  sign += mCh;
  if ( mCh == '+' )
    tok = SetNewToken( sign, PLUS );
  else if ( mCh == '-' )
    tok = SetNewToken( sign, MINUS );
  else if ( mCh == '*' )
    tok = SetNewToken( sign, MULTIPLY );
  else if ( mCh == '/' )
    tok = SetNewToken( sign, DIVIDE );
  return tok;
} // GetOperator()

void Lexer::SkipWhiteSpace( ) {
  while ( isspace( mCh ) )
    ReadChar( );
} // SkipWhiteSpace()

void Lexer::SkipComment( ) {
  while ( HasMoreToken( ) )
    ReadChar( );
  ReadChar( );
  cout << mCh << endl;
} // SkipComment()

Token *Lexer::ReadPar( ) {
  Token *tok = NULL;
  if ( mCh == '(' )
    tok = SetNewToken( "(", LPAREN );
  else
    tok = SetNewToken( ")", RPAREN );
  return tok;
} // ReadPar()

// Get next token
Token *Lexer::ReadNextToken( ) {
  Token *tok;

  if ( isspace( mCh ) )
    SkipWhiteSpace( );

  // Number encountered eg 1.23 .235 251,....
  if ( isdigit( mCh ) || mCh == '.' ) {
    tok = ReadNumber( );
    return tok;
  } // if

  else if ( mCh == '\0' ) {
    tok = SetNewToken( "\0", EOFF );
  }

  // slicing id Hello_world, temp, flag,....
  else if ( isalpha( mCh ) ) {
    tok = ReadID( );
    return tok;
  } // else if

  else if ( mCh == ';' )
    tok = SetNewToken( ";", SEMICOLON );

  else if ( mCh == '+' || mCh == '-' || mCh == '*' )
    tok = ReadOperator( );

  else if ( mCh == '(' || mCh == ')' )
    tok = ReadPar( );

  else if ( mCh == ':' )
    tok = ReadAssign( );

  else if ( mCh == '=' )
    tok = SetNewToken( "=", EQ );

  else if ( mCh == '<' )
    tok = SetNewToken( "<", LT );

  else if ( mCh == '>' )
    tok = SetNewToken( ">", GT );

  // Skip comment
  else if ( mCh == '/' )
    tok = ReadDivide( );

  else
    tok = SetNewToken( "\0", ILLEGAL );

  ReadChar( );
  return tok;
} // ReadNextToken()

class Node {
public:
  virtual json Print( ) = 0;
};

/*********************************************************************
 **************************Our AST type            *******************
 *********************************************************************/
class Statement : public Node {
public:
  virtual void Stmt( ) = 0;
}; // Statement

class Expression : public Node {
public:
  virtual void Expr( ) = 0;
};

/*********************************************************************
 **************************      Expression        *******************
 *********************************************************************/

class IntExpr : public Expression {
  Token *mTok;
  size_t mValue;

public:
  IntExpr( Token *tok, size_t value ) {
    mValue = value;
    mTok = tok;
  } // NumberExpr()

  json Print( );
  void Expr( );
};

json IntExpr::Print( ) {
  return { { "Type", "Int" }, { "Value", mValue } };
} // Print()

void IntExpr::Expr( ) { cout << mValue << endl; } // Expr()

class FloatExpr : public Expression {
  Token *mTok;
  float mValue;

public:
  FloatExpr( Token *tok, float value ) {
    mValue = value;
    mTok = tok;
  } // NumberExpr()

  json Print( );
  void Expr( );
};

json FloatExpr::Print( ) {
  return { { "Type", "Float" }, { "Value", mValue } };
} // Print()

void FloatExpr::Expr( ) { cout << mValue << endl; } // Expr()

class StringExpr : public Expression {
  string mValue;

public:
  StringExpr( string value ) { mValue = value; } // StringExpr
  void Expr( );
  json Print( );
};

json StringExpr::Print( ) {
  return { { "Type ", "StringExpr" }, { "Value", mValue } };
} // Print()

void StringExpr::Expr( ) {} // expr

class BinExpr : public Expression {
  Expression *mLeft;
  Token *mOp;
  Expression *mRight;

public:
  BinExpr( Expression *left, Token *op, Expression *right ) {
    mLeft = left;
    mOp = op;
    mRight = right;
  } // BinExpr()

  void Expr( );

  json Print( );
};

json BinExpr::Print( ) {
  return { { "Type", "Binary Expr" },
           { "Left", mLeft->Print( ) },
           { "OP", mOp->value },
           { "Right", mRight->Print( ) } };
} // Print()

void BinExpr::Expr( ) {} // expr

class SymbolExpression : public Expression {
  Token *mTok;
  string mValue;

public:
  SymbolExpression( Token *tok, string val ) {
    mValue = val;
    mTok = tok;
  } // SymbolExpression

  void Expr( ) {} // Expr
  json Print( );
};

json SymbolExpression::Print( ) {
  return { { "Type", "SymbolExpression" }, { "Value", mValue } };
} // Print()

class UnaryExpression : public Expression {
  Token *mOp;
  Expression *mRhs;

public:
  UnaryExpression( Token *op, Expression *right ) {
    mOp = op;
    mRhs = right;
  } // UnaryExpression()

  void Expr( ) {}
  json Print( );
};

json UnaryExpression::Print( ) {
  return { { "Type", "Unary Expression " },
           { "Operator", mOp->value },
           { "Right Expression", mRhs->Print( ) } };
} // Print()

/*********************************************************************
 **************************      Statement         *******************
 *********************************************************************/
class AssignmentStmt : public Statement {
  Token *mToken;           // The type token
  SymbolExpression *mName; // Variable name
  Expression *mValue;

public:
  AssignmentStmt( Token *token, SymbolExpression *name, Expression *value ) {
    mToken = token;
    mName = name;
    mValue = value;
  } // AssignmentStmt()

  void Stmt( ) {};
  json Print( );
};

json AssignmentStmt::Print( ) {
  return { { "Var", mName->Print( ) }, { "Value", mValue->Print( ) } };
} // AssignmentStmt

class Program : public Statement {

public:
  vector<Statement *> mBody;
  void Stmt( );
  void Append( Statement *stmt );
  json Print( );
};

json Program::Print( ) {
  for ( auto e : mBody )
    cout << setw( 4 ) << e->Print( ) << endl;
  return NULL;
} // Print()

void Program::Stmt( ) {}

void Program::Append( Statement *stmt ) { mBody.push_back( stmt ); } // Append()

class ExpressionStatement : public Statement {
  Token *mToken;
  Expression *mExpr;

public:
  ExpressionStatement( Expression *expr, Token *token ) {
    mExpr = expr;
    mToken = token;
  } // ExpressionStatement()

  void Stmt( ) {}
  json Print( );
};

json ExpressionStatement::Print( ) { return mExpr->Print( ); } // Print()

/*********************************************************************
 **************************      Parser         *******************
 *********************************************************************/

typedef enum {
  LOWEST,
  EQUAL,
  LESSGREATER,
  SUM,
  PRODUCT,
  PREFIX,
  CALL
} BindingPower;

typedef enum {
  IDENTIFER_PARSER,
  NUMBER_PARSER,
  PREFIX_PARSER,
  INFIX_PARSER,
  GROUP_PARSER,
} Handler;

class Parser {
  Lexer *mLexer;
  Token *mCurToken;
  Token *mPeekToken;
  vector<string> mErrs;
  map<TokenType, Handler> mPrefixFNs;
  map<TokenType, Handler> mInfixFNs;
  map<TokenType, BindingPower> mPrecedences;

public:
  Parser( ) {
    Mapper( );
    mCurToken = NULL;
  } // Parser

  void Init( );

  // Helper function
  void Mapper( );
  void NextToken( );
  bool CurrentTokenIs( TokenType type );
  bool PeekTokenIs( TokenType type );
  bool ExpectPeek( TokenType type );
  BindingPower PeekBP( );
  BindingPower CurrentBP( );
  vector<string> Errors( );
  void PeekError( Token *tok );

  // Parsing stmt
  Statement *ParseExpressionStmt( );
  Statement *ParseStatement( );
  Statement *ParseAssignStmt( );
  Program *ParseProgram( );

  // Parse expression
  Expression *ParseGroup( );
  Expression *ParseNumber( );
  Expression *ParseIdentifier( );
  Expression *ParseExpression( BindingPower bp );
  Expression *ParseInfix( Expression *left );
  Expression *ParsePrefix( );

  // Map register
  Expression *PrefixExecutor( );
  Expression *InfixExecutor( Expression *left );
  void RegisterPrefix( TokenType tp, Handler fn );
  void RegisterInfix( TokenType tp, Handler fn );
};

void Parser::NextToken( ) {
  mCurToken = mPeekToken;
  mPeekToken = mLexer->ReadNextToken( );
} // NextToken()

void Parser::Init( ) {
  string str = "";
  getline( cin, str );
  mLexer = new Lexer( str );
  mPeekToken = mLexer->ReadNextToken( );
  while ( mPeekToken->type == EOFF ) {
    getline( cin, str );
    mLexer = new Lexer( str );
    mPeekToken = mLexer->ReadNextToken( );
  } // NextToken()
} // Init()

// Mapping the function with token type
void Parser::Mapper( ) {

  // Register the binding power of operation
  mPrecedences.insert( { EQ, EQUAL } );
  mPrecedences.insert( { NOT_EQ, EQUAL } );
  mPrecedences.insert( { LT, LESSGREATER } );
  mPrecedences.insert( { GT, LESSGREATER } );
  mPrecedences.insert( { PLUS, SUM } );
  mPrecedences.insert( { MINUS, SUM } );
  mPrecedences.insert( { MULTIPLY, PRODUCT } );
  mPrecedences.insert( { DIVIDE, PRODUCT } );

  // Mapping token type with their associated parser
  RegisterPrefix( IDENT, IDENTIFER_PARSER );
  RegisterPrefix( FLOAT, NUMBER_PARSER );
  RegisterPrefix( INT, NUMBER_PARSER );
  RegisterPrefix( MINUS, PREFIX_PARSER );
  RegisterPrefix( LPAREN, GROUP_PARSER );

  RegisterInfix( PLUS, INFIX_PARSER );
  RegisterInfix( MINUS, INFIX_PARSER );
  RegisterInfix( MULTIPLY, INFIX_PARSER );
  RegisterInfix( DIVIDE, INFIX_PARSER );
  RegisterInfix( LT, INFIX_PARSER );
  RegisterInfix( GT, INFIX_PARSER );
  RegisterInfix( EQ, INFIX_PARSER );
  RegisterInfix( NOT_EQ, INFIX_PARSER );

} // Mapper()

Expression *Parser::PrefixExecutor( ) {

  Expression *expr = NULL;
  Handler fn = mPrefixFNs[mCurToken->type];
  if ( fn == IDENTIFER_PARSER )
    expr = ParseIdentifier( );

  else if ( fn == NUMBER_PARSER )
    expr = ParseNumber( );

  else if ( fn == PREFIX_PARSER )
    expr = ParsePrefix( );

  else if ( fn == GROUP_PARSER )
    expr = ParseGroup( );

  return expr;
} // Executor()

Expression *Parser::InfixExecutor( Expression *left ) {
  Expression *expr = NULL;
  Handler fn = mInfixFNs[mCurToken->type];
  if ( fn == INFIX_PARSER )
    expr = ParseInfix( left );
  return expr;
} // Executor()

void Parser::RegisterPrefix( TokenType tp, Handler fn ) {
  mPrefixFNs[tp] = fn;
} // RegisterPrefix()

void Parser::RegisterInfix( TokenType tp, Handler fn ) {
  mInfixFNs[tp] = fn;
} // RegisterInfix()

BindingPower Parser::PeekBP( ) {
  if ( mPrecedences.find( mPeekToken->type ) != mPrecedences.end( ) )
    return mPrecedences[mPeekToken->type];

  return LOWEST;
} // PeekBP()

BindingPower Parser::CurrentBP( ) {
  if ( mPrecedences.find( mCurToken->type ) != mPrecedences.end( ) )
    return mPrecedences[mCurToken->type];

  return LOWEST;
} // CurrentBP()

void Parser::PeekError( Token *tok ) {
  string error = "Unexpected token :";
  error += tok->value;
  mErrs.push_back( error );
} // PeekError()

vector<string> Parser::Errors( ) { return mErrs; } // Errors()

bool Parser::CurrentTokenIs( TokenType tp ) {
  if ( tp == mCurToken->type )
    return true;
  return false;
} // CurrentTokenIs()

bool Parser::PeekTokenIs( TokenType tp ) {
  if ( tp == mPeekToken->type )
    return true;
  return false;
} // PeekTokenIs()

// Check if Peek is matching the expected one
// if correct move forward
bool Parser::ExpectPeek( TokenType tp ) {
  if ( PeekTokenIs( tp ) ) {
    NextToken( );
    return true;
  } // if

  else {
    PeekError( mPeekToken );
    return false;
  } // else
} // ExpectPeek()

Expression *Parser::ParseGroup( ) {
  if ( mPeekToken->type == EOFF )
    Init( );
  NextToken( );
  Expression *expr = ParseExpression( LOWEST );
  if ( !ExpectPeek( RPAREN ) )
    return NULL;

  return expr;
} // ParseGroup()

Expression *Parser::ParseNumber( ) {
  stringstream ss( mCurToken->value );
  if ( mCurToken->type == INT ) {
    size_t num;
    ss >> num;
    IntExpr *integer = new IntExpr( mCurToken, num );
    return integer;
  } // if

  float num;
  ss >> num;
  FloatExpr *fp = new FloatExpr( mCurToken, num );
  return fp;

} // ParseNumber()

Expression *Parser::ParseInfix( Expression *left ) {
  Token *op = mCurToken;
  BindingPower bp = CurrentBP( );
  if ( mPeekToken->type == EOFF )
    Init( );
  NextToken( );
  Expression *right = ParseExpression( bp );
  BinExpr *infix = new BinExpr( left, op, right );
  return infix;
} // ParseInfix()

Expression *Parser::ParsePrefix( ) {
  Token *op = mCurToken;
  if ( mPeekToken->type == EOFF )
    Init( );
  NextToken( );
  Expression *right = ParseExpression( PREFIX );
  UnaryExpression *prefix = new UnaryExpression( op, right );
  return prefix;
} // ParsePrefix()

Expression *Parser::ParseIdentifier( ) {
  SymbolExpression *id = new SymbolExpression( mCurToken, mCurToken->value );
  return id;
} // ParseIdentifier()

// 1
// +
// 2
// ;
Expression *Parser::ParseExpression( BindingPower bp ) {

  if ( mPrefixFNs.find( mCurToken->type ) == mPrefixFNs.end( ) ) {
    cout << "Unexpected token :" << mCurToken->value << endl;
    return NULL;
  } // if
  Expression *left = PrefixExecutor( );
  if ( PeekTokenIs( EOFF ) )
    Init( );

  while ( !PeekTokenIs( SEMICOLON ) && bp < PeekBP( ) ) {
    if ( mInfixFNs.find( mPeekToken->type ) == mInfixFNs.end( ) ) {
      cout << "Unexpected token :" << mPeekToken->value << endl;
      return left;
    } // if
    NextToken( );
    left = InfixExecutor( left );
  } // while

  return left;
} // ParseExpression()

Statement *Parser::ParseExpressionStmt( ) {
  Expression *expr = ParseExpression( LOWEST );
  ExpressionStatement *exprStmt = new ExpressionStatement( expr, mCurToken );
  if ( PeekTokenIs( SEMICOLON ) ) {
    NextToken( );
  } // if
  return exprStmt;
} // ParseExpressionStmt()

// a := 1 + 3 ; -> Assign statement
Statement *Parser::ParseAssignStmt( ) {
  SymbolExpression *var = new SymbolExpression( mCurToken, mCurToken->value );
  AssignmentStmt *astmt = NULL;

  if ( !ExpectPeek( ASSIGN ) )
    return NULL;

  else {
    NextToken( ); // Skip to expr
    Expression *right = ParseExpression( LOWEST );
    if ( right == NULL )
      return NULL;
    astmt = new AssignmentStmt( NULL, var, right );
  } // else

  if ( PeekTokenIs( SEMICOLON ) )
    NextToken( );
  return astmt;

} // ParseAssignStmt()

Statement *Parser::ParseStatement( ) {
  Statement *stmt = NULL;
  if ( CurrentTokenIs( IDENT ) && PeekTokenIs( ASSIGN ) )
    stmt = ParseAssignStmt( );
  else
    stmt = ParseExpressionStmt( );
  return stmt;
} // ParseStatement()

Program *Parser::ParseProgram( ) {
  Program *program = new Program( );
  string input = "";
  getline( cin, input );
  cout << "> ";
  Init( );
  NextToken( ); // setting current token to peek token
  while ( mCurToken->value != "quit" ) {
    Statement *stmt = ParseStatement( );

    if ( stmt != NULL ) {
      program->Append( stmt );
      cout << stmt->Print( ) << endl;
    } // if

    NextToken( );

    if ( mCurToken->type == EOFF ) {
      cout << "> ";
      Init( );
      NextToken( );
    } // if

  } // while
  return program;
} // Parser

/*********************************************************************
 * ************************main**************************************
 *********************************************************************/

int main( ) {
  Parser *parser = new Parser( );
  Program *program = parser->ParseProgram( );

} // main();
