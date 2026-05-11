%{
#include "y.tab.h"
%}

%option nounistd
%option never-interactive

	    		note [A-G] | [A-G]b | [A-G]"#" | "Mb" | "H"
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

"Ab" {return A_FLAT;}
"Bb" {return B_FLAT;}
"Cb" {return C_FLAT;}
"Db" {return D_FLAT;}
"Eb" {return E_FLAT;}
"Fb" {return F_FLAT;}
"Gb" {return G_FLAT;}
"H"  {return B_FLAT;}
"Mb" {return E_FLAT;}


"A#" {return A_SHARP;}
"B#" {return B_SHARP;}
"C#" {return C_SHARP;}
"D#" {return D_SHARP;}
"E#" {return E_SHARP;}
"F#" {return F_SHARP;}
"G#" {return G_SHARP;}

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
