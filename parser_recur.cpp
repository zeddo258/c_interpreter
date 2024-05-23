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

/*********************************************************************
 **************************Interface for statement *******************
 *********************************************************************/
typedef enum {
  PROGRAM,
  NUMERIC_LITERAL,
  IDENTIFIER,
  BINARY_EXP,
  UNARY_EXP,
  NULL_LITERAL
} NodeType;

class Stmt {
  NodeType mType;

public:
  virtual json Print( ) = 0;
  virtual NodeType GetType( ) = 0;
  virtual Token *GetToken( ) = 0;
};

class Program {
  NodeType mType;
  vector<Stmt *> mBody;

public:
  Program( ) { mType = PROGRAM; } //

  void Append( Stmt *stmt ) { mBody.push_back( stmt ); } // append()

  vector<Stmt *> GetBody( ) { return mBody; } // getBody()

  void Print( ) {
    for ( auto e : mBody )
      cout << setw( 4 ) << e->Print( ) << endl;
  } // print()
};

/*********************************************************************
 **************************Interface for expression ******************
 *********************************************************************/
class Expr : public Stmt {};

class Binary_Expr : public Expr {
  NodeType mType;
  Expr *mLeft;
  Expr *mRight;
  Token *mOp;

public:
  Binary_Expr( Expr *left, Expr *right, Token *op ) {
    mType = BINARY_EXP;
    mLeft = left;
    mRight = right;
    mOp = op;
  } // Binary_Expr()

  NodeType GetType( ) { return mType; } // GetType()

  Expr *GetLeft( ) { return mLeft; } // GetLeft()

  Expr *GetRight( ) { return mRight; } // GetRight()

  Token *GetOp( ) { return mOp; } // GetOp()

  Token *GetToken( ) { return NULL; } // GetToken()

  json Print( ) {
    json js = { { "Type", "BINARY_EXP" },
                { "OP", mOp->value },
                { "Left", mLeft->Print( ) },
                { "Right", mRight->Print( ) } };
    return js;
  } // Print()
};

class Unary_Expr : public Expr {
  NodeType mType;
  Expr *mArg;
  Token *mOp;

public:
  Unary_Expr( Expr *arg, Token *op ) {
    mArg = arg;
    mOp = op;
    mType = UNARY_EXP;
  } // Unary_Expr()

  NodeType GetType( ) { return mType; } // getType

  Token *GetToken( ) { return NULL; } //

  Token *GetOp( ) { return mOp; } // getOp()

  Expr *GetArgs( ) { return mArg; } // get argument

  json Print( ) {
    json js = { { "Type ", "Unary" },
                { "OP ", mOp->value },
                { "Arg", mArg->Print( ) } };
    return js;
  } // Print()
};

class Identifier : public Expr {
  NodeType mType;
  Token *mToken;

public:
  Identifier( NodeType tp, Token *token ) {
    mType = tp;
    mToken = token;
  } // Identifier

  NodeType GetType( ) { return mType; } // getType

  Token *GetToken( ) { return mToken; } // getValue()

  json Print( ) {
    return { { "Type:", mType }, { "Value:", mToken->value } };
  } // print()
};

class NumericLiteral : public Expr {
  NodeType mType;
  Token *mToken;

public:
  NumericLiteral( NodeType tp, Token *token ) {
    mType = tp;
    mToken = token;
  } // Numeric

  NodeType GetType( ) { return mType; } // getType

  Token *GetToken( ) { return mToken; } // getValue()

  json Print( ) {
    return { { "Type", mType }, { "Value", mToken->value } };
  } // print()
};

/*********************************************************************
 **************************     Parser  ******************************
 *********************************************************************/
class Parser {
  string mStr;
  Lexer *mLexer;
  Token *mLookahead;

  Stmt *ParseStmt( );
  Expr *ParseExpr( );
  Expr *ParseCompareExpr( );
  Expr *ParseAdditiveExpr( );
  Expr *ParseMultiplyExpr( );
  Expr *ParsePrimaryExpr( );
  Expr *ParseUnaryExpr( );

public:
  void Init( );
  Program *Parse( string str );
  Token *Eat( TokenType tp );
};

// 1 =
// 1;
// 1 + 1
// < 3 + 4;
Expr *Parser::ParsePrimaryExpr( ) {
  Expr *expr = NULL;

  if ( mLookahead->type == EOFF ) {
    Init( );
  } // if

  if ( mLookahead->type == INT || mLookahead->type == FLOAT ) {
    expr = new NumericLiteral( NUMERIC_LITERAL, mLookahead );
    Eat( mLookahead->type );
  } // if

  else if ( mLookahead->type == IDENT ) {
    expr = new Identifier( IDENTIFIER, mLookahead );
    Eat( mLookahead->type );
  } // else if

  else if ( mLookahead->type == RPAREN ) {
    Eat( RPAREN );
    expr = ParseExpr( );
    Eat( LPAREN );
  } // else if

  else if ( mLookahead->type == MINUS )
    expr = ParseUnaryExpr( );

  else if ( mLookahead->type == ILLEGAL ) {
    // Skip
    cout << "Unrecognized token with first char : " << mLookahead->value
         << endl;
    mLookahead = NULL;
  } // else if

  return expr;
} // ParsePrimaryExpr()

Expr *Parser::ParseUnaryExpr( ) {
  Token *op = Eat( mLookahead->type );
  Expr *arg = ParsePrimaryExpr( );
  Expr *unary = new Unary_Expr( arg, op );
  return unary;
} // ParseUnaryExpr()

Expr *Parser::ParseMultiplyExpr( ) {

  Expr *left = ParsePrimaryExpr( );
  if ( mLookahead->type == EOFF ) {
    Init( );
  } // if
  while ( mLookahead->type == MULTIPLY || mLookahead->type == DIVIDE ) {
    Token *op = Eat( mLookahead->type );
    Expr *right = ParsePrimaryExpr( );
    left = new Binary_Expr( left, right, op );
  } // while
  return left;

} // ParseCompareExpression

// 4 + 5
// + 4
// ;
Expr *Parser::ParseAdditiveExpr( ) {

  Expr *left = ParseMultiplyExpr( ); // 1
  if ( mLookahead->type == EOFF ) {
    Init( );
  } // if
  while ( mLookahead->type == PLUS || mLookahead->type == MINUS ) {
    Token *op = Eat( mLookahead->type );
    Expr *right = ParseMultiplyExpr( );
    left = new Binary_Expr( left, right, op );
  } // while
  return left;

} // ParseCompareExpression

// 1 + 5 * 2 ;

Expr *Parser::ParseCompareExpr( ) {
  Expr *left = ParseAdditiveExpr( );
  if ( mLookahead->type == EOFF ) {
    Init( );
  } // if
  while ( mLookahead->type == EQ || mLookahead->type == LT ||
          mLookahead->type == GT ) {
    Token *op = Eat( mLookahead->type );
    Expr *right = ParseAdditiveExpr( );
    left = new Binary_Expr( left, right, op );
  } // while
  return left;
} // ParseCompareExpression

// 1 + 2 < 3 + 4;
Expr *Parser::ParseExpr( ) {
  Expr *expr = ParseCompareExpr( );
  return expr;
} // ParseExpression

Stmt *Parser::ParseStmt( ) {
  Expr *expr = ParseExpr( );
  return expr;
} //

void Parser::Init( ) {
  string str = "";
  getline( cin, str );
  mStr = str;
  mLexer = new Lexer( mStr );
  mLookahead = mLexer->ReadNextToken( );
  while ( mLookahead->type == EOFF ) {
    getline( cin, str );
    mStr = str;
    mLexer = new Lexer( mStr );
    mLookahead = mLexer->ReadNextToken( );
  } // while
} // Init()

// To do :
// Three case of EOFF reaching before finish
// 1. EOFF reached when there are more to parse:  1 +
// -> Implement this in parsePrimary: left = 1, op = +, right = EOFF.
// 2. EOFF reached when there are more to parse: 2 + 3 <enter> + 4;
// -> Implement this in ParseAdditive first to see how does it work
// 2. EOFF reached after finish parsing: 1 + 3 ;
Program *Parser::Parse( string str ) {
  Stmt *stmt = NULL;
  Program *pr = new Program( );
  bool quit = false;
  getline( cin, str );
  cout << "> ";
  Init( );
  // 1 + 2 ; quit
  while ( mLookahead->value != "quit" && !quit ) {

    while ( mLookahead->type != EOFF && !quit ) {
      stmt = ParseStmt( );
      Eat( SEMICOLON );
      if ( stmt != NULL )
        pr->Append( stmt );

      pr->Print( );
      // There are more token to be parse:
      // 1 + 4 ;
      // 5 + 7
      // ;
      if ( mLookahead->type != EOFF ) {
        stmt = ParseStmt( );
        Eat( SEMICOLON );
        pr->Append( stmt );
      } // if

      cout << "> ";
      Init( );
      if ( mLookahead->value == "quit" )
        quit = true;

    } // while

  } // while

  return pr;
} // Parse

Token *Parser::Eat( TokenType tp ) {
  Token *tok = mLookahead;

  if ( tok->type == EOFF ) {
    Init( );
  } // if

  if ( tok->type != tp ) {
    cout << "Unexpected token " << tok->value;
    tok = NULL;
    mLookahead = NULL;
  } // if

  else {
    mLookahead = mLexer->ReadNextToken( );
  } // else

  return tok;
} // Eat

int main( ) {
  Parser *p = new Parser( );
  string str = "";
  Program *pr = p->Parse( str );
  pr->Print( );
  return 0;
} // main()
