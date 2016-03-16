// S-Expr Copyright Robert L. Read, 2016
// Published under GPL.
// Eventually this may turn into an Arduino library that I publish. Right now it is a draft.

/*

Our basic goal is to write in a tiny Arduino-compatible style a function like:

parse("(m 4 5 6)") that will return
a cons-like tree representing the value: 

(cons m (cons 4 (cons 5 (cons 6 nil)))) 

(although of course that will be represent as a structure in C.

Our main goal her is to read such expressions on the serial port and drive functions
in the C object space.  So this expressions could more or less easily turn into a
call (although dyanamic) like:
m(4,5,6),

where m is a function defined in the C space.

So we want a function "invoke", and if you called 

invoke("(m 4 5 6)") 
it would call m(4,5,6).

So to do a good job with this what would Kent Beck do?

Write the test first!  Although testing on a Arduino is a painful.

Note: At first I will support only strings and integers --- not "symbols".

Our basic alphabet of functions and types:

strings 
int
nil
atom: string | int | nil
sexpr : atom | cons sexpr sexpr

functions:

Cons
car 
cdr
nil
null
list
nth 
delete
lex

But do I really have a need for anyting that can't be handled as a one-level list with the lexer?

I think I probably don't need to continue work on this file until my use-cases sharpen.
 */

#include "limits.h"
#include "S-Expr.h"

// sexpr NIL = { .tp = NIL_T, .car = NULL, .cdr = NULL };

// sexpr NIL = *cons_nil();

boolean test_equal1() {
  sexpr* b = cons(cons_int(4),cons(cons_int(5),cons(cons_int(6),cons_nil())));
  boolean p =  equal(b,b);
  del(b);
  return p;
}

boolean test_equal2() {
  TOKEN tk = getToken((char *) "methyl",0);
  if (tk.terminator != 6) return false;
  sexpr* b = cons_string(tk);
  sexpr* a = cons_string(tk);
  boolean p =  equal(a,b);
  del(a);
  del(b);
  return p;
}

boolean test_equal3() {
  sexpr* a = cons_nil();
  sexpr* b = cons_nil();
  boolean p =  equal(a,b);
  del(a);
  del(b);
  return p;
}
boolean test_equal4() {
  sexpr* b = cons_int(37);
  sexpr* a = cons_int(37);
  boolean p =  equal(a,b); 
  del(a);
  del(b);
  return p;
}

boolean test_equal5() {
  TOKEN tk = getToken((char *) "m",0);  
  sexpr* b = cons(cons_string(tk),cons(cons_int(4),cons(cons_int(5),cons(cons_int(6),cons_nil()))));
  sexpr* a = parse((char *) "(m 4 5 6)");
  boolean p =  equal(b,b);
  //  print_sexpr(a);
  //  print_sexpr(b);
  boolean q =  equal(a,b);
  del(a);
  del(b);
  return p && q;
}

boolean test_parse1() {
  TOKEN tk = getToken((char *) "methyl",0);
    
  sexpr* a = parse((char *) "methyl");
  sexpr* b = cons_string(tk);
 
  boolean p =  equal(a,b);
  del(a);
  del(b);
  
  return p;
}

boolean test_parse2() {
  sexpr* a = parse((char *) "4");
  sexpr* b = cons_int(4);
  boolean p =  equal(a,b); 
  del(a);
  del(b);
  return p;
}

boolean test_getToken() {
  TOKEN tk0 = getToken((char *) "(m)",0);
  TOKEN tk1 = getToken((char *) "(m)",1);
  TOKEN tk2 = getToken((char *) "(m)",2);
  return (tk0.terminator == 1) && (tk1.terminator == 2) && (tk2.terminator == 3);
}

boolean test_parse3() {
  sexpr* a = parse((char *) "(m)");
  TOKEN tk = getToken((char *) "m",0);
  sexpr* b = cons(cons_string(tk),cons_nil());
  boolean p =  equal(a,b);
  del(b);
  del(a);
 return p;
}

boolean test_parse4() {
  sexpr* a = parse((char *) "400");
  sexpr* b = cons_int(400);
  boolean p =  equal(a,b); 
  del(a);
  del(b);
  return p;
}

boolean test_parse5() {
  sexpr* a = parse((char *) "(2 400)");
  sexpr* b = cons(cons_int(2),cons(cons_int(400),cons_nil()));
  boolean p =  equal(a,b); 
  del(a);
  del(b);
  return p;
}

boolean test_value_i_0() {
  sexpr* a = cons_int(400);
  String str = print_as_String(a);
  int n = value_i(a);
  boolean p = (n == 400);
  del(a);
  return p;
}

boolean test_parse10() {
  sexpr* a = parse((char *) "((4))");
  del(a);
  return true;
}

boolean test_parse11() {
  sexpr* a = parse((char *) "((4) 2)");
  // canon_print_sexpr(a);
  //  print_sexpr(a);
  del(a);
  return true;
}

boolean test_parse12() {
  TOKEN tk = getToken((char *) "m",0);
  sexpr* b = cons(cons_string(tk),cons(cons_int(4),cons(cons_int(5),cons(cons_int(6),cons_nil()))));
  sexpr* a = parse((char *) "(m 4 5 6)");
  //  print_sexpr(b);
  //  print_sexpr(a);
  boolean p =  equal(a,b);
  del(a);
  del(b);
  return p;
}


boolean test_tokenize() {
  char s[] = "(A)";

  return TokenEquals_string(getToken(s,0),"(")
    &&  TokenEquals_string(getToken(s,1),"A")
    && TokenEquals_string(getToken(s,2),")");
}

boolean test_print1() {
  /* char const * spudlist  = "tram"; */
  /* sexpr* s = parse((char *) spudlist); */
  /* String p = print_as_String(s); */
  /* Serial.println(p); */

  char const * list  = "(spud)";
  sexpr* a = parse((char *) list);
		 
  String pa = print_as_String(a);
  return pa.equals("(spud)");
}

boolean test_loop() {
  const int n = 3;
  char const * ss[n] = { "(((3) (4)) 5)",
		   "(spud (4))",
		   "(sum (4 3) (3 2))"
  };

  for (int i = 0; i < n; i++) {
    sexpr* s = parse((char *) ss[i]);
    String p = print_as_String(s);
    if (!p.equals(ss[i])) {
      Serial.println("RED:");
      Serial.println(ss[i]);
      Serial.println(p);
    }
    del(s);
  }
  return true;
}

boolean test_nth() {
  TOKEN tk = getToken((char *) "m",0);
  sexpr* a = parse((char *) "(m 4 5 6)");
  return !(
     equal(nth(a,0),cons_string(tk))
        && equal(nth(a,1),cons_int(4))
        && equal(nth(a,2),cons_int(5))
        &&
	   equal(nth(a,4),(sexpr*) cons_nil()))
    ;
  del(a);
}

boolean test_length() {
   sexpr* a = parse((char *) "(m 4 5 (6))");
   int lena = s_length(a);
   sexpr* b = parse((char *) "(m)");
   sexpr* c = parse((char *) "()");   
   int lenb = s_length(b);
   int lenc = s_length(c);
   del(a);
   del(b);
   del(c);   
   return (lena == 4) && (lenb == 1) && (lenc == 0);
}

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

boolean test_delete() {
  int f = freeRam();
  for(int i = 0; i < 50; i++) {
    sexpr* a = parse((char *) "(m 4 5 (6))");
    del(a);
  }
  int g = freeRam();
  return (f == g);
}

void setup()
{
  // It is possible that this is a real problem for battery powered operation!!!!
  Serial.begin(9600);  // Begin the serial monitor at 9600bps
  Serial.println("Serial port ready!");
}

void loop()
{

  Serial.print("test_equal1: ");
  Serial.println(test_equal1() ? "GREEN" : "RED");

  Serial.print("test_tokenize: ");
  Serial.println(test_tokenize() ? "GREEN" : "RED");
  
  Serial.print("test_parse1: ");
  Serial.println(test_parse1() ? "GREEN" : "RED");

  Serial.print("test_parse2: ");
  Serial.println(test_parse2() ? "GREEN" : "RED");

  Serial.print("test_getToken: ");
  Serial.println(test_getToken() ? "GREEN" : "RED");

  Serial.print("test_parse3: ");
  Serial.println(test_parse3() ? "GREEN" : "RED");

  Serial.print("test_parse4: ");
  Serial.println(test_parse4() ? "GREEN" : "RED");

  Serial.print("test_value_i_0: ");
  Serial.println(test_value_i_0() ? "GREEN" : "RED");
  

  Serial.print("test_parse5: ");
  Serial.println(test_parse5() ? "GREEN" : "RED");

  Serial.print("test_equal2: ");
  Serial.println(test_equal2() ? "GREEN" : "RED");

  Serial.print("test_equal3: ");
  Serial.println(test_equal3() ? "GREEN" : "RED");

  Serial.print("test_equal4: ");
  Serial.println(test_equal4() ? "GREEN" : "RED");

  Serial.print("test10: ");
  Serial.println(test_parse10() ? "GREEN" : "RED");

  Serial.print("test_parse11: ");
  Serial.println(test_parse11() ? "GREEN" : "RED");

  Serial.print("test_print1: ");
  Serial.println(test_print1() ? "GREEN" : "RED");

  Serial.print("test_loop: ");
  Serial.println(test_loop() ? "GREEN" : "RED");

  Serial.print("test_equal5: ");
  Serial.println(test_equal5() ? "GREEN" : "RED");

  Serial.print("test12: ");
  Serial.println(test_parse12() ? "GREEN" : "RED");


  Serial.print("test_nth: ");
  Serial.println(test_nth() ? "GREEN" : "RED");

  Serial.print("test_delete: ");
  Serial.println(test_delete() ? "GREEN" : "RED");

  delay(5000);
}
 