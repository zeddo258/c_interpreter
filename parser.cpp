# include <cctype>
# include <cmath>
# include <cstring>
# include <ctype.h>
# include <iostream>
# include <map>
# include <sstream>
# include <stdlib.h>
# include <string>
# include <vector>
# include <iomanip> 

using namespace std;

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
  LTEQ,
  GTEQ,
  EQ,
  NOT_EQ,
  NOT,

  // Keywords
  KEY_IF,
  KEY_ELSE,
  KEY_ELIF,
  KEY_INT,
  KEY_STRING,
  KEY_FLOAT,
  KEY_BOOL,
  KEY_VOID,
  KEY_CUSTOM_TYPE
} TokenType
;

struct Token {
  TokenType type;
  string value;
};

string GetLine() {
  string line;
  char ch; 
  while ( cin.get( ch ) && ch != '\n' )
    line += ch; 

  return line;
} // GetLine()

float GStringToFloat( string str ) {
  float result;
  stringstream ss( str ) ;
  ss >> result;
  return result;
} // GStringToFloat()

int GStringToInt( string str ) {
  int result;
  stringstream ss( str ) ;
  ss >> result;
  return result;
} // GStringToInt()

float GRound( float num ) {
  float result = round( num * 1000.0 ) / 1000.0;
  return result;
} // GRound()

bool GCompareFloats( float left_value, float right_value, float epsilon, TokenType type ) {

  bool result = false; 
  if ( type == LT ) 
    result = left_value < ( right_value - epsilon );
  else if ( type == GT )
    result = left_value > ( right_value + epsilon );
  else if ( type == EQ )
    result = fabs( left_value - right_value ) < epsilon;
  else if ( type == LTEQ ) {
    if ( left_value < right_value + epsilon || 
         fabs( left_value - right_value ) < epsilon )
      result = true; 
  } // else if
  else if ( type == GTEQ ) {
    if ( left_value > right_value + epsilon || 
         fabs( left_value - right_value ) < epsilon )
      result = true; 
  } // else if

  return result;
} // GCompareFloats()

bool GCompareInts( int left_value, int right_value, TokenType type ) {

  bool result = false; 
  if ( type == LT ) 
    result = left_value < right_value;
  else if ( type == GT )
    result = left_value > right_value;
  else if ( type == EQ )
    result = left_value == right_value;
  else if ( type == LTEQ ) 
    result = left_value <=  right_value;
  else if ( type == GTEQ ) 
    result = left_value >=  right_value;

  return result;
} // GCompareInts()

// --------------------------------------- Lexer ---------------------------
class Lexer {
  string mStr;
  size_t mCurrent;
  size_t mPeek;
  char mCh;

public:
  Lexer( string str ) {
    mStr = str;
    mPeek = 0;
    ReadChar( ) ;
  } // Lexer()

  void ReadChar( ) ;
  void AppendStr( string str ) ;
  bool HasMoreToken( ) ;
  void SkipComment( ) ;
  void SkipWhiteSpace( ) ;

  Token *ReadDivide( ) ;
  Token *ReadNot( ) ;
  Token *ReadNumber( ) ;
  Token *ReadID( ) ;
  Token *ReadAssign( ) ;
  Token *ReadPar( ) ;
  Token *ReadOperator( ) ;
  Token *ReadNextToken( ) ;
  Token *ReadBraces( ) ;
  Token *ReadCompare( ) ;
  char PeekChar( ) ;

  Token *SetNewToken( string value, TokenType type ) ;
};

void Lexer::AppendStr( string str ) { mStr += "\n" + str; } // AppendStr()

char Lexer::PeekChar( ) {
  if ( ! HasMoreToken( ) )
    return '\0';
  return mStr [ mPeek ];
} // Lexer::PeekChar()

// Read and advance
void Lexer::ReadChar( ) {
  if ( ! HasMoreToken( ) )
    mCh = '\0';
  else
    mCh = mStr [ mPeek ];
  mCurrent = mPeek;
  mPeek++;
} // Lexer::ReadChar()

// Helper function to check if there are more token to be cut
bool Lexer::HasMoreToken( ) {
  return mPeek < mStr.length( ) ;
} // Lexer::HasMoreToken()

Token *Lexer::ReadAssign( ) {
  Token *tok = NULL;
  if ( PeekChar( ) == '=' ) {
    tok = SetNewToken( ":=", ASSIGN ) ;
    ReadChar( ) ;
  } // if
  else
    tok = SetNewToken( ":", ILLEGAL ) ;
  return tok;
} // Lexer::ReadAssign()

Token *Lexer::ReadBraces( ) {
  Token *tok = NULL;
  if ( mCh == '{' )
    tok = SetNewToken( "{", LBRACE ) ;
  else if ( mCh == '}' )
    tok = SetNewToken( "}", RBRACE ) ;
  return tok;
} // Lexer::ReadBraces()

Token *Lexer::ReadNot( ) {
  Token *tok = NULL;
  if ( PeekChar( ) == '=' ) {
    tok = SetNewToken( "!=", NOT_EQ ) ;
    ReadChar( ) ;
  } // if
  else
    tok = SetNewToken( "!", NOT ) ;
  return tok;
} // Lexer::ReadNot()

Token *Lexer::ReadDivide( ) {
  Token *tok = NULL;
  if ( PeekChar( ) == '/' ) {
    SkipComment( ) ;
    tok = ReadNextToken( ) ;
  } // if
  else
    tok = SetNewToken( "/", DIVIDE ) ;
  return tok;
} // Lexer::ReadDivide()

Token *Lexer::ReadNumber( ) {
  Token *tok = NULL;
  string number = "";
  // if it start with an.
  // .122 .22122 .5125.2512 .2124.helloworld
  number += mCh;
  if ( mCh == '.' ) {
    ReadChar( ) ;
    while ( isdigit( mCh ) ) {
      number += mCh;
      ReadChar( ) ;
    } // while

    if ( number.length( ) == 1 )
      tok = SetNewToken( number, ILLEGAL ) ;
    else
      tok = SetNewToken( number, FLOAT ) ;
  } // if

  // 1.231 3.1415 2.19 2 23 1.
  else if ( isdigit( mCh ) ) {
    ReadChar( ) ;
    while ( isdigit( mCh ) || mCh == '.' ) {
      number += mCh;
      // if dot is encoutered
      if ( mCh == '.' ) {
        ReadChar( ) ;
        while ( isdigit( mCh ) ) {
          number += mCh;
          ReadChar( ) ;
        } // while

        tok = SetNewToken( number, FLOAT ) ;
        return tok;
      } // if

      ReadChar( ) ;
    } // while

    tok = SetNewToken( number, INT ) ;
  } // else if

  return tok;
} // Lexer::ReadNumber()

Token *Lexer::ReadID( ) {
  string id = "";
  Token *tok = NULL;
  while ( isalnum( mCh ) || mCh == '_' ) {
    id += mCh;
    ReadChar( ) ;
  } // while

  if ( id == "if" )
    tok = SetNewToken( id, KEY_IF ) ;
  else if ( id == "else" )
    tok = SetNewToken( id, KEY_ELSE ) ;
  else if ( id == "else if" )
    tok = SetNewToken( id, KEY_ELIF ) ;
  else
    tok = SetNewToken( id, IDENT ) ;
  return tok;
} // Lexer::ReadID()

Token *Lexer::SetNewToken( string val, TokenType type ) {
  Token *tok = new Token;
  tok->value = val;
  tok->type = type;
  return tok;
} // Lexer::SetNewToken()

// ReadOperator
Token *Lexer::ReadOperator( ) {
  Token *tok = new Token;
  string sign = "";
  sign += mCh;
  if ( mCh == '+' )
    tok = SetNewToken( sign, PLUS ) ;
  else if ( mCh == '-' )
    tok = SetNewToken( sign, MINUS ) ;
  else if ( mCh == '*' )
    tok = SetNewToken( sign, MULTIPLY ) ;
  else if ( mCh == '/' )
    tok = SetNewToken( sign, DIVIDE ) ;
  return tok;
} // Lexer::ReadOperator()

void Lexer::SkipWhiteSpace( ) {
  while ( isspace( mCh ) )
    ReadChar( ) ;
} // Lexer::SkipWhiteSpace()

void Lexer::SkipComment( ) {
  while ( HasMoreToken( ) && mCh != '\n' )
    ReadChar( ) ;
  ReadChar( ) ;
} // Lexer::SkipComment()

Token *Lexer::ReadPar( ) {
  Token *tok = NULL;
  if ( mCh == '(' )
    tok = SetNewToken( "(", LPAREN ) ;
  else
    tok = SetNewToken( ")", RPAREN ) ;
  return tok;
} // Lexer::ReadPar()

Token *Lexer::ReadCompare( ) {
  Token *tok = NULL;
  if ( mCh == '<' ) {
    
    if ( PeekChar() == '=' ) {
      tok = SetNewToken( "<=", LTEQ );
      ReadChar();
    } // if 

    else
      tok = SetNewToken( "<", LT ) ;
  } // if

  else if ( mCh == '>' ) {
    if ( PeekChar() == '=' ) {
      tok = SetNewToken( ">=", GTEQ );
      ReadChar();
    } // if 
    else
      tok = SetNewToken( ">", GT ) ;  
  } // else if

  return tok;
} // Lexer::ReadCompare()

// Get next token
Token *Lexer::ReadNextToken( ) {
  Token *tok;

  if ( isspace( mCh ) )
    SkipWhiteSpace( ) ;

  // Number encountered eg 1.23 .235 251,....
  if ( isdigit( mCh ) || mCh == '.' ) {
    tok = ReadNumber( ) ;
    return tok;
  } // if

  else if ( mCh == '\0' ) {
    tok = SetNewToken( "\0", EOFF ) ;
  } // else if

  // slicing id Hello_world, temp, flag,....
  // TODO:  slicing keyword,
  else if ( isalpha( mCh ) ) {
    tok = ReadID( ) ;
    return tok;
  } // else if

  else if ( mCh == ';' )
    tok = SetNewToken( ";", SEMICOLON ) ;

  else if ( mCh == '+' || mCh == '-' || mCh == '*' )
    tok = ReadOperator( ) ;

  else if ( mCh == '(' || mCh == ')' )
    tok = ReadPar( ) ;

  else if ( mCh == ':' )
    tok = ReadAssign( ) ;

  else if ( mCh == '=' )
    tok = SetNewToken( "=", EQ ) ;

  else if ( mCh == '{' || mCh == '}' )
    tok = ReadBraces( ) ;

  else if ( mCh == '<' || mCh == '>' )
    tok = ReadCompare(); 

  // Skip comment
  else if ( mCh == '/' )
    tok = ReadDivide( ) ;

  else {
    tok = SetNewToken( "\0", ILLEGAL ) ;
    cout << "Unrecognized token with first char : '" << mCh << "'" << endl;
  } // else

  ReadChar( ) ;
  return tok;
} // Lexer::ReadNextToken()

// --------------------------- Data type -----------------------------
class Obj {
public:
  virtual void Inspect( ) = 0;
  virtual string Type( ) = 0;
  virtual string Value( ) = 0;
};

class Integer : public Obj {
  string mType;
  int mValue;

public:
  Integer( size_t value, string tp ) {
    mType = tp;
    mValue = value;
  } // Integer()

  void Inspect( ) ;
  string Type( ) ;
  string Value( ) ;
};

string Integer::Type( ) { return mType; } // Integer::Type()

string Integer::Value( ) {
  stringstream ss;
  ss << mValue;
  string result = ss.str( ) ;
  return result;
} // Integer::Value()

void Integer::Inspect( ) { cout << mValue << endl; } // Integer::Inspect()

class Float : public Obj {
  string mType;
  float mValue;

public:
  Float( float value, string tp ) {
    mType = tp;
    mValue = value;
  } // Float()

  string Type( ) ;
  string Value( ) ;
  void Inspect( ) ;
};

string Float::Type( ) { return mType; } // Float::Type()

string Float::Value( ) {
  stringstream ss;
  ss << mValue;
  string res = ss.str( ) ;
  return res;
} // Float::Value()

void Float::Inspect( ) { 
  cout << fixed << setprecision( 3 ) << mValue << endl; 
} // Float::Inspect()

class Boolean : public Obj {
  string mType;
  bool mValue;

public:
  Boolean( bool value, string tp ) {
    mType = tp;
    mValue = value;
  } // Boolean()

  string Type( ) ;
  string Value( ) ;
  void Inspect( ) ;
};

string Boolean::Type( ) { return mType; } // Boolean::Type()

string Boolean::Value( ) {
  if ( mValue == true )
    return "true";
  return "false";
} // Boolean::Value()

void Boolean::Inspect( ) {
  if ( mValue == true )
    cout << "true" << endl;
  else
    cout << "false" << endl;
} // Boolean::Inspect()

class String : public Obj {
  string mType;
  string mValue;

public:
  String( string type, string value ) {
    mType = type;
    mValue = value;
  } // String()

  void Inspect( ) ;
  string Type( ) ;
  string Value( ) ;
};

string String::Type( ) { return mType; } // String::Type()

string String::Value( ) { return mValue; } // String::Value()

void String::Inspect( ) { cout << mValue << endl; } // String::Inspect()

class Null : public Obj {
  string mType;

public:
  Null( ) { mType = "NULL"; } // Null()

  void Inspect( ) ;
  string Type( ) ;
  string Value( ) ;
};

string Null::Type( ) { return mType; } // Null::Type()

string Null::Value( ) { return "Null"; } // Null::Value()

void Null::Inspect( ) { cout << "" << endl; } // Null::Inspect()

// ------------------Environment------------------------------------------
class Environment {
  map< string, Obj * > mVars;

public:
  Environment( ) {
  } // Environment()

  void Set( string var, Obj *data ) ;
  Obj *Get( string var ) ;
  bool VarExist( string var ) ;
};

bool Environment::VarExist( string var ) {
  if ( mVars.find( var ) != mVars.end( ) )
    return true;
  return false;
} // Environment::VarExist()

void Environment::Set( string var, Obj *data ) {
  if ( VarExist( var ) )
    mVars [ var ] = data;
  else
    mVars [ var ] = data;
} // Environment::Set()

Obj *Environment::Get( string var ) {
  Obj *obj = mVars [ var ];
  return obj;
} // Environment::Get()

// ------------------------------- Node ---------------------------

class Node {
  string mType;

public:
  virtual void Print( ) = 0;
  virtual string Type( ) = 0;
  virtual string Value( ) = 0;
  virtual Obj *Eval( Environment *env ) = 0;
};

// ---------------------------- AST node type -------------------
class Statement : public Node {
public:
  virtual void Stmt( ) = 0;
}; // Statement

class Expression : public Node {
public:
  virtual void Expr( ) = 0;
};

// 1 + 2 ; 2 + 4 ;
// --------------------------- Expression -----------------------
class BlockStatement : public Statement {
  Token *mTok; // { token
  string mType;
  vector< Statement * > mStmts;

public:
  BlockStatement( Token *token ) {
    mType = "Block Statements";
    mTok = token;
  } // BlockStatement()
  
  void Append( Statement *stmt ) ;

  void Stmt( ) {
  } // Stmt()

  void Print( ) ;
  string Type( ) ;
  string Value( ) ;
  Obj *Eval( Environment *env ) ;
};

void BlockStatement::Append( Statement *stmt ) {

  mStmts.push_back( stmt ) ;

} // BlockStatement::Append()

void BlockStatement::Print( ) {

  for ( int i = 0; i < mStmts.size( ) ; ++i ) {
    mStmts [ i ]->Print( ) ;
  } // for

} // BlockStatement::Print()

string BlockStatement::Type( ) { return mType; } // BlockStatement::Type()

Obj *BlockStatement::Eval( Environment *env ) { return NULL; } // BlockStatement::Eval()

string BlockStatement::Value( ) { return mTok->value; } // BlockStatement::Value()

// If-else structure
class ConditionalExpr : public Expression {
  Token *mTok; // If token
  string mType;
  Expression *mCondition;
  BlockStatement *mConsequence;
  BlockStatement *mAlternative;

public:
  ConditionalExpr( Token *tok, Expression *cond, BlockStatement *consequence,
                   BlockStatement *alternative ) {
    mTok = tok;
    mType = "Conditional Expression";
    mCondition = cond;
    mConsequence = consequence;
    mAlternative = alternative;
  } // ConditionalExpr()

  void Expr( ) {
  } // Expr() 

  void Print( ) ;
  string Type( ) ;
  string Value( ) ;
  Obj *Eval( Environment *env ) ;
};

void ConditionalExpr::Print( ) {
  cout << " If ";
  mCondition->Print( ) ;
  mConsequence->Print( ) ;
  if ( mAlternative != NULL ) {
    cout << "Else", mConsequence->Print( ) ;
  } // if

} // ConditionalExpr::Print()

string ConditionalExpr::Type( ) { return mType; } // ConditionalExpr::Type()

string ConditionalExpr::Value( ) { return mTok->value; } // ConditionalExpr::Value()

// TODO: Implement evaluation
Obj *ConditionalExpr::Eval( Environment *env ) { return NULL; } // ConditionalExpr::Eval()

class IntExpr : public Expression {
  Token *mTok;
  string mType;
  size_t mValue;

public:
  IntExpr( Token *tok, size_t value ) {
    mValue = value;
    mTok = tok;
    mType = "Integer";
  } // IntExpr()

  string Type( ) ;
  void Print( ) ;
  void Expr( ) ;
  string Value( ) ;
  Obj *Eval( Environment *env ) ;
};

string IntExpr::Type( ) { return mType; } // IntExpr::Type()

Obj *IntExpr::Eval( Environment *env ) {
  Obj *result = new Integer( mValue, mType ) ;
  return result;
} // IntExpr::Eval()

string IntExpr::Value( ) {
  stringstream ss;
  ss << mValue;
  string str = ss.str( ) ;
  return str;
} // IntExpr::Value()

void IntExpr::Print( ) { cout << mTok->value; } // IntExpr::Print()

void IntExpr::Expr( ) { cout << mValue << endl; } // IntExpr::Expr()

class FloatExpr : public Expression {
  Token *mTok;
  float mValue;
  string mType;

public:
  FloatExpr( Token *tok, float value ) {
    mValue = value;
    mTok = tok;
    mType = "Float";
  } // FloatExpr()

  string Type( ) { 
    return mType; 
  } // Type()

  void Print( ) ;
  void Expr( ) ;
  string Value( ) ;
  Obj *Eval( Environment *env ) ;
};

Obj *FloatExpr::Eval( Environment *env ) {
  Obj *result = new Float( mValue, mType ) ;
  return result;
} // FloatExpr::Eval()

string FloatExpr::Value( ) {
  stringstream ss;
  ss << mValue;
  string str = ss.str( ) ;
  return str;
} // FloatExpr::Value()

void FloatExpr::Print( ) { 
  cout << mTok->value; 
} // FloatExpr::Print()

void FloatExpr::Expr( ) { 
  cout << mValue << endl; 
} // FloatExpr::Expr()

class StringExpr : public Expression {
  string mValue;
  string mType;

public:
  StringExpr( string value ) {
    mValue = value;
    mType = "String";
  } // StringExpr()

  string Type( ) { 
    return mType; 
  } // Type()

  void Expr( ) ;
  void Print( ) ;
  string Value( ) ;
  Obj *Eval( Environment *env ) ;
};

Obj *StringExpr::Eval( Environment *env ) {
  Obj *result = new String( mType, mValue ) ;
  return result;
} // StringExpr::Eval()

string StringExpr::Value( ) { 
  return mValue; 
} // StringExpr::Value()

void StringExpr::Print( ) { 
  cout << mValue; 
} // StringExpr::Print()

void StringExpr::Expr( ) { 
} // StringExpr::Expr()

class BinExpr : public Expression {
  Expression *mLeft;
  Token *mOp;
  Expression *mRight;
  string mType;

public:
  BinExpr( Expression *left, Token *op, Expression *right ) {
    mLeft = left;
    mOp = op;
    mRight = right;
    mType = "Binary Expression";
  } // BinExpr()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return ""; 
  } // Value()

  void Expr( ) ;
  void Print( ) ;

  Obj *Eval( Environment *env ) ;
  Obj *EvalCompare( Obj *left, Obj *right ) ;
  Obj *EvalArithmatic( Obj *left, Obj *right ) ;

};

Obj *BinExpr::EvalArithmatic( Obj *left, Obj *right ) {
  Obj *res = NULL;
  string leftType = left->Type( ) ;
  string rightType = right->Type( ) ;
  string rightValue = right->Value( ) ;
  string leftValue = left->Value( ) ;

  if ( leftType == "Float" || rightType == "Float" ) {
    float left_value = GStringToFloat( leftValue ) ;
    float right_value = GStringToFloat( rightValue ) ;
    float result;
    if ( mOp->type == PLUS )
      result = left_value + right_value;

    else if ( mOp->type == MINUS )
      result = left_value - right_value;

    else if ( mOp->type == DIVIDE )
      result = left_value / right_value;

    else if ( mOp->type == MULTIPLY )
      result = left_value * right_value;

    res = new Float( result, "Float" ) ;
  } // if

  else {
    int left_value = GStringToInt( leftValue ) ;
    int right_value = GStringToInt( rightValue ) ;
    int result;
    if ( mOp->type == PLUS )
      result = left_value + right_value;

    else if ( mOp->type == MINUS )
      result = left_value - right_value;

    else if ( mOp->type == DIVIDE )
      result = left_value / right_value;

    else if ( mOp->type == MULTIPLY )
      result = left_value * right_value;

    res = new Integer( result, "Integer" ) ;
  } // else

  return res;
} // BinExpr::EvalArithmatic()

Obj *BinExpr::EvalCompare( Obj *left, Obj *right ) {
  Obj *res = NULL;
  string leftType = left->Type( ) ;
  string rightType = right->Type( ) ;
  string rightValue = right->Value( ) ;
  string leftValue = left->Value( ) ;
  bool result = true;
  float epsilon = 0.0001;

  if ( leftType == "Float" || rightType == "Float" ) {
    float left_value = GStringToFloat( leftValue ) ;
    float right_value = GStringToFloat( rightValue ) ;  
    result = GCompareFloats( left_value, right_value, epsilon, mOp -> type ); 
  } // if

  else {
    int left_value = GStringToInt( leftValue ) ;
    int right_value = GStringToInt( rightValue ) ;
    result = GCompareInts( left_value, right_value, mOp -> type );
  } // else

  res = new Boolean( result, "Boolean" ) ;
  return res;
} // BinExpr::EvalCompare() 

Obj *BinExpr::Eval( Environment *env ) {
  Obj *result = NULL;
  Obj *left = mLeft->Eval( env ) ;
  Obj *right = mRight->Eval( env ) ;
  if ( left == NULL || right == NULL )
    return NULL;

  string leftType = left->Type( ) ;
  string rightType = right->Type( ) ;

  if ( mOp->type == PLUS || mOp->type == MINUS || mOp->type == DIVIDE ||
       mOp->type == MULTIPLY )
    result = EvalArithmatic( left, right ) ;
  else if ( mOp->type == LT || mOp->type == GT || 
            mOp->type == EQ || mOp->type == GTEQ || 
            mOp -> type == LTEQ )
    result = EvalCompare( left, right ) ;

  return result;
} // BinExpr::Eval()

void BinExpr::Print( ) {
  cout << "(";
  mLeft->Print( ) ;
  cout << mOp->value;
  mRight->Print( ) ;
  cout << ")";
} // BinExpr::Print()

void BinExpr::Expr( ) { } // BinExpr::Expr()

class SymbolExpression : public Expression {
  Token *mTok;
  string mValue;
  string mType;

public:
  SymbolExpression( Token *tok, string val ) {
    mValue = val;
    mTok = tok;
    mType = "Symbol Expression";
  } // SymbolExpression()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return mValue; 
  } // Value()

  void Expr( ) { 
  } // Expr()

  void Print( ) ;
  
  Obj *Eval( Environment *env ) ;
};

Obj *SymbolExpression::Eval( Environment *env ) {
  Obj *var = NULL;

  if ( env->VarExist( mValue ) )
    var = env->Get( mValue ) ;

  else
    cout << "Undefined identifier : " << mValue << endl;

  return var;
} // SymbolExpression::Eval()

void SymbolExpression::Print( ) { 
  cout << mValue; 
} // SymbolExpression::Print()

class UnaryExpression : public Expression {
  Token *mOp;
  Expression *mRhs;
  string mType;

public:
  UnaryExpression( Token *op, Expression *right ) {
    mOp = op;
    mRhs = right;
    mType = "Unary expression";
  } // UnaryExpression()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return ""; 
  } // Value()

  void Expr( ) { 
  } // Expr()

  void Print( ) ;
  
  Obj *Eval( Environment *env ) ;
  Obj *EvalPlusMinus( Environment *env ) ;
};

Obj *UnaryExpression::EvalPlusMinus( Environment *env ) {
  Obj *rhs = mRhs->Eval( env ) ;
  if ( rhs == NULL )
    return NULL;

  Obj *result = NULL;
  string value = rhs->Value( ) ;
  if ( mOp->type == PLUS ) {
    if ( rhs->Type( ) == "Float" ) {
      float num = GStringToFloat( value ) ;
      result = new Float( num, "Float" ) ;
    } // if

    else if ( rhs->Type( ) == "Integer" ) {
      int num = GStringToInt( value ) ;
      result = new Integer( num, "Integer" ) ;
    } // else if

  } // if

  else if ( mOp->type == MINUS ) {
    if ( rhs->Type( ) == "Float" ) {
      float num = GStringToFloat( value ) ;
      num = num * -1.0;
      result = new Float( num, "Float" ) ;
    } // if

    else if ( rhs->Type( ) == "Integer" ) {
      int num = GStringToInt( value ) ;
      num = num * -1;
      result = new Integer( num, "Integer" ) ;
    } // else if

  } // else if

  return result;
} // UnaryExpression::EvalPlusMinus()

Obj *UnaryExpression::Eval( Environment *env ) {
  Obj *result = NULL;
  if ( mOp->type == PLUS || mOp->type == MINUS ) {
    result = EvalPlusMinus( env ) ;
  } // if

  return result;
} // UnaryExpression::Eval()

void UnaryExpression::Print( ) {
  cout << "( ";
  cout << mOp->value;
  mRhs->Print( ) ;
  cout << ") ";
} // UnaryExpression::Print()

// ------------------------------- Statements --------------------------
class AssignmentStmt : public Statement {
  Token *mToken;           // The type token
  SymbolExpression *mName; // Variable name
  Expression *mValue;
  string mType;

public:
  AssignmentStmt( Token *token, SymbolExpression *name, Expression *value ) {
    mToken = token;
    mName = name;
    mValue = value;
    mType = "Assignment Expression";
  } // AssignmentStmt()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return ""; 
  } // Value()

  void Stmt( ) {
  } // Stmt()

  void Print( ) ;
  
  Obj *Eval( Environment *env ) ;
};

Obj *AssignmentStmt::Eval( Environment *env ) {
  Obj *rhs = mValue->Eval( env ) ;
  if ( rhs == NULL )
    return NULL;
  string varName = mName->Value( ) ;
  env->Set( varName, rhs ) ;
  return rhs;
} // AssignmentStmt::Eval()

void AssignmentStmt::Print( ) {
  mName->Print( ) ;
  cout << ":=";
  mValue->Print( ) ;
  cout << ";\n";
} // AssignmentStmt::Print()


class Program : public Node {

public:
  vector< Statement * > mBody;

  string Type( ) { 
    return "Program"; 
  } // Type() 

  string Value( ) { 
    return ""; 
  } // Value()

  void Append( Statement *stmt ) ;
  void Print( ) ;

  Statement *Pop( ) ;
  Obj *Eval( Environment *env ) ;
};

Obj *Program::Eval( Environment *env ) {
  Obj *obj = NULL;
  Statement *stmt = NULL;
  while ( mBody.size( ) > 0 ) {
    stmt = Pop( ) ;
    obj = stmt->Eval( env ) ;
    if ( obj != NULL )
      obj->Inspect( ) ;
    else
      return NULL;
  } // while

  return obj;
} // Program::Eval()

Statement *Program::Pop( ) {
  Statement *stmt = NULL;
  if ( ! mBody.empty( ) ) {
    stmt = mBody.front( ) ;
    mBody.erase( mBody.begin( ) ) ;
  } // if

  return stmt;
} // Program::Pop()

void Program::Print( ) {
  for ( int i = 0; i < mBody.size( ) ; ++i )
    mBody [ i ]->Print( ) ;
} // Program::Print()

void Program::Append( Statement *stmt ) { 
  mBody.push_back( stmt ) ; 
} // Program::Append()

class ExpressionStatement : public Statement {
  Token *mToken;
  Expression *mExpr;
  string mType;

public:
  ExpressionStatement( Expression *expr, Token *token ) {
    mExpr = expr;
    mToken = token;
    mType = "Expression Statement";
  } // ExpressionStatement()

  Expression *Expr( ) { 
    return mExpr; 
  } // Expr()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return ""; 
  } // Value()

  void Stmt( ) {
  } // Stmt()

  void Print( ) ;
  
  Obj *Eval( Environment *env ) ;
};

Obj *ExpressionStatement::Eval( Environment *env ) {
  Obj *obj = mExpr->Eval( env ) ;
  return obj;
} // ExpressionStatement::Eval()

void ExpressionStatement::Print( ) {
  mExpr->Print( ) ;
  cout << ";\n";
} // ExpressionStatement::Print()

class NullStatement : public Statement {
  string mType;

public:
  NullStatement( ) { 
    mType = "Null Statement"; 
  } // NullStatement()

  string Value( ) { 
    return ""; 
  } // Value()

  void Stmt( ) {
  } // Stmt()

  string Type( ) ;
  void Print( ) ;
  Obj *Eval( Environment *env ) ;
};

Obj *NullStatement::Eval( Environment *env ) {
  Obj *obj = new Null( ) ;
  return obj;
} // NullStatement::Eval()

string NullStatement::Type( ) { return mType; } // NullStatement::Type()

void NullStatement::Print( ) { cout << ";\n"; } // NullStatement::Print()

// -------------------------- Parser ------------------------

typedef enum {
  LOWEST,
  EQUAL,
  LESSGREATER,
  SUM,
  PRODUCT,
  PREFIX,
  CALL
} BindingPower
;

typedef enum {
  IDENTIFER_PARSER,
  NUMBER_PARSER,
  PREFIX_PARSER,
  INFIX_PARSER,
  GROUP_PARSER,
  IF_PARSER
} Handler
;

class Parser {
  Lexer *mLexer;
  Token *mCurToken;
  Token *mPeekToken;
  vector< string > mErrs;
  map< TokenType, Handler > mPrefixFNs;
  map< TokenType, Handler > mInfixFNs;
  map< TokenType, BindingPower > mPrecedences;

public:
  Parser( ) {
    Mapper( ) ;
    mCurToken = NULL;
  } // Parser()

  void Init( ) ;
  void Reset( ) ;
  // Helper function
  void Mapper( ) ;
  void NextToken( ) ;
  bool CurrentTokenIs( TokenType type ) ;
  bool PeekTokenIs( TokenType type ) ;
  bool ExpectPeek( TokenType type ) ;
  BindingPower PeekBP( ) ;
  BindingPower CurrentBP( ) ;
  vector< string > Errors( ) ;
  void PeekError( Token *tok ) ;

  // Parsing stmt
  Statement *ParseExpressionStmt( ) ;
  Statement *ParseStatement( ) ;
  Statement *ParseAssignStmt( ) ;
  Program *ParseProgram( ) ;

  // Parse expression
  BlockStatement *ParseBlockStatement( ) ;
  Expression *ParseIf( ) ;
  Expression *ParseGroup( ) ;
  Expression *ParseNumber( ) ;
  Expression *ParseIdentifier( ) ;
  Expression *ParseExpression( BindingPower bp ) ;
  Expression *ParseInfix( Expression *left ) ;
  Expression *ParsePrefix( ) ;

  // Map register
  Expression *PrefixExecutor( ) ;
  Expression *InfixExecutor( Expression *left ) ;
  void RegisterPrefix( TokenType tp, Handler fn ) ;
  void RegisterInfix( TokenType tp, Handler fn ) ;
};

void Parser::NextToken( ) {
  mCurToken = mPeekToken;
  mPeekToken = mLexer->ReadNextToken( ) ;
} // Parser::NextToken()

void Parser::Reset( ) { 
  mCurToken->value = '\0'; 
  mCurToken->type = EOFF; 
} // Parser::Reset()

void Parser::Init( ) {
  string str = GetLine(); 
  mLexer = new Lexer( str ) ;
  mPeekToken = mLexer->ReadNextToken( ) ;
  while ( mPeekToken->type == EOFF ) {
    str = GetLine(); 
    mLexer = new Lexer( str ) ;
    mPeekToken = mLexer->ReadNextToken( ) ;
  } // while

} // Parser::Init()

// Mapping the function with token type
void Parser::Mapper( ) {

  // Register the binding power of operation
  mPrecedences [ EQ ] = EQUAL;
  mPrecedences [ EQ ] = EQUAL;
  mPrecedences [ NOT_EQ ] = EQUAL;
  mPrecedences [ LT ] = LESSGREATER;
  mPrecedences [ GT ] = LESSGREATER;
  mPrecedences [ GTEQ ] = LESSGREATER;
  mPrecedences [ LTEQ ] = LESSGREATER;
  mPrecedences [ PLUS ] = SUM;
  mPrecedences [ MINUS ] = SUM;
  mPrecedences [ MULTIPLY ] = PRODUCT;
  mPrecedences [ DIVIDE ] = PRODUCT;

  // Mapping token type with their associated parser
  RegisterPrefix( IDENT, IDENTIFER_PARSER ) ;
  RegisterPrefix( FLOAT, NUMBER_PARSER ) ;
  RegisterPrefix( INT, NUMBER_PARSER ) ;
  RegisterPrefix( MINUS, PREFIX_PARSER ) ;
  RegisterPrefix( PLUS, PREFIX_PARSER ) ;
  RegisterPrefix( KEY_IF, IF_PARSER ) ;
  RegisterPrefix( LPAREN, GROUP_PARSER ) ;

  RegisterInfix( PLUS, INFIX_PARSER ) ;
  RegisterInfix( MINUS, INFIX_PARSER ) ;
  RegisterInfix( MULTIPLY, INFIX_PARSER ) ;
  RegisterInfix( DIVIDE, INFIX_PARSER ) ;
  RegisterInfix( LT, INFIX_PARSER ) ;
  RegisterInfix( LTEQ, INFIX_PARSER ) ;
  RegisterInfix( GT, INFIX_PARSER ) ;
  RegisterInfix( GTEQ, INFIX_PARSER ) ;
  RegisterInfix( EQ, INFIX_PARSER ) ;
  RegisterInfix( NOT_EQ, INFIX_PARSER ) ;

} // Parser::Mapper()

Expression *Parser::PrefixExecutor( ) {

  Expression *expr = NULL;
  Handler fn = mPrefixFNs [ mCurToken->type ];
  if ( fn == IDENTIFER_PARSER )
    expr = ParseIdentifier( ) ;

  else if ( fn == NUMBER_PARSER )
    expr = ParseNumber( ) ;

  else if ( fn == PREFIX_PARSER )
    expr = ParsePrefix( ) ;

  else if ( fn == GROUP_PARSER )
    expr = ParseGroup( ) ;

  else if ( fn == IF_PARSER )
    expr = ParseIf( ) ;

  return expr;
} // Parser::PrefixExecutor()

Expression *Parser::InfixExecutor( Expression *left ) {
  Expression *expr = NULL;
  Handler fn = mInfixFNs [ mCurToken->type ];
  if ( fn == INFIX_PARSER )
    expr = ParseInfix( left ) ;
  return expr;
} // Parser::InfixExecutor()

void Parser::RegisterPrefix( TokenType tp, Handler fn ) {
  mPrefixFNs [ tp ] = fn;
} // Parser::RegisterPrefix()

void Parser::RegisterInfix( TokenType tp, Handler fn ) {
  mInfixFNs [ tp ] = fn;
} // Parser::RegisterInfix()

BindingPower Parser::PeekBP( ) {
  if ( mPrecedences.find( mPeekToken->type ) != mPrecedences.end( ) )
    return mPrecedences [ mPeekToken->type ];

  return LOWEST;
} // Parser::PeekBP()

BindingPower Parser::CurrentBP( ) {
  if ( mPrecedences.find( mCurToken->type ) != mPrecedences.end( ) )
    return mPrecedences [ mCurToken->type ];

  return LOWEST;
} // Parser::CurrentBP()

void Parser::PeekError( Token *tok ) {
  string error = "Unexpected token : ";
  error += "'" + tok->value + "'" ;
  mErrs.push_back( error ) ;
} // Parser::PeekError()

vector< string > Parser::Errors( ) { return mErrs; } // Errors()

bool Parser::CurrentTokenIs( TokenType tp ) {
  if ( tp == mCurToken->type )
    return true;
  return false;
} // Parser::CurrentTokenIs()

bool Parser::PeekTokenIs( TokenType tp ) {
  if ( tp == mPeekToken->type )
    return true;
  return false;
} // Parser::PeekTokenIs()

// Check if Peek is matching the expected one
// if correct move forward
bool Parser::ExpectPeek( TokenType tp ) {
  if ( PeekTokenIs( tp ) ) {
    NextToken( ) ;
    return true;
  } // if

  else {
    PeekError( mPeekToken ) ;
    return false;
  } // else

} // Parser::ExpectPeek()

BlockStatement *Parser::ParseBlockStatement( ) {
  BlockStatement *block = new BlockStatement( mCurToken ) ;
  if ( PeekTokenIs( EOFF ) )
    Init( ) ;

  NextToken( ) ;
  while ( ! CurrentTokenIs( RBRACE ) && ! CurrentTokenIs( EOFF ) ) {
    Statement *stmt = ParseStatement( ) ;
    if ( stmt != NULL )
      block->Append( stmt ) ;
    NextToken( ) ;
  } // while

  return block;
} // Parser::ParseBlockStatement()

Expression *Parser::ParseIf( ) {
  Expression *condition = NULL;
  BlockStatement *consequence = NULL;
  BlockStatement *alternative = NULL;
  Expression *expr = NULL;

  if ( ! ExpectPeek( LPAREN ) )
    return NULL;

  NextToken( ) ;
  condition = ParseExpression( LOWEST ) ;

  // Expecting ) afterward
  if ( ! ExpectPeek( RPAREN ) )
    return NULL;

  // Expecting { afterward
  if ( ! ExpectPeek( LBRACE ) )
    return NULL;

  consequence = ParseBlockStatement( ) ;

  if ( PeekTokenIs( KEY_ELSE ) ) {
    NextToken( ) ;
    if ( ! ExpectPeek( LBRACE ) )
      return NULL;

    alternative = ParseBlockStatement( ) ;
  } // if

  expr = new ConditionalExpr( mCurToken, condition, consequence, alternative ) ;
  return expr;
} // Parser::ParseIf()

Expression *Parser::ParseGroup( ) {
  if ( mPeekToken->type == EOFF )
    Init( ) ;
  NextToken( ) ;
  Expression *expr = ParseExpression( LOWEST ) ;
  if ( ! ExpectPeek( RPAREN ) )
    return NULL;

  return expr;
} // Parser::ParseGroup()

Expression *Parser::ParseNumber( ) {
  stringstream ss( mCurToken->value ) ;
  if ( mCurToken->type == INT ) {
    size_t num;
    ss >> num;
    IntExpr *integer = new IntExpr( mCurToken, num ) ;
    return integer;
  } // if

  float num = GStringToFloat( mCurToken->value );
  FloatExpr *fp = new FloatExpr( mCurToken, num ) ;
  return fp;

} // Parser::ParseNumber()

Expression *Parser::ParseInfix( Expression *left ) {
  Token *op = mCurToken;

  BindingPower bp = CurrentBP( ) ;
  if ( mPeekToken->type == EOFF )
    Init( ) ;
  NextToken( ) ;
  Expression *right = ParseExpression( bp ) ;
  if ( right == NULL )
    return NULL;

  BinExpr *infix = new BinExpr( left, op, right ) ;
  return infix;
} // Parser::ParseInfix()

Expression *Parser::ParsePrefix( ) {
  Token *op = mCurToken;
  if ( mPeekToken->type == EOFF )
    Init( ) ;

  NextToken( ) ;
  Expression *right = ParseExpression( PREFIX ) ;
  if ( right == NULL )
    return NULL;

  UnaryExpression *prefix = new UnaryExpression( op, right ) ;
  return prefix;
} // Parser::ParsePrefix()

Expression *Parser::ParseIdentifier( ) {
  SymbolExpression *id = new SymbolExpression( mCurToken, mCurToken->value ) ;
  return id;
} // Parser::ParseIdentifier()

Expression *Parser::ParseExpression( BindingPower bp ) {

  if ( mCurToken->type == ILLEGAL )
    return NULL;

  if ( mPrefixFNs.find( mCurToken->type ) == mPrefixFNs.end( ) ) {
    cout << "Unexpected token : '" << mCurToken->value << "'" << endl;
    return NULL;
  } // if

  Expression *left = PrefixExecutor( ) ;
  if ( PeekTokenIs( EOFF ) )
    Init( ) ;

  if ( mPeekToken->type == ILLEGAL )
    return NULL;

  while ( ! PeekTokenIs( SEMICOLON ) && bp < PeekBP( ) ) {
    if ( mInfixFNs.find( mPeekToken->type ) == mInfixFNs.end( ) ) {
      cout << "Unexpected token : '" << mPeekToken->value << "'" << endl;
      return NULL;
    } // if

    NextToken( ) ;
    if ( mPeekToken->type == ILLEGAL )
      return NULL;

    left = InfixExecutor( left ) ;
  } // while

  return left;
} // Parser::ParseExpression()

Statement *Parser::ParseExpressionStmt( ) {
  ExpressionStatement *exprStmt = NULL;
  Expression *expr = ParseExpression( LOWEST ) ;

  if ( expr == NULL ) 
    return NULL;
  else 
    exprStmt = new ExpressionStatement( expr, mCurToken ) ;

  if ( PeekTokenIs( SEMICOLON ) ) {
    NextToken( ) ;
  } // if

  return exprStmt;
} // Parser::ParseExpressionStmt()

// a := 1 + 3 ; -> Assign statement
Statement *Parser::ParseAssignStmt( ) {
  SymbolExpression *var = new SymbolExpression( mCurToken, mCurToken->value ) ;
  AssignmentStmt *astmt = NULL;

  if ( ! ExpectPeek( ASSIGN ) )
    return NULL;

  else {
    NextToken( ) ; // Skip to expr
    Expression *right = ParseExpression( LOWEST ) ;
    if ( right == NULL )
      return NULL;
    astmt = new AssignmentStmt( NULL, var, right ) ;
  } // else

  if ( PeekTokenIs( SEMICOLON ) )
    NextToken( ) ;
  return astmt;

} // Parser::ParseAssignStmt()

Statement *Parser::ParseStatement( ) {
  Statement *stmt = NULL;
  if ( CurrentTokenIs( IDENT ) && PeekTokenIs( ASSIGN ) )
    stmt = ParseAssignStmt( ) ;
  /*
  else if ( CurrentTokenIs( SEMICOLON ) )
    stmt = new NullStatement( ) ;
  */

  else if ( CurrentTokenIs( ILLEGAL ) )
    Reset( ) ;
  else
    stmt = ParseExpressionStmt( ) ;

  return stmt;
} // Parser::ParseStatement()

Program *Parser::ParseProgram( ) {
  // Needed object
  Program *program = new Program( ) ;
  Obj *obj = NULL;
  Environment *env = new Environment( ) ;
  // Input string
  string input = "";
  input = GetLine( ) ; 

  cout << "Program starts..." << endl;
  cout << "> ";
  Init( ) ;
  NextToken( ) ; // setting current token to peek token

  while ( mCurToken->value != "quit" ) {

    while ( mCurToken-> type != EOFF ) {
      Statement *stmt = ParseStatement();

      if ( stmt != NULL ) {
        program -> Append( stmt );
        obj = program -> Eval( env );
        if ( obj == NULL )
          Reset( );
        NextToken();
      } // if

      else 
        Reset();
      cout << "> ";
    } // while
    
    Init( );
    NextToken( ) ;
  } // while

  cout << "Program exits..." << endl; 
  return program;
} // Parser::ParseProgram()

// --------------- Main --------------------------
int main( ) {
  Parser *parser = new Parser( ) ;
  Program *program = parser->ParseProgram( ) ;
} // main()