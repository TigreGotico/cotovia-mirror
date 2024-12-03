/*
Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ramón Piñeiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
           1996-2012 Centro Ramon Pineiro para a Investigación en Humanidades,
                     Xunta de Galicia, Santiago de Compostela, Spain

License: GPL-3.0+
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 .
 On Debian systems, the complete text of the GNU General
 Public License version 3 can be found in /usr/share/common-licenses/GPL-3.

______________________________________________________________________________*/
 
 
#ifndef LETRAS_H
   #define LETRAS_H

extern unsigned char to_minuscula[256];


#define dixito(c) ( c>47 && c<58 )
#define caracter_de_formato(a) (a==' ' || a=='\n' || a=='\t' || a=='\r' || a=='\f')
#if defined(_GENERA_CORPUS) || defined(_CORPUS_PROSODICO)
#define signo_fin_frase(c) (c=='.'||c=='\n'||c=='\r'||c==0||c=='?'||c=='!')
#else
#ifdef _NO_RETORNOS
#define signo_fin_frase(c) (c=='.'||c==';'/*||c==':'*/||c==0||c=='?'||c=='!')
#else
#define signo_fin_frase(c) (c=='.'||c==';'/*||c==':'*/||c=='\n'||c=='\r'||c==0||c=='?'||c=='!')
#endif
#endif

#define vocal_minuscula_acentuada(c) (c==a_acentuada ||c==e_acentuada ||c==i_acentuada ||c==o_acentuada ||c==u_acentuada)
#define vocal_maiuscula_acentuada(c) (c==A_acentuada ||c==E_acentuada ||c==I_acentuada ||c==O_acentuada ||c==U_acentuada)
#define vocal_acentuada(p) (vocal_minuscula_acentuada(p) || vocal_maiuscula_acentuada(p) )

#define vocal(p) ( p=='a'                            || \
                   p=='e'                            || \
                   p=='i'                            || \
                   p=='o'                            || \
                   p=='u'                            ||  \
                   p=='A'                            ||  \
                   p=='E'                            ||  \
                   p=='I'                            || \
                   p=='O'                            || \
                   p=='U'                            || \
                   vocal_minuscula_acentuada(p)      ||  \
                   vocal_maiuscula_acentuada(p)      || \
                   vocal_minuscula_rara(p)      ||  \
                   vocal_maiuscula_rara(p)      || \
                   p==(unsigned char)U_con_dierese   || \
                   p==(unsigned char)I_con_dierese   || \
                  p==(unsigned char)E_con_dierese ||   \
                  p==(unsigned char)e_con_dierese  ||  \
                   p==(unsigned char)u_con_dierese   || \
                   p==(unsigned char)i_con_dierese )

#define vocal_minuscula_rara(p) ( p == (unsigned char)'â' || p == (unsigned char)'ê' || p == (unsigned char)'î' || p == (unsigned char)'ô' || p ==(unsigned char)'û' || \
																	p == (unsigned char)'à' || p == (unsigned char)'è' || p == (unsigned char)'ì' || p == (unsigned char)'ò' || p ==(unsigned char)'ù' || \
																	p == (unsigned char)'ä' || p == (unsigned char)'ö' )

#define vocal_maiuscula_rara(p) ( p == (unsigned char)'Â' || p == (unsigned char)'Ê' || p == (unsigned char)'Î' || p == (unsigned char)'Ô' || p ==(unsigned char)'Û' || \
																	p == (unsigned char)'À' || p == (unsigned char)'È' || p == (unsigned char)'Ì' || p == (unsigned char)'Ò' || p ==(unsigned char)'Ù' || \
																	p == (unsigned char)'Ä' || p == (unsigned char)'Ö' )
																	


#define letra(p) (  (p>64 && p<91)  ||   \
                     (p>96 && p<123) || \
                  vocal_minuscula_acentuada(p)      || \
                  vocal_maiuscula_acentuada(p)      || \
                   vocal_minuscula_rara(p)      ||  \
                   vocal_maiuscula_rara(p)      || \
                  p==(unsigned char)E_con_dierese ||   \
                  p==(unsigned char)e_con_dierese  ||  \
                  p==(unsigned char)U_con_dierese ||   \
                  p==(unsigned char)u_con_dierese  ||  \
                  p==(unsigned char)I_con_dierese ||   \
                  p==(unsigned char)i_con_dierese  ||  \
                  p==(unsigned char)enne_minuscula  || \
                  p==(unsigned char)enne_maiuscula   ||\
                  p==(unsigned char)cedilla_minuscula ||\
                  p==(unsigned char)cedilla_maiuscula)
#define consonante(p) (letra(p) && !vocal(p))

#define maiuscula(p) ((p>64 && p<91) ||\
                  vocal_maiuscula_acentuada(p)||\
                   vocal_maiuscula_rara(p)      || \
				          p==(unsigned char)U_con_dierese ||\
				          p==(unsigned char)I_con_dierese ||\
				          p==(unsigned char)E_con_dierese ||\
				          p==(unsigned char)enne_maiuscula ||\
				          p==(unsigned char)cedilla_maiuscula)

#define caracter_especial(c) ( \
 /*	c=='%' ||*/ c=='{' || c=='}' || c=='+' || c=='=' || \
	c=='$' || c=='>' || c=='<' || c=='#' || c=='&' || \
	c=='*' || c=='@' || c=='\\' || c=='^' || c=='_' || c=='|' || c=='~' || \
	c=='£' || c=='¥' || c=='®' || /*c==simbolo_de_grados ||*/ c=='±' || \
	c=='²' || c=='³' || c=='µ' || c=='½' || c=='¼')

#define signo(c)( c=='%' || \
		c==' ' || c==',' ||	c=='.' || c==':' || c==';' ||	c=='\n' || 	c=='\r' ||\
		c=='\t' || c=='-' || c=='\f' ||	c=='#' ||c=='/' ||c=='`' || 	c==(unsigned char)'´' || \
		c=='\'' || c=='(' ||c==')' || c=='\"' || c=='[' || c==']' ||c==apertura_exclamacion || \
		c=='!' || c==apertura_interrogacion  || c=='?' || c=='_' || c=='%' || c==simbolo_ordinal_masculino ||\
		c== 0 || c==equival_apert_comillas  || c==equival_cerr_comillas || c==simbolo_ordinal_femenino || \
		c==simbolo_de_grados || c== 255 || c==(unsigned char)'·') //el ultimo es para puntos suspensivos

#define minuscula(p) ((p>96 && p<123) ||\
                      vocal_minuscula_acentuada(p)|| \
                   vocal_minuscula_rara(p)      ||  \
                      p==(unsigned char)enne_minuscula  || \
                      p==(unsigned char)cedilla_minuscula ||\
                      p==(unsigned char)e_con_dierese  ||  \
                      p==(unsigned char)u_con_dierese  ||  \
                      p==(unsigned char)i_con_dierese  )

#define to_minusculas(p) (to_minuscula[(unsigned char)p])

#define vocal_feble(c) (\
c=='I' || c=='U' || c=='i' || c=='u' ||\
 c==(unsigned char)U_con_dierese || c==(unsigned char)u_con_dierese)


#define vocal_aberta(c) (\
		c=='A' || c=='a' || c=='E' ||	c=='e' || c=='O' || c=='o' ||\
		c==(unsigned char)A_acentuada || c==(unsigned char)a_acentuada ||\
    c==(unsigned char)E_acentuada || c==(unsigned char)e_acentuada ||\
    c==(unsigned char)O_acentuada || c==(unsigned char)o_acentuada )

// Fran Campillo: Representación de la ruptura entonativa: Puede ser cualquiera,
// pero debe ir precedido de una coma.
#define SIMBOLO_RUPTURA_ENTONATIVA	",#R-E#"
// Fran Campillo: Representación de aquellas comas que se realizan mediante una
// ruptura entonativa sin pausa.
#define SIMBOLO_RUPTURA_COMA		",#R-C#"

void pasar_a_minusculas(const t_palabra_proc entrada,t_palabra_proc saida);
int palabra_formada_por_solo_consonantes( char *orixinal);
#endif
 

