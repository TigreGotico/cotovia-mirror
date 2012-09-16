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
 
/* deste xeito podemos utilizar graficamente no programa igual sen depender
	 de con qué editor foi feito o arquivo de entrada (*.TXT) se co editor de
	 windows ou co de ms_dos_ . Hai que indicar no include
	 do principal o que se axuste co editor*/

 

#define OEM


/*# define '!'  33
# define '"'  34
# define '#'  35
# define '$'  36
# define '%'  37
# define '&'  38
# define '''  39
# define '('  40               difiren solamente a partir
# define ')'  41                do 128
# define '*'  42
# define '+'  43            */

# define  comillas_simples 39
# define  comillas_dobles  34
# define  equival_apert_comillas 174
# define  equival_cerr_comillas  175



# define cedilla_maiuscula          128
# define u_con_dierese              129
# define e_acentuada                130
/*# define car_'â' 131
# define car_'ä' 132
# define car_'à' 133
# define car_'å' 134   */
# define cedilla_minuscula          135
/*# define car_'ê' 136
# define car_'ë' 137
# define car_'è' 138   */
# define i_con_dierese              139
/*# define car_'î' 140
# define car_'ì' 141
# define car_'Ä' 142
# define car_'Å' 143  */
# define E_acentuada                144
/*# define car_'æ' 145
# define car_'Æ' 146
# define car_'ô' 147
# define car_'ö' 148
# define car_'ò' 149
# define car_'û' 150
# define car_'ù' 151
# define car_'ÿ' 152
# define car_'Ö' 153  */
# define U_con_dierese              154
/*# define car_'¢' 155
# define car_'¥' 157
# define car_'¤' 158
# define car_'-' 159  */
# define a_acentuada                160
# define i_acentuada 								161
# define o_acentuada 								162
# define u_acentuada                163
# define enne_minuscula              164
# define enne_maiuscula              165
# define simbolo_ordinal_femenino                  166
# define simbolo_ordinal_masculino                 167
# define apertura_interrogacion     168
/*# define car_'¨' 169
# define car_'¬' 170   */
# define un_medio                   171
# define un_cuarto                  172
# define apertura_exclamacion       173
/*# define car_'«' 174
# define car_'¯' 175
# define car_'_' 176
# define car_'ÿ' 177
# define car_'Ö' 178
# define car_'þ' 179   */
# define o_cubo 180
# define A_acentuada								 181
/*# define car_'µ' 182
# define car_'¸' 183
# define car_'¹' 184
# define car_' ' 185
# define car_'¦  186
# define car_'©' 187
# define car_'­' 188
# define »          189
*/
/*
# define car_'¾' 190
# define car_'À' 191
# define car_'Á' 192
# define car_'Â' 193
# define car_'Ã' 194
# define car_'È' 195
# define car_'ý' 196
# define car_'Ê' 197
# define car_'Ë' 198
# define car_'Ì' 199
# define car_'®' 200
# define car_'Ð' 201
# define car_'×' 202
# define car_'Ý' 203
# define car_'Þ' 204
# define car_'Í' 205
# define car_'ð' 206
# define car_'Î' 207
# define car_'Ï' 208
# define car_'Ò' 209
# define car_'Ó' 210
# define car_'Ô' 211
# define car_'Õ' 212
# define car_'Ø' 213   */
# define I_acentuada                214
/*# define car_'Ú' 215   */
# define I_con_dierese                    216
/*# define car_'ã' 217
# define car_'õ' 218
# define car_'s' 219
# define car_'>' 220
# define car_'o' 221
# define car_'Ø' 222
# define car_'×' 223 */
# define O_acentuada                224
# define Beta                       225
/*# define car_',' 226
# define car_'ƒ' 227
# define car_',' 228
# define car_'_' 229
# define car_'²' 230
# define car_'³' 231
# define car_'ê' 232   */
# define U_acentuada                233
/*# define car_'S' 234
# define car_'<' 235
# define car_'O' 236
# define car_'ì' 237
# define car_'Ä' 238
# define car_'Å' 239
# define car_'¶' 240
# define car_'±' 241
# define car_'ø' 242 */
# define tres_cuartos                243
/*# define car_'Y' 244
# define car_'§' 245
# define car_'÷' 246
# define car_'ò' 247   */
# define simbolo_de_grados                      248

/*
# define car_'·' 249
# define car_'"' 250
# define car_'"' 251
# define car_''' 252
# define car_'²' 253
# define car_'`' 254
# define car_'É' 255    */
 

