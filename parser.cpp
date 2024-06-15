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
  PLUSPLUS,
  MINUSMINUS,
  MINUS,
  MULTIPLY,
  DIVIDE,
  COMMA,
  SEMICOLON,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  MODULO,
  LT,
  GT,
  LTEQ,
  GTEQ,
  EQ,
  NOT_EQ,
  NOT,
  LEFT_SHIFT, 
  RIGHT_SHIFT,
  AND,
  ANDAND,
  PASSREF,
  OR,
  OROR,
  PLUS_EQ, 
  MINUS_EQ, 
  MULTI_EQ, 
  DIVIDE_EQ,
  STRING,


  // Keywords
  KEY_IF,
  KEY_ELSE,
  KEY_ELIF,
  KEY_INT,
  KEY_STRING,
  KEY_FLOAT,
  KEY_BOOL,
  KEY_VOID,
  KEY_RETURN,
  KEY_DO,
  KEY_WHILE,
  KEY_CHAR
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

  else if ( type == NOT_EQ ) 
    result = fabs( left_value - right_value ) >= epsilon;
  

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
  else if ( type == NOT_EQ ) 
    result = left_value != right_value;

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
  Token *ReadAnd( );
  Token *ReadString( );
  Token *ReadCompare( ) ;
  Token *ReadOr( ); 
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

Token* Lexer::ReadOr( ) {
  Token* tok = NULL;
  if ( PeekChar() == '|' ) {
    tok = SetNewToken( "||", OROR );
    ReadChar();
  } // if

  else 
    tok = SetNewToken( "|", OR );

  return tok;
} // Lexer::ReadOr() 

Token* Lexer::ReadAnd( ) {
  Token *tok = NULL;
  if ( PeekChar( ) == '&' ) {
    tok = SetNewToken( "&&", AND );
    ReadChar( );
  } // if

  else 
    tok = SetNewToken( "&", PASSREF ); 

  return tok;
} // Lexer::ReadAnd() 

Token *Lexer::ReadString() {
  string str = "";
  ReadChar();
  while ( mCh != '"' ) {
    str += mCh;
    ReadChar( );
  } // while

  Token* tok = SetNewToken( str, STRING );
  return tok;
} // Lexer::ReadString()

Token *Lexer::ReadAssign( ) {
  Token *tok = NULL;
  if ( PeekChar( ) == '=' ) {
    tok = SetNewToken( "==", EQ ) ;
    ReadChar( ) ;
  } // if
  else
    tok = SetNewToken( "=", ASSIGN ) ;
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

  else if ( PeekChar( ) == '=' ) {
    tok = SetNewToken( "/=", DIVIDE_EQ ) ;
    ReadChar();
  } // else if

  else
    tok = SetNewToken ( "/", DIVIDE );
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
  else if ( id == "void" )
    tok = SetNewToken( id, KEY_VOID );
  else if ( id == "int" )
    tok = SetNewToken( id, KEY_INT );
  else if ( id == "string" )
    tok = SetNewToken( id, KEY_STRING );
  else if ( id == "float" ) 
    tok = SetNewToken( id, KEY_FLOAT );
  else if ( id == "bool" )
    tok = SetNewToken( id, KEY_BOOL );
  else if ( id == "do" )
    tok = SetNewToken( id, KEY_DO );
  else if ( id == "while" )
    tok = SetNewToken( id, KEY_WHILE );
  else if ( id == "return" )
    tok = SetNewToken( id, KEY_RETURN );

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
  if ( PeekChar() == '=' ) {
    if ( mCh == '+' )
      tok = SetNewToken( "+=", PLUS_EQ ) ;
    else if ( mCh == '-' )
      tok = SetNewToken( "-=", MINUS_EQ ) ;
    else if ( mCh == '*' )
      tok = SetNewToken( "*=", MULTI_EQ ) ;
    ReadChar();
  } // if

  else {
    if ( mCh == '+' ) { 
      if ( PeekChar() == '+' ) {
        tok = SetNewToken( "++", PLUSPLUS );
        ReadChar();
      } // if
      else 
        tok = SetNewToken( sign, PLUS ) ;
    } // if

    else if ( mCh == '-' ) { 
      if ( PeekChar() == '-' ) {
        tok = SetNewToken( "--", MINUSMINUS );
        ReadChar();
      } // if
      else 
        tok = SetNewToken( sign, MINUS ) ;
    } // if

    else if ( mCh == '*' )
      tok = SetNewToken( sign, MULTIPLY ) ;

  } // else

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

    else if ( PeekChar() == '<' ) {
      tok = SetNewToken( "<<", LEFT_SHIFT );
      ReadChar();
    } // else if

    else
      tok = SetNewToken( "<", LT ) ;
  } // if

  else if ( mCh == '>' ) {
    if ( PeekChar() == '=' ) {
      tok = SetNewToken( ">=", GTEQ );
      ReadChar();
    } // if 

    else if ( PeekChar() == '>' ) {
      tok = SetNewToken( ">>", RIGHT_SHIFT );
      ReadChar();
    } // else if 
    
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

  else if ( mCh == '!' )
    tok = ReadNot();

  else if ( mCh == '|' )
    tok = ReadOr();

  else if ( mCh == '=' )
    tok = ReadAssign( ) ;

  else if ( mCh == '{' || mCh == '}' )
    tok = ReadBraces( ) ;

  else if ( mCh == '<' || mCh == '>' )
    tok = ReadCompare(); 

  // Skip comment
  else if ( mCh == '/' )
    tok = ReadDivide( ) ;

  else if ( mCh == ',' )
    tok = SetNewToken( ",", COMMA );

  else if ( mCh == '&' )
    tok = ReadAnd( );

  else if ( mCh == '%' )
    tok = SetNewToken( "%", MODULO );

  else if ( mCh == '\"' )
    tok = ReadString( );

  else {
    string illegal = "";
    illegal += mCh;
    tok = SetNewToken( illegal, ILLEGAL ) ;
  } // else

  ReadChar( ) ;
  return tok;
} // Lexer::ReadNextToken()

// --------------------------- Environment --------------------------
class Obj;
// ------------------Environment------------------------------------------
class Environment {
  map< string, Obj * > mVars;
  Environment *mOuter;

public:
  Environment( ) {
    mOuter = NULL;
  } // Environment()

  void SetOuter( Environment * env ) {
    mOuter = env; 
  } // SetOuter

  Environment *NewEnclosedEnvironment( Environment* outer );
  void Set( string var, Obj *data ) ;
  Obj *Get( string var ) ;
  bool VarExist( string var ) ;
};

Environment *Environment::NewEnclosedEnvironment( Environment* outer ) {
  Environment *env = new Environment();
  env -> SetOuter( outer );
  return env;
} // Environment::NewEnclosedEnvironment()

bool Environment::VarExist( string var ) {
  if ( mVars.find( var ) == mVars.end( ) && mOuter != NULL )
    return mOuter -> VarExist( var );
  return true;
} // Environment::VarExist()

void Environment::Set( string var, Obj *data ) {
  if ( VarExist( var ) )
    mVars [ var ] = data;
  else
    mVars [ var ] = data;
} // Environment::Set()

Obj *Environment::Get( string var ) {
  Obj* obj = NULL; 
  if ( mVars.find( var ) != mVars.end( ) ) 
    obj = mVars[ var ]; 

  else {
    if ( mOuter != NULL ) 
      obj = mOuter -> Get( var );
  } // else

  return obj;
} // Environment::Get()

class Parameter; 
// --------------------------- Data type -----------------------------
class Obj {
public:
  virtual void Inspect( ) = 0;
  virtual string Type( ) = 0;
  virtual string Value( ) = 0;
  virtual Obj* Eval( Environment* env ) = 0;
  virtual Environment *GetEnv( ) = 0; 
  virtual vector < Parameter *> GetParameter( ) = 0;
};

class Return : public Obj {
  string mType;
  Obj* mValue;
public:

  Return( Obj* value ) {
    mValue = value;
    mType = "Return";
  } // Return

  void Inspect(); 
  string Type();
  string Value();

  Obj* Eval( Environment *env ) {
    return mValue;
  } // Eval();

  Environment *GetEnv( ) {
    return NULL;
  } // GetEnv() 

  vector < Parameter *> GetParameter( ) {
    vector< Parameter*> prm;
    return prm;
  } // GetParameter()

} ; 

void Return::Inspect() {
  mValue -> Inspect();
} // Return::Inspect()

string Return::Type() {
  return mType;
} // Return::Type()

string Return::Value() {
  string str = mValue -> Value();
  return str;
} // Return::Value()


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
  Obj* Eval( Environment *env );
  
  Environment *GetEnv( ) {
    return NULL;
  } // GetEnv() 
 
  vector < Parameter *> GetParameter( ) {
    vector< Parameter*> prm;
    return prm;
  } // GetParameter()

};

Obj* Integer::Eval( Environment *env ) {
  return NULL;
} // Integer::Eval()

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
  
  Environment *GetEnv( ) {
    return NULL;
  } // GetEnv() 

  Obj* Eval( Environment *env );
  string Type( ) ;
  string Value( ) ;
  void Inspect( ) ;
  
  vector < Parameter *> GetParameter( ) {
    vector< Parameter*> prm;
    return prm;
  } // GetParameter()
};


Obj* Float::Eval( Environment *env ) {
  return NULL; 
} // Float::Eval()

string Float::Type( ) { return mType; } // Float::Type()

string Float::Value( ) {
  stringstream ss;
  ss << mValue;
  string res = ss.str( ) ;
  return res;
} // Float::Value()

void Float::Inspect( ) { 
  cout << fixed << setprecision( 3 ) << GRound( mValue ) << endl; 
} // Float::Inspect()

class Boolean : public Obj {
  string mType;
  bool mValue;

public:
  Boolean( bool value, string tp ) {
    mType = tp;
    mValue = value;
  } // Boolean()

  Obj* Eval( Environment *env ) {
    return NULL;
  } // Eval()

  Environment *GetEnv( ) {
    return NULL;
  } // GetEnv() 

  string Type( ) ;
  string Value( ) ;
  void Inspect( ) ;

  vector < Parameter *> GetParameter( ) {
    vector< Parameter*> prm;
    return prm;
  } // GetParameter()
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

  Environment *GetEnv( ) {
    return NULL;
  } // GetEnv() 

  Obj* Eval( Environment *env ) {
    return NULL;
  } // Eval()

  void Inspect( ) ;
  string Type( ) ;
  string Value( ) ;
 
  vector < Parameter *> GetParameter( ) {
    vector< Parameter*> prm;
    return prm;
  } // GetParameter()
};

string String::Type( ) { return mType; } // String::Type()

string String::Value( ) { return mValue; } // String::Value()

void String::Inspect( ) { cout << mValue << endl; } // String::Inspect()

class Null : public Obj {
  string mType;

public:
  Null( ) { mType = "NULL"; } // Null()

  Obj* Eval( Environment *env ) {
    return NULL;
  } // Eval()

  Environment *GetEnv( ) {
    return NULL;
  } // GetEnv() 


  void Inspect( ) ;
  string Type( ) ;
  string Value( ) ;
  
  vector < Parameter *> GetParameter( ) {
    vector< Parameter*> prm;
    return prm;
  } // GetParameter()
};

string Null::Type( ) { return mType; } // Null::Type()

string Null::Value( ) { return "Null"; } // Null::Value()

void Null::Inspect( ) { cout << "" << endl; } // Null::Inspect()

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
// --------------------------- Block Statement -----------------------
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

  vector< Statement* > GetStmts() {
    return mStmts;
  }  // GetStmts()

  Obj *Eval( Environment *env ) ;
};

void BlockStatement::Append( Statement *stmt ) {

  mStmts.push_back( stmt ) ;

} // BlockStatement::Append()

void BlockStatement::Print( ) {
  cout << "{ \n";
  for ( int i = 0; i < mStmts.size( ) ; ++i ) {
    mStmts [ i ]->Print( ) ;
  } // for
  cout << "} \n ";
} // BlockStatement::Print()

string BlockStatement::Type( ) { return mType; } // BlockStatement::Type()

Obj *BlockStatement::Eval( Environment *env ) { 
  Obj *obj = NULL;
  for ( int i = 0; i < mStmts.size(); ++i ) {
    obj = mStmts[i] -> Eval( env );
    if ( obj != NULL && obj -> Type() == "Return" ) 
      return obj;
  } // for 

  return obj;
} // BlockStatement::Eval()

string BlockStatement::Value( ) { return mTok->value; } // BlockStatement::Value()


//----------------- if else ------------------
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

  if ( leftType == "String" || rightType == "String" ) {
    string result;
    if ( mOp->type == PLUS )
      result = leftValue + rightValue;

    else { 
      cout << "Incompatible type between " 
           <<  leftType << " and " << rightType << endl; 
      return NULL;
    } // else 
  
    res = new String( "String", result ) ;
  } // if


  else if ( leftType == "Float" || rightType == "Float" ) {
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

    else if ( mOp -> type == MODULO )
      result = left_value % right_value;

    else if ( mOp -> type == RIGHT_SHIFT )
      result = left_value >> right_value;
  
    else if ( mOp -> type == LEFT_SHIFT )
      result = left_value << right_value;

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
  if ( left == NULL || right == NULL ) {
    string undefined = "";
    if ( left == NULL ) 
      undefined = mLeft -> Value();
    else if ( right == NULL )
      undefined = mRight -> Value(); 

    cout << "Undefined identifier : '" << undefined << "'\n";
    return NULL;
  } // if

  string leftType = left->Type( ) ;
  string rightType = right->Type( ) ;

  if ( mOp->type == PLUS || mOp->type == MINUS || mOp->type == DIVIDE ||
       mOp->type == MULTIPLY || mOp->type == LEFT_SHIFT || mOp -> type == RIGHT_SHIFT )
    result = EvalArithmatic( left, right ) ;
  else if ( mOp->type == LT || mOp->type == GT || 
            mOp->type == EQ || mOp->type == GTEQ || 
            mOp -> type == LTEQ || mOp->type == NOT_EQ )
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

void SymbolExpression::Print() {
  cout << mValue;
} // SymbolExpression::Print()

Obj *SymbolExpression::Eval( Environment *env ) {
  Obj *var = NULL;
  var = env->Get( mValue ) ;
  return var;
} // SymbolExpression::Eval()

class UpdateExpression : public Expression {
  Token *mOp;
  SymbolExpression *mId;
  string mType;
  bool mPrefix;
public:
  UpdateExpression( Token* tok, SymbolExpression *id, bool prefix ) {
    mOp = tok;
    mId = id;
    mPrefix = prefix;
    mType = "Update Expression";
  } // UpdateExpression()
  
  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return mOp -> value; 
  } // Value()

  void Expr( ) { 
  } // Expr()

  void Print( ) ;
  
  Obj *Eval( Environment *env ) ;
} ;

void UpdateExpression::Print() {
  cout << "(";
  if ( mPrefix ) {
    cout << mOp -> value; 
    mId -> Print();
  } // if
  
  else { 
    mId -> Print();
    cout << mOp -> value;
  } // else

  cout << ")"; 
} // UpdateExpression::Print()

Obj *UpdateExpression::Eval( Environment *env ) {
  Obj* obj = env -> Get( mId -> Value() ); 
  string value = obj -> Value();

  Obj* res = NULL; 
  if ( mPrefix ) {

    if ( obj -> Type() == "Float" ) {
      float num = GStringToFloat(value);
      num += 1.0;
      res = new Float( num, "Float" );
    } // if

    else if ( obj -> Type() == "Integer" ) {
      int num = GStringToInt(value);
      num += 1;
      res = new Integer( num, "Integer" );
    } // if
 
    env -> Set( mId -> Value(), res );
    return env -> Get( mId -> Value() ); 
  } // if

  else {
    if ( obj -> Type() == "Float" ) {
      float num = GStringToFloat(value);
      num += 1.0;
      res = new Float( num, "Float" );
    } // if

    else if ( obj -> Type() == "Integer" ) {
      int num = GStringToInt(value);
      num += 1;
      res = new Integer( num, "Integer" );
    } // if

    env -> Set( mId -> Value(), res );
    return obj; 
  } // if

} // UpdateExpression::Eval()

class Parameter : public Expression {
  Token* mTok; // kind token
  string mType;
  SymbolExpression* mPara;
  bool mPassByRef;

public:
  Parameter( Token *tok, SymbolExpression* para, bool pbr ) {
    mTok = tok;
    mType = "Parameter";
    mPara = para; 
    mPassByRef = pbr; 
  } // Parameter()
  
  Parameter() {} // Parameter()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return mPara -> Value(); 
  } // Value()

  void Expr( ) { 
  } // Expr()

  void Print( ) ;
  
  Obj *Eval( Environment *env ) ;
  
};

void Parameter::Print() {
  cout << mTok -> value << " " << mPara -> Value();
} // Parameter::Print()

// Register parameter to current env
Obj* Parameter::Eval( Environment *env ) {
  Obj* obj = NULL;
  if ( mTok -> type == KEY_INT ) 
    obj = new Integer( 0, "Integer");
  else if ( mTok -> type ==  KEY_FLOAT )
    obj = new Float( 0.0, "Float");
  else if ( mTok -> type == KEY_STRING )
    obj = new String( "", "String" );
  
  string name = mPara -> Value();
  env -> Set( name, obj );
  return obj;
} // Parameter::Eval()

class Function : public Obj {
  Token* mKind;
  Expression* mName;
  vector< Parameter* > mPara;
  BlockStatement *mBody;
  Environment *mEnv;
  string mType;
public: 
  Function( Token* kind, Expression* name, 
           BlockStatement* bstmt, Environment* env, 
           vector< Parameter* > para ) {
    mName = name;
    mKind = kind; 
    mPara = para;
    mEnv = env;
    mBody = bstmt;
    mType = "Function";
  } // Function()

  Obj* Eval( Environment *env );
  void Inspect();
  string Type(); 
  string Value();

  Environment *GetEnv( ) {
    return mEnv;
  } // GetEnv() 

  vector < Parameter *> GetParameter( ) {
    return mPara;
  } // GetParameter()
} ;

Obj *Function::Eval( Environment *env ) {
  Obj* blockObj = mBody -> Eval( env );
  return blockObj;
} // Function::Eval()

void Function::Inspect() {
  cout << mKind -> value << " ";
  mName -> Print();
  cout << " (";
  for ( int i = 0; i < mPara.size(); ++i ) {
    mPara[i] -> Print(); 
    if ( i + 1 < mPara.size() )
      cout << ", ";
  } // for

  cout << ") \n";
  mBody -> Print();
} // Function::Inspect()

string Function::Value() {
  string str = mKind -> value;
  return str;
} // Function::Value()

string Function::Type() {
  return mType;
} // Function::Type()

class FunctionDeclaration : public Statement {
  Token* mTok; // Type token
  SymbolExpression *mId;
  vector< Parameter *> mParams;
  BlockStatement *mBlockStmt;
  string mType;
  Environment *mEnv;

public:

  FunctionDeclaration( Token* tok, SymbolExpression *id ) {
    mTok = tok;
    mId = id;
    mType = "Function Declaration";
  } // FunctionDeclaration()

  void Append( Parameter* prm );

  void SetEnv( Environment* env ) {
    mEnv = env; 
  } // SetEnv() 

  void SetBlock( BlockStatement* bstmt ) {
    mBlockStmt = bstmt; 
  } // SetBlock()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return ""; 
  } // Value()


  void Stmt( ) { 
  } // Expr()

  void Print( ) ;

  Obj *Eval( Environment *env ) ;
} ;

void FunctionDeclaration::Append( Parameter* prm ) {
  mParams.push_back( prm );
} // FunctionDeclaration::Append()

void FunctionDeclaration::Print() {
  cout << mTok -> value << " " << mId -> Value() << "(";
  for ( int i = 0; i < mParams.size(); ++i ) {
    cout << " ";
    mParams[i] -> Print();
    if ( i + 1 < mParams.size() )
      cout << ","; 
  } // for
  
  cout << " ) {\n";
  vector <Statement *> stmts = mBlockStmt -> GetStmts();
  
  for ( int i = 0; i < stmts.size(); ++i ) {
    cout << "  ";
    stmts[i] -> Print();
  } // for

  cout << "}\n";
} // FunctionDeclaration::Print()

Obj* FunctionDeclaration::Eval( Environment* env ) {
  Obj *obj = new Function( mTok, mId, mBlockStmt, mEnv, mParams );
  env -> Set( mId -> Value(), obj ); 
  return obj;
}  // FunctionDeclaration::Eval() 

class CallExpression : public Expression {
  Obj* mFunction;
  vector < Expression* > mArgs;
  string mValue;
  string mType;

public:
  CallExpression( Obj *fid, vector< Expression*> args ) {
    mFunction = fid;
    mArgs = args;
    mType = "Call Expression";
    mValue = "";
  } // CallExpression()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return mValue; 
  } // Value()

  void Expr( ) { 
  } // Expr()

  void Print( ) ;
  Obj* ApplyFunction( Obj* function, vector< Obj* > args );
  Environment* ExtendFunctionEnv( Obj* function, vector< Obj* > args ); 
  Obj *Eval( Environment *env ) ;

  vector < Parameter *> GetParameter( ) {
    vector< Parameter*> prm;
    return prm;
  } // GetParameter()

} ;

void CallExpression::Print() {
  mFunction -> Value();
  cout << "( ";
  for ( int i = 0; i < mArgs.size(); ++i ) {
    mArgs[i] -> Print();
    if ( i + 1 < mArgs.size() )
      cout << ", ";
  } // for 
  cout << ") ";
} // CallExpression::Print(); 

Environment* CallExpression::ExtendFunctionEnv( Obj* function, 
                                               vector< Obj* > args ) {
  // This would get the inner environment
  Environment* env =  function -> GetEnv(); 
  // Register the parameter
  vector < Parameter* > parameter = function -> GetParameter();

  // Update the inner environment variable to equal to args
  for ( int i = 0; i < parameter.size(); i++ ) {
    string name = parameter[i] -> Value();
    env -> Set( name, args[i] ); 
  } // for

  return env;
} // CallExpression::ExtendFunctionEnv()


Obj* CallExpression::ApplyFunction( Obj* function, vector< Obj* > args ) {
  if ( function -> Type() != "Function" ) 
    return NULL;

  Environment *extendEnv = ExtendFunctionEnv( function, args );
  // Evaluate the block statement
  Obj* blockStmt = function -> Eval( extendEnv );
  if ( blockStmt -> Type() == "Return" ) {

    if ( mFunction -> Value() == "void" ) {
      cout << "Error : Void function should not return a value." << endl;
      return NULL; 
    } // if 

  } // if
  return blockStmt;
} // CallExpression::ApplyFunction() 

Obj* CallExpression::Eval( Environment *env ) {
  vector < Obj* > args;
  for ( int i = 0; i < mArgs.size(); ++i ) {
    Obj* arg = mArgs[i] -> Eval( env );
    if ( arg != NULL )
      args.push_back( arg );
  } // for

  Obj* ret = ApplyFunction( mFunction, args );
  return ret; 
} // CallExpression::Eval()


class DeclarationStatement : public Statement {
  Token* mTok; // Type token
  vector < SymbolExpression* > mIds;
  string mType;

public: 
  DeclarationStatement( Token* tok ) {
    mTok = tok;
    mType = "Declaration statement";
  } // DeclarationStatement()

  void Stmt( ) {
  } // Stmt()
  
  void Append( Token* id ); 
  void Print( ) ;
  string Type( ) ;
  string Value( ) ;
  Obj *Eval( Environment *env ) ;
};

void DeclarationStatement::Append( Token* id ) {
  SymbolExpression *symbol = new SymbolExpression( id, id -> value);
  mIds.push_back( symbol );
} // DeclarationStatement::Append()

void DeclarationStatement::Print() {
  cout << mTok -> value << " ";
  for ( int i = 0; i < mIds.size(); ++i ) {
    mIds[i] -> Print();
    if ( i + 1 < mIds.size() )
      cout << ", ";  
  } // for 
  cout << ";";
} // DeclarationStatement::Print()

string DeclarationStatement::Type() {
  return mType; 
} // DeclarationStatement::Type()

string DeclarationStatement::Value() {
  return mTok -> value;
} // DeclarationStatement::Value()

Obj *DeclarationStatement::Eval( Environment *env ) {
  TokenType tp = mTok -> type;
  Obj* obj = NULL;
  if ( tp == KEY_INT )
    obj = new Integer( 0, "Integer" );
  else if ( tp == KEY_FLOAT )
    obj = new Float( 0.0, "FLoat" );
  else if ( tp == KEY_STRING ) 
    obj = new String( "String", "" );

  for ( int i = 0; i < mIds.size(); ++i )
    env -> Set( mIds[i] -> Value(), obj );

  return obj;
} //  DeclarationStatment::Eval()

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
class ReturnStmt : public Statement {
  Token *mTok;
  Expression* mReturnValue;
  string mType; 

public:
  ReturnStmt( Token* tok, Expression* value) {
    mTok = tok;
    mReturnValue = value; 
    mType = "Return Statement";
  } // ReturnStmt()
  
  void Stmt() {}

  void Print();
  string Type(); 
  string Value();
  Obj* Eval( Environment* env ); 
} ;

Obj* ReturnStmt::Eval( Environment* env ) {
  Obj *obj = mReturnValue -> Eval( env );
  Obj *res = new Return( obj );
  return res;
} // ReturnStmt::Eval()

string ReturnStmt::Type() {
  return mType;
} // ReturnStmt::Type()

string ReturnStmt::Value() {
  return "";
} // ReturnStmt::Value()

void ReturnStmt::Print() {
  cout << mTok -> value << " ";
  mReturnValue -> Print();
  cout << ";\n";
} // ReturnStmt::Print()

// ------------------------------- Statements --------------------------
class AssignmentExpr : public Expression {
  Token *mToken; // Operator token 
  Expression *mName; // Variable name
  Expression *mValue;
  string mType;

public:
  AssignmentExpr( Token *token, Expression *name, Expression *value ) {
    mToken = token;
    mName = name;
    mValue = value;
    mType = "Assignment Expression";
  } // AssignmentExpr()

  string Type( ) { 
    return mType; 
  } // Type()

  string Value( ) { 
    return ""; 
  } // Value()

  void Expr( ) {
  } // Stmt()

  void Print( ) ;

  Obj *Eval( Environment *env ) ;
  Obj *EvalFloat( string var, string rhs ) ;
  Obj *EvalInt( string var, string rhs ) ;
  Obj *EvalString( string var, string rhs ) ;
};

Obj *AssignmentExpr::EvalInt( string var, string rhs ) {
  int left = GStringToFloat( var );
  int right = GStringToFloat( var );
  if ( mToken -> type == PLUS_EQ )
    left += right;
  else if ( mToken -> type == MINUS_EQ )
    left -= right;
  else if ( mToken -> type == DIVIDE_EQ )
    left /= right;
  else if ( mToken -> type == MULTI_EQ ) 
    left *= right;

  Obj *obj = new Integer( left,"Integer" );
  return obj;
} // AssignmenExpr::EvalFloat()

Obj *AssignmentExpr::EvalString( string var, string rhs ) {
  if ( mToken -> type == PLUS_EQ )
    var += rhs ;

  else {
    cout << "Incorrect operation on string\n"; 
    return NULL;
  } // else

  Obj *obj = new String( "String", var );
  return obj;
} // AssignmenExpr::EvalFloat()

Obj *AssignmentExpr::EvalFloat( string var, string rhs ) {
  float left = GStringToFloat( var );
  float right = GStringToFloat( var );
  if ( mToken -> type == PLUS_EQ )
    left += right;
  else if ( mToken -> type == MINUS_EQ )
    left -= right;
  else if ( mToken -> type == DIVIDE_EQ )
    left /= right;
  else if ( mToken -> type == MULTI_EQ ) 
    left *= right;

  Obj *obj = new Float( left,"Float" );
  return obj;
} // AssignmenExpr::EvalFloat()

Obj *AssignmentExpr::Eval( Environment *env ) {
  Obj *rhs = mValue->Eval( env ) ;
  if ( rhs == NULL )
    return NULL;

  string varName = mName->Value( ) ;
  Obj *var = env -> Get( varName );

  string var_value = var -> Value( );
  string rhs_value = rhs -> Value( );

  if ( mToken -> type == ASSIGN ) {
    env -> Set( varName, rhs );
    var = env -> Get( varName );
  } // if
  
  else { 
    Obj *result = NULL;
    if ( var -> Type() == "Float" || rhs -> Type() == "Float" ) 
      result = EvalFloat( var_value, rhs_value );
    else if ( var -> Type() == "String" && rhs -> Type() == "String" ) 
      result = EvalString( var_value, rhs_value );
    else if ( var -> Type() == "Integer" && rhs -> Type() == "Integer" )
      result = EvalInt( var_value, rhs_value );
    else {
      cout << "Invalid operation between : " << var -> Type() << " and "
           << rhs -> Type();
      return NULL;
    } // else

    env -> Set( varName,result ); 
    var = env -> Get( varName ); 
  } // else
  
  return var;
} // AssignmentExpr::Eval()

void AssignmentExpr::Print( ) {
  mName->Print( ) ;
  cout << " = (";
  mValue->Print( ) ;
  cout << ") ";
} // AssignmentExpr::Print()


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
  SET,
  EQUAL,
  LESSGREATER,
  SUM,
  SHIFT,
  PRODUCT,
  ANDBP,
  ORBP,
  PREFIX,
  CALL,
} BindingPower
;

typedef enum {
  ASSIGNMENT_PARSER,
  IDENTIFER_PARSER,
  NUMBER_PARSER,
  PREFIX_PARSER,
  INFIX_PARSER,
  GROUP_PARSER,
  IF_PARSER,
  ILLEGAL_PARSER,
  CALL_PARSER,
  STRING_PARSER
} Handler
;

class Parser {
  Lexer *mLexer;
  Token *mCurToken;
  vector< string > mErrs;
  map< TokenType, Handler > mPrefixFNs;
  map< TokenType, Handler > mInfixFNs;
  map< TokenType, Handler > mPostFixFNs;
  map< TokenType, BindingPower > mPrecedences;
  vector< Token* > mToks;

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
  Token* Pop( ) ;
  bool Expect( Token* tok, TokenType tp ) ;
  bool CurrentTokenIs( TokenType type ) ;
  BindingPower BPLookUp( Token* tok ) ;
  vector< string > Errors( ) ;

  // Parsing stmt
  Statement *ParseExpressionStmt( Environment *env ) ;
  Statement *ParseStatement( Environment *env ) ;
  Statement *ParseLetStmt( Environment *env );
  Statement *ParseReturnStmt( Environment *env );
  Statement* ParseCompound( Environment *env ); 
  Statement *ParseAssignStmt( Environment *env );
  FunctionDeclaration *ParseFunction( Environment *env, FunctionDeclaration *fn ); 
  Program *ParseProgram( ) ;

  // Parse expression
  BlockStatement *ParseBlockStatement( Environment* env ) ;
  Expression *ParseIf( ) ;
  vector <Parameter *> ParseParameter( ); 
  Expression *ParseGroup( Environment *env ) ;
  Expression *ParseNumber( Environment *env ) ;
  Expression *ParseString( Environment *env ) ;
  Expression *ParseIdentifier( Environment *env ) ;
  Expression *ParseExpression( BindingPower bp, Environment *env ) ;
  Expression *ParseInfix( Expression *left, Environment *env ) ;
  Expression *ParsePrefix( Environment *env ) ;
  Expression *ParseAssignExpr( Expression* left, Environment *env ) ;
  Expression *ParseCallExpr( Expression* left, Environment *env );
  vector <Expression *> ParseCallArgs( Environment *env ); 

  // Map register
  Expression *PrefixExecutor( Environment *env ) ;
  Expression *InfixExecutor( Expression *left, Environment *env ) ;
  void RegisterPrefix( TokenType tp, Handler fn ) ;
  void RegisterInfix( TokenType tp, Handler fn ) ;
};

Token *Parser::Pop( ) {

  Token *top = NULL;
  if ( mToks.size() > 0 ) {
    top = mToks[0];
    mToks.erase( mToks.begin() );
  } // if

  else cout << "Tokens list is empty \n";

  return top;
} // Parser::Pop() 

bool Parser::Expect( Token* tok, TokenType tp ) {
  if ( tok -> type == tp ) 
    return true;
  else {
    string str = "Unexpected token : '" + tok -> value + "'\n";
    mErrs.push_back( str );
    return false; 
  } // else

} // Parser::Expect()

void Parser::NextToken( ) {
  if ( mCurToken -> type != SEMICOLON ) {
    mCurToken = mLexer -> ReadNextToken( );
    if ( mCurToken -> type == EOFF ) 
      Init();
    else
      mToks.push_back( mCurToken ); 
  } // if
} // Parser::NextToken()

void Parser::Reset( ) { 
  mCurToken->value = '\0'; 
  mCurToken->type = EOFF;
  mToks.clear();
  mErrs.clear();
} // Parser::Reset()

void Parser::Init( ) {
  string str = GetLine(); 
  mLexer = new Lexer( str ) ;
  mCurToken = mLexer->ReadNextToken( ) ;
  while ( mCurToken -> type == EOFF ) {
    str = GetLine(); 
    mLexer = new Lexer( str ) ;
    mCurToken = mLexer->ReadNextToken( ) ;
  } // while

  mToks.push_back( mCurToken );
} // Parser::Init()

// Mapping the function with token type
void Parser::Mapper( ) {

  // Register the binding power of operation
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
  mPrecedences [ MODULO ] = SHIFT;
  mPrecedences [ ASSIGN ] = SET; 
  mPrecedences [ DIVIDE_EQ ] = SET;
  mPrecedences [ MULTI_EQ ] = SET;
  mPrecedences [ PLUS_EQ ] = SET;
  mPrecedences [ MINUS_EQ ] = SET;
  mPrecedences [ LPAREN ] = CALL;
  mPrecedences [ RIGHT_SHIFT ] = SHIFT;
  mPrecedences [ LEFT_SHIFT ] = SHIFT;

  // Mapping token type with their associated parser
  RegisterPrefix( IDENT, IDENTIFER_PARSER ) ;
  RegisterPrefix( FLOAT, NUMBER_PARSER ) ;
  RegisterPrefix( INT, NUMBER_PARSER ) ;
  RegisterPrefix( MINUS, PREFIX_PARSER ) ;
  RegisterPrefix( PLUS, PREFIX_PARSER ) ;
  RegisterPrefix( KEY_IF, IF_PARSER ) ;
  RegisterPrefix( LPAREN, GROUP_PARSER ) ;
  RegisterPrefix( ILLEGAL, ILLEGAL_PARSER );
  RegisterPrefix( PLUSPLUS, IDENTIFER_PARSER );
  RegisterPrefix( MINUSMINUS, IDENTIFER_PARSER );
  RegisterPrefix( STRING, STRING_PARSER );

  RegisterInfix( MINUS_EQ, ASSIGNMENT_PARSER );
  RegisterInfix( DIVIDE_EQ, ASSIGNMENT_PARSER );
  RegisterInfix( MULTI_EQ, ASSIGNMENT_PARSER );
  RegisterInfix( PLUS_EQ, ASSIGNMENT_PARSER );
  RegisterInfix( ASSIGN, ASSIGNMENT_PARSER );

  RegisterInfix( MODULO, INFIX_PARSER );
  RegisterInfix( LEFT_SHIFT, INFIX_PARSER );
  RegisterInfix( RIGHT_SHIFT, INFIX_PARSER );
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
  RegisterInfix( ILLEGAL, ILLEGAL_PARSER );
  RegisterInfix( LPAREN, CALL_PARSER );

} // Parser::Mapper()

Expression *Parser::PrefixExecutor( Environment *env ) {

  Expression *expr = NULL;
  Handler fn = mPrefixFNs [ mToks[0] -> type ];

  if ( fn == IDENTIFER_PARSER ) {
    expr = ParseIdentifier( env ) ;
    return expr; 
  } // if

  else if ( fn == NUMBER_PARSER ) 
    expr = ParseNumber( env ) ;

  else if ( fn == PREFIX_PARSER ) {
    expr = ParsePrefix( env ) ;
    return expr;
  } // else if 

  else if ( fn == GROUP_PARSER )
    expr = ParseGroup( env ); 

  else if ( fn == STRING_PARSER )
    expr = ParseString( env );

  else if ( fn == ILLEGAL_PARSER )
    return NULL;

  if ( expr != NULL ) {
    Pop();
    NextToken();
  } // if

  return expr;
} // Parser::PrefixExecutor()

vector<Expression *> Parser::ParseCallArgs( Environment *env ) {
  vector< Expression *> args;
  if ( CurrentTokenIs( RPAREN ) ) 
    return args;

  Expression *expr = ParseExpression( LOWEST, env );
  args.push_back( expr );
  
  while ( CurrentTokenIs( COMMA ) ) {
    Pop();
    NextToken();
    expr = ParseExpression( LOWEST, env );
    args.push_back( expr );
  } // while

  return args; 
} // Parser::ParseCallArgs()

Expression *Parser::ParseCallExpr( Expression *left, Environment *env ) {
  Token* lparen = Pop();
  Expression *expr = NULL; 
  NextToken();
  vector < Expression* > args = ParseCallArgs( env );

  if ( !Expect( mToks[0], RPAREN ) )
    return NULL;

  Pop(); // Skip )
  NextToken();
  Obj* function = env -> Get( left -> Value() );
  expr = new CallExpression( function, args ); 

  return expr;
} // Parser::ParseCallExpr()

Expression *Parser::InfixExecutor( Expression *left, Environment *env ) {
  Expression *expr = NULL;
  Handler fn = mInfixFNs [ mToks[0] -> type ];
  if ( fn == INFIX_PARSER )
    expr = ParseInfix( left, env ) ;

  else if ( fn == ASSIGNMENT_PARSER ) 
    expr = ParseAssignExpr( left, env );

  else if ( fn == ILLEGAL_PARSER )
    return NULL;

  else if ( fn == CALL_PARSER )
    expr = ParseCallExpr( left, env );

  return expr;
} // Parser::InfixExecutor()

void Parser::RegisterPrefix( TokenType tp, Handler fn ) {
  mPrefixFNs [ tp ] = fn;
} // Parser::RegisterPrefix()

void Parser::RegisterInfix( TokenType tp, Handler fn ) {
  mInfixFNs [ tp ] = fn;
} // Parser::RegisterInfix()

BindingPower Parser::BPLookUp( Token* tok ) {
  if ( mPrecedences.find( tok->type ) != mPrecedences.end( ) )
    return mPrecedences [ tok->type ];

  return LOWEST;
} // Parser::BPLookUp()


bool Parser::CurrentTokenIs( TokenType tp ) {
  Token* current = mToks[0];
  if ( tp == current -> type )
    return true;
  return false;
} // Parser::CurrentTokenIs()

Expression* Parser::ParseGroup( Environment *env ) {
  Pop();
  NextToken();
  Expression *expr = ParseExpression( LOWEST, env );
  if ( !CurrentTokenIs( RPAREN ) ) {
    string err = "Unexpected token : '" + mToks[0] -> value + "'\n";
    mErrs.push_back( err ); 
    return NULL; 
  } // if

  if ( expr == NULL )
    return NULL;

  return expr;
} // Parser::ParseGroup()

Expression *Parser::ParseString( Environment *env ) {
  string str = mToks[0] -> value;
  StringExpr *expr = new StringExpr( str );
  return expr;
} // Parser::ParseString()

Expression *Parser::ParseNumber( Environment *env ) {
  stringstream ss( mToks[0]->value ) ;
  if ( mToks[0]->type == INT ) {
    size_t num;
    ss >> num;
    IntExpr *integer = new IntExpr( mToks[0], num ) ;
    return integer;
  } // if

  float num = GStringToFloat( mToks[0]->value );
  FloatExpr *fp = new FloatExpr( mToks[0], num ) ;
  return fp;

} // Parser::ParseNumber()


Expression *Parser::ParsePrefix( Environment *env ) {
  Token *op = Pop();
  NextToken( ) ;

  if ( mToks[0] -> type == ILLEGAL ) {
    string err = "Unrecognized token with first char : '" + mToks[0] -> value + "'\n";
    mErrs.push_back( err ); 
    return NULL; 
  } // if

  Expression *right = ParseExpression( PREFIX, env ) ;
  if ( right == NULL  )
    return NULL;


  UnaryExpression *prefix = new UnaryExpression( op, right ) ;
  return prefix;
} // Parser::ParsePrefix()

Expression *Parser::ParseInfix( Expression *left, Environment *env ) {
  BinExpr* infix = NULL;
  Token *op = Pop(); 
  BindingPower bp = BPLookUp( op );
  NextToken();
  Expression* right = ParseExpression( bp, env );
  if ( right != NULL ) 
    infix = new BinExpr( left, op, right ); 

  return infix;
} // Parser::ParseInfix()

Expression *Parser::ParseIdentifier( Environment *env ) {
  if ( CurrentTokenIs( PLUSPLUS ) || CurrentTokenIs( MINUSMINUS ) ) {
    Token* op = Pop();
    NextToken();

    if ( !Expect( mToks[0], IDENT ) ) 
      return NULL;
    
    if ( env -> VarExist( mToks[0]->value ) == false ) {
      string err = "Undefined identifier : '" + mToks[0] -> value + "'\n";
      mErrs.push_back( err ); 
      return NULL;
    } // if
    

    SymbolExpression *id = new SymbolExpression( mToks[0], mToks[0] -> value );
    UpdateExpression *upexpr = new UpdateExpression( op, id, true );
    Pop();
    NextToken();

    return upexpr;
  } // if

  else {
    SymbolExpression* id = new SymbolExpression( mToks[0], mToks[0] -> value ); 
    
    if ( env -> VarExist( mToks[0]->value ) == false ) {
      string err = "Undefined identifier : '" + mToks[0] -> value + "'\n";
      mErrs.push_back( err ); 
      return NULL;
    } // if
    
    Pop();
    NextToken();
    
    if ( mToks[0] -> type ==  PLUSPLUS || mToks[0] -> type == MINUSMINUS ) {
      Token *op = Pop();
      NextToken();
      UpdateExpression *upexpr = new UpdateExpression( op, id, false ); 
      return upexpr;
    } // if
    
    else 
      return id;
  } // else if

} // Parser::ParseIdentifier()

vector <Parameter*> Parser::ParseParameter() {
  vector <Parameter*> params;
  bool pbr = false;


 
  // Skip ( 
  Pop();
  NextToken();

  if ( CurrentTokenIs( RPAREN ) )
    return params;
    
  // Data type
  Token *kind = Pop();
  NextToken();
  if ( CurrentTokenIs( PASSREF ) ) {
    pbr = true;
    Pop();
    NextToken();
  } // if
  
  // ID 
  Token *id = Pop();
  SymbolExpression *ident = new SymbolExpression( id, id -> value );
  Parameter *param = new Parameter( kind, ident, pbr ); 
  params.push_back( param );

  NextToken();
  while ( CurrentTokenIs( COMMA ) ) {
    pbr = false;
    // Skip comma
    Pop();
    NextToken();
    // Data type
    kind = Pop();
    NextToken();

    if ( CurrentTokenIs( PASSREF ) ) 
      pbr = true;

    // ID
    id = Pop();
    ident = new SymbolExpression( id, id -> value );
    param = new Parameter( kind, ident, pbr );
    params.push_back( param );

    NextToken();
  } // while

  return params;
} // Parser::ParseParameter()


Expression *Parser::ParseExpression( BindingPower bp, Environment *env ) {
  // At first enter
  if ( mToks[0] -> type == ILLEGAL ) {
    string err = "Unrecognized token with first char : '" + mToks[0] -> value + "'\n";
    mErrs.push_back( err ); 
    return NULL; 
  } // if

  if ( mPrefixFNs.find( mToks[0] -> type ) == mPrefixFNs.end() ) {
    string err = "Unexpected token : '" + mToks[0] -> value + "'\n";
    mErrs.push_back( err ); 
    return NULL;
  } // if

  Expression *left = PrefixExecutor( env );
  if ( left == NULL )
    return NULL;
    
  if ( mToks[0] -> type == ILLEGAL ) {
    string err = "Unrecognized token with first char : '" + mToks[0] -> value + "'\n";
    mErrs.push_back( err ); 
    return NULL;
  } // if 

  while ( mToks[0] -> type != SEMICOLON && bp < BPLookUp( mToks[0] ) ) {
    if ( mInfixFNs.find( mToks[0] -> type ) == mInfixFNs.end() ) {
      string err = "Unexpected token : '" + mToks[0] -> value + "'\n";
      mErrs.push_back( err ); 
      return NULL;
    } // if
      
    left = InfixExecutor( left, env );
    if ( mToks[0] -> type == ILLEGAL ) {
      string err = "Unrecognized token with first char : '" + mToks[0] -> value + "'\n";
      mErrs.push_back( err ); 
      return NULL;
    } // if 

    if ( left == NULL )
      return NULL;
  } // while

  return left;
} // Parser::ParseExpression() 

BlockStatement* Parser::ParseBlockStatement( Environment *env ) {
  if ( !Expect( mToks[0], LBRACE ) ) 
    return NULL;
  Token* brace = Pop();
  BlockStatement* bstmt = new BlockStatement( brace );
  NextToken();

  if ( CurrentTokenIs( RBRACE ) ) 
    return bstmt;

  while ( !CurrentTokenIs( RBRACE ) ) {
    Statement *stmt = ParseCompound(env);

    if ( stmt == NULL )
      return NULL;

    bstmt -> Append( stmt );
    mCurToken  = mLexer -> ReadNextToken();
    if ( mCurToken -> type != EOFF )
      mToks.push_back( mCurToken );
    else 
      Init();
  } // while

  return bstmt;
} // Parser::ParseBlockStatment();

Statement* Parser::ParseExpressionStmt( Environment *env ) {
  ExpressionStatement *exprStmt = NULL;
  Token *start = mToks[0]; 
  Expression *expr = ParseExpression( LOWEST, env ); 
  if ( expr == NULL  )
    return NULL;

  exprStmt = new ExpressionStatement( expr, start );

  Token* end = Pop(); 
  if ( !Expect( end, SEMICOLON ))
    return NULL;

  return exprStmt;
} // Parser::ParseExpressionStmt()

Expression *Parser::ParseAssignExpr( Expression* left, Environment *env ) {
  Token *op = Pop();
  NextToken();
  Expression *rhs = ParseExpression( LOWEST, env );
  if ( rhs == NULL )
    return NULL;

  Expression *expr = new AssignmentExpr( op, left, rhs );
  return expr;
} // Parser::ParseAssignStmt()

Statement *Parser::ParseReturnStmt( Environment *env ) {
  Statement *stmt = NULL;
  Token* ret = Pop(); 
  NextToken();
  Expression *rhs = ParseExpression( LOWEST, env );
  stmt = new ReturnStmt( ret, rhs ); 
  Token* end = Pop();
  if ( !Expect( end, SEMICOLON ) )
    return NULL;

  return stmt;
} // Parser::ParseReturnStmt() 

FunctionDeclaration *Parser::ParseFunction( Environment *env, FunctionDeclaration *func ) {
  FunctionDeclaration *funcRes = func; 
  Environment* innerEnv = new Environment(); 
  innerEnv -> SetOuter( env );
  vector < Parameter *> prms = ParseParameter();

  if ( !Expect( mToks[0], RPAREN ) )
    return NULL;

  for ( int i = 0; i < prms.size(); ++i ) { 
    Obj* parameter_value = prms[i] -> Eval( innerEnv );
    funcRes -> Append( prms[i] );
  } // for

  // Skip )
  Pop();
  NextToken();

  BlockStatement* bstmt = NULL;
  bstmt = ParseBlockStatement( innerEnv );
  if ( bstmt == NULL )
    return NULL;

  Token *end = Pop();
  if ( !Expect( end, RBRACE ) ) 
    return NULL;

  funcRes -> SetEnv( innerEnv ); 
  funcRes -> SetBlock( bstmt );
  return funcRes;
} // Parser::ParseFunction 

Statement *Parser::ParseAssignStmt( Environment *env ) {
  DeclarationStatement *stmt = NULL;
  Token *type = Pop();
  stmt = new DeclarationStatement( type );

  NextToken();
  if ( !Expect( mToks[0], IDENT ) ) 
    return stmt;
  Token *id = Pop();
  stmt -> Append( id ); 

  NextToken();
  while ( !CurrentTokenIs( SEMICOLON ) ) {
    if ( !Expect( mToks[0], COMMA ) )
      return NULL;
    Pop();
    NextToken(); 

    if ( !Expect( mToks[0], IDENT ) )
      return NULL;
    id = Pop();
    stmt -> Append( id ); 
    NextToken();
  } // while

  Token* end = Pop();
  if ( !Expect( end, SEMICOLON ) ) 
    return NULL;

  return stmt;
} // Parser::ParseAssignStmt()


Statement *Parser::ParseLetStmt( Environment *env ) {
  DeclarationStatement *stmt = NULL;
  Token *type = Pop();
  stmt = new DeclarationStatement( type );

  NextToken();
  if ( !Expect( mToks[0], IDENT ) ) 
    return stmt;
  Token *id = Pop();
  stmt -> Append( id ); 

  NextToken();
  if ( CurrentTokenIs( LPAREN ) ) {
    SymbolExpression *ident = new SymbolExpression( id, id -> value );
    FunctionDeclaration *func = new FunctionDeclaration( type, ident );
    func = ParseFunction(env, func);
    return func;
  } // if

  while ( !CurrentTokenIs( SEMICOLON ) ) {
    if ( !Expect( mToks[0], COMMA ) )
      return NULL;
    Pop();
    NextToken(); 

    if ( !Expect( mToks[0], IDENT ) )
      return NULL;
    id = Pop();
    stmt -> Append( id ); 
    NextToken();
  } // while

  Token* end = Pop();
  if ( !Expect( end, SEMICOLON ) ) 
    return NULL;

  return stmt;
} // Parser::ParseLetStmt()

Statement *Parser::ParseStatement( Environment *env ) {
  Statement* stmt = NULL;

  if ( mToks[0] -> type == KEY_STRING || mToks[0] -> type == KEY_VOID ||
            mToks[0] -> type == KEY_INT || mToks[0] -> type == KEY_FLOAT || 
            mToks[0] -> type == KEY_BOOL || mToks[0] -> type == KEY_CHAR )
    stmt = ParseLetStmt( env );

  else if ( mToks[0] -> type == KEY_RETURN )
    stmt = ParseReturnStmt( env );

  else if ( mToks[0] -> type == LBRACE )
    stmt = ParseBlockStatement( env );

  else
    stmt = ParseExpressionStmt( env );

  if ( stmt == NULL )
    return NULL;

  return stmt;
} // Parser::ParseStatement()

Statement* Parser::ParseCompound( Environment *env ) {
  Statement* stmt = NULL;
  if ( mToks[0] -> type == KEY_STRING ||  mToks[0] -> type == KEY_INT || 
       mToks[0] -> type == KEY_FLOAT || mToks[0] -> type == KEY_BOOL || mToks[0] -> type == KEY_CHAR )
    stmt = ParseAssignStmt( env );

  else if ( mToks[0] -> type == KEY_RETURN )
    stmt = ParseReturnStmt( env );

  else
    stmt = ParseExpressionStmt( env );

  if ( stmt == NULL )
    return NULL;

  return stmt;
} // Parser::ParseCompound()

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
  while ( mCurToken->value != "quit" ) {

    while ( mCurToken-> type != EOFF  && mCurToken -> value != "quit" ) {
      Statement *stmt = ParseStatement( env );
      if ( stmt != NULL ) {
        stmt -> Print();
        program -> Append( stmt );
        obj = program -> Eval( env );
        if ( obj == NULL )
          Reset();

        mCurToken = mLexer -> ReadNextToken();
        if ( mCurToken -> type != EOFF )
          mToks.push_back( mCurToken ); 
      } // if

      else {
        if ( mErrs.size() > 0 )
          cout << mErrs[0];
        Reset();
      } // else 

      cout << "> ";
    } // while
    
    if ( mCurToken -> value != "quit" ) 
      Init( );
  } // while

  cout << "Program exits..." << endl; 
  return program;
} // Parser::ParseProgram()

// --------------- Main --------------------------
int main( ) {
  Parser *parser = new Parser( ) ;
  Program *program = parser->ParseProgram( ) ;
} // main()
