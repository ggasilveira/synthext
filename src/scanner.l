%{
#include "y.tab.h"
%}

%option nounistd
%option never-interactive

			note [A-H] | "Mb" | "H"
			pause [a-h]
			space " "
			othervowel [iouIOU]
			otherconsonant [jklmnpqrstvwxyzJKLMNPQRSTVWXYZ]
			
%%

"A" {return A;}
"B" {return B;}
"C" {return C;}
"D" {return D;}
"E" {return E;}
"F" {return F;}
"G" {return G;}
"H" {return B_FLAT;}
"Mb" {return E_FLAT;}

{pause} {return PAUSE;}
[ \t] /* skip single spaces */
"  " {return DOUBLE_VOLUME;}
"!" {yylval.instrument = 24; return CHANGE_INSTRUMENT;}
[iouIOU] {yylval.instrument = 110; return CHANGE_INSTRUMENT;}
{otherconsonant} {return REPEAT_OR_PAUSE;}
[02468] {yylval.amount = atoi(yytext); return MOVE_INSTRUMENT;}
"?"|"." {return INCREASE_OCTAVE;}
"\n" {return VOICE_END;}
[1369]|";" {yylval.instrument = 15; return CHANGE_INSTRUMENT;}
"," {yylval.instrument = 114; return CHANGE_INSTRUMENT;}

. {return REPEAT_OR_PAUSE;}
