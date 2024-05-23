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
    tok = SetNewToken( "(", RPAREN );
  else
    tok = SetNewToken( ")", LPAREN );
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

class NumberExpr : public Expression {
  size_t mValue;

public:
  NumberExpr( size_t value ) { mValue = value; } // NumberExpr
  json Print( );
  void Expr( );
};

json NumberExpr::Print( ) {
  return { { "Type", "NumberExpr" }, { "Value", mValue } };
} // Print()

void NumberExpr::Expr( ) { cout << mValue << endl; } // Expr()

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
  string mValue;

public:
  SymbolExpression( string val ) { mValue = val; } // SymbolExpression
  void Expr( ) {}                                  // Expr
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
class BlockStmt : public Statement {
  vector<Statement *> mBody;

public:
  void Stmt( );
  void Append( Statement *stmt );
  json Print( );
};

json BlockStmt::Print( ) {
  for ( auto e : mBody )
    cout << setw( 4 ) << e->Print( );
  return NULL;
} // Print()

void BlockStmt::Stmt( ) {}

void BlockStmt::Append( Statement *stmt ) {
  mBody.push_back( stmt );
} // Append()

class ExpressionStatement : public Statement {
  Expression *mExpr;

public:
  ExpressionStatement( Expression *expr ) { mExpr = expr; }
  void Stmt( ) {}
  json Print( );
};

json ExpressionStatement::Print( ) { return mExpr->Print( ); } // Print()

/*********************************************************************
 **************************      Parser         *******************
 *********************************************************************/

typedef enum {
  LOWEST,
  COMMA_BP,
  ASSIGNMENT_BP,
  LOGICAL_BP,
  RELATIONAL_BP,
  ADDITIVE_BP,
  MULTIPLICATIVE_BP,
  UNARY_BP,
  CALL_BP,
} BindingPower;

typedef enum {
  BINARY_HANDLER,
  PRIMARY_HANDLER,
  STATEMENT_HANDLER,
  GROUP_HANDLER,
  UNARY_HANDLER
} Handler;

class Parser {

  Lexer *mLexer;
  Token *mLookahead;
  vector<Token *> mTokens;
  size_t mPos;

  // Lookup table
  map<TokenType, Handler> mStmt_lu;
  map<TokenType, Handler> mNud_lu;
  map<TokenType, Handler> mLed_lu;
  map<TokenType, BindingPower> mBp_lu;

public:
  BlockStmt *Parse( );

  // Helper method
  void Init( );
  Token *NextToken( );
  bool HasMoreToken( );
  Token *ExpectError( TokenType expectType );

  // Pratt Parser
  Expression *HandlerFN( Handler fn, Expression *left, BindingPower bp );
  void Led( TokenType tp, BindingPower bp, Handler fn );
  void Nud( TokenType tp, Handler fn );
  void Stmt( TokenType tp, Handler fn );
  void CreateLUtable( );

  // Parsing fn
  Statement *ParseStmt( );
  Expression *ParseGroup( );
  Expression *ParseUnary( );
  Expression *ParseExpr( BindingPower bp );
  Expression *ParsePrimaryExpr( );
  Expression *ParseBinaryExpr( Expression *left, BindingPower bp );
};

Token *Parser::ExpectError( TokenType expectedType ) {
  if ( mLookahead->type != expectedType ) {
    cout << "Unexpected token : " << mLookahead->value << endl;
    return NULL;
  } // if

  return NextToken( );
} // ExpectError

void Parser::Init( ) {
  string str = "";
  getline( cin, str );
  mLexer = new Lexer( str );
  mLookahead = mLexer->ReadNextToken( );
  while ( mLookahead->type == EOFF ) {
    Init( );
  } // while
} // Init()

bool Parser::HasMoreToken( ) {
  if ( mLookahead->type != EOFF )
    return true;
  return false;
} // HasMoreToken

// Read the next token and return the current token
// When NextToken is invoked, which mean it is
// expecting a token, so if EOFF is reached
// scan new token
// 1 + EOFF
// 3 ;
Token *Parser::NextToken( ) {
  Token *tok = mLookahead;
  mLookahead = mLexer->ReadNextToken( );
  if ( mLookahead->type == EOFF ) {
    Init( );
  } // if
  mLookahead = mLexer->ReadNextToken( );
  return tok;
} // NextToken()

// 1 + 4 ; 4 + 6 ;
BlockStmt *Parser::Parse( ) {
  BlockStmt *block = new BlockStmt( );
  bool quit = false;
  string str = "";
  getline( cin, str );
  cout << "> ";

  Init( );
  CreateLUtable( );
  while ( mLookahead->value != "quit" ) {
    while ( mLookahead->type != EOFF && mLookahead->value != "quit" )
      block->Append( ParseStmt( ) );
    cout << "> ";
    Init( );
  } // while
  return block;
} // Parse

void Parser::Led( TokenType tp, BindingPower bp, Handler led_fn ) {
  mBp_lu[tp] = bp;
  mLed_lu[tp] = led_fn;
} // LED

void Parser::Nud( TokenType tp, Handler nud_fn ) {
  mNud_lu[tp] = nud_fn;
} // Nud

void Parser::Stmt( TokenType tp, Handler stmt_fn ) {
  mBp_lu[tp] = LOWEST;
  mStmt_lu[tp] = stmt_fn;
} // Stmt

void Parser::CreateLUtable( ) {

  // Logical
  // Group Expression

  // Relational
  Led( LT, RELATIONAL_BP, BINARY_HANDLER );
  Led( GT, RELATIONAL_BP, BINARY_HANDLER );
  Led( EQ, RELATIONAL_BP, BINARY_HANDLER );

  // Additive and multiplicative
  Led( PLUS, ADDITIVE_BP, BINARY_HANDLER );
  Led( MINUS, ADDITIVE_BP, BINARY_HANDLER );
  Led( MULTIPLY, MULTIPLICATIVE_BP, BINARY_HANDLER );
  Led( DIVIDE, MULTIPLICATIVE_BP, BINARY_HANDLER );

  // Literal and symbols
  Nud( INT, PRIMARY_HANDLER );
  Nud( FLOAT, PRIMARY_HANDLER );
  Nud( IDENT, PRIMARY_HANDLER );
  Nud( RPAREN, GROUP_HANDLER );
  Nud( MINUS, UNARY_HANDLER );
} // CreateLUtable

Expression *Parser::HandlerFN( Handler fn, Expression *left, BindingPower bp ) {
  Expression *res = NULL;
  if ( fn == PRIMARY_HANDLER )
    res = ParsePrimaryExpr( );
  else if ( fn == BINARY_HANDLER )
    res = ParseBinaryExpr( left, bp );
  else if ( fn == GROUP_HANDLER )
    res = ParseGroup( );
  else if ( fn == UNARY_HANDLER )
    res = ParseUnary( );

  return res;
} // HandlerFN()

Expression *Parser::ParseUnary( ) {
  Token *op = ExpectError( MINUS );
  Expression *rhs = ParseExpr( UNARY_BP );
  UnaryExpression *ue = new UnaryExpression( op, rhs );
  return ue;
} // ParseUnary()

Expression *Parser::ParseBinaryExpr( Expression *left, BindingPower bp ) {
  Token *op = NextToken( );
  Expression *right = ParseExpr( bp );
  BinExpr *bin = new BinExpr( left, op, right );
  return bin;
} // ParseBinaryExpr

Expression *Parser::ParsePrimaryExpr( ) {
  Token *currTok = mLookahead;
  Expression *expr = NULL;

  if ( currTok->type == INT || currTok->type == FLOAT ) {
    currTok = NextToken( );
    stringstream ss( currTok->value );
    size_t value = 0;
    ss >> value;
    expr = new NumberExpr( value );
  } // if

  else if ( currTok->type == IDENT ) {
    currTok = NextToken( );
    expr = new SymbolExpression( currTok->value );
  } // else if

  else if ( currTok->type == EOFF ) {
    Init( );
    return ParsePrimaryExpr( );
  } // else if

  return expr;
} // ParsePrimaryExp

Expression *Parser::ParseGroup( ) {
  ExpectError( RPAREN );
  Expression *expr = ParseExpr( LOWEST );
  ExpectError( LPAREN );
  return expr;
} // ParseGroup()

Expression *Parser::ParseExpr( BindingPower bp ) {
  // Parse NUD
  TokenType tp = mLookahead->type;
  Expression *left = NULL;
  Handler fn;
  if ( mNud_lu.find( tp ) != mNud_lu.end( ) ) {
    // Invoking the function for TokenType
    fn = mNud_lu[tp];
    left = HandlerFN( fn, left, bp );
    tp = mLookahead->type;
    while ( mBp_lu[tp] > bp ) {
      tp = mLookahead->type;
      if ( mLed_lu.find( tp ) != mLed_lu.end( ) ) {
        fn = mLed_lu[tp];
        bp = mBp_lu[tp];
        left = HandlerFN( fn, left, bp );
      } // if

    } // while

  } // if

  return left;
} // ParseExpr

Statement *Parser::ParseStmt( ) {

  Statement *stmt = NULL;
  /*
  if ( mStmt_lu.find( tp ) != mStmt_lu.end( ) ) {
    stmt = ( this->*mStmt_lu[tp] )( );
    return stmt;
  } // if
  */

  Expression *expr = ParseExpr( LOWEST );
  ExpectError( SEMICOLON );
  ExpressionStatement *exprStm = new ExpressionStatement( expr );
  return exprStm;
} // ParseStmt()

/*********************************************************************
 * ************************main**************************************
 *********************************************************************/

int main( ) {
  Parser *p = new Parser( );
  p->Parse( )->Print( );
  return 0;
}
