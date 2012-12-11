/*

cotovia.c: Speech Dispatcher backend for Cotovia TTS

Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ramón Piñeiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Based on  Speech Dispatcher generic output module (generic.c).

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


#include <glib.h>

#include "fdset.h"

#include "module_utils.h"
#include "module_utils_addvoice.c"

#define MODULE_NAME     "cotovia"
#define MODULE_VERSION  "0.1"

DECLARE_DEBUG()

/* Thread and process control */
static int cotovia_speaking = 0;

static pthread_t cotovia_speak_thread;
static pid_t cotovia_pid;
static sem_t *cotovia_semaphore;

static char **cotovia_message;
static EMessageType cotovia_message_type;

static int cotovia_position = 0;
static int cotovia_pause_requested = 0;

static char *execute_synth_str1;
static char *execute_synth_str2;

/* Internal functions prototypes */
static void* _cotovia_speak(void*);
static void _cotovia_child(TModuleDoublePipe dpipe, const size_t maxlen);
static void cotovia_child_close(TModuleDoublePipe dpipe);

void cotovia_set_rate(signed int rate);
void cotovia_set_pitch(signed int pitch);
void cotovia_set_voice(EVoiceType voice);
void cotovia_set_language(char* language);
void cotovia_set_volume(signed int volume);

/* Fill the module_info structure with pointers to this modules functions */

MOD_OPTION_1_STR(CotoviaExecuteSynth)
MOD_OPTION_1_INT(CotoviaMaxChunkLength)
MOD_OPTION_1_STR(CotoviaDelimiters)
MOD_OPTION_1_STR(CotoviaStripPunctChars)
MOD_OPTION_1_STR(CotoviaRecodeFallback)

MOD_OPTION_1_INT(CotoviaRateAdd)
MOD_OPTION_1_FLOAT(CotoviaRateMultiply)
MOD_OPTION_1_INT(CotoviaRateForceInteger)
MOD_OPTION_1_INT(CotoviaPitchAdd)
MOD_OPTION_1_FLOAT(CotoviaPitchMultiply)
MOD_OPTION_1_INT(CotoviaPitchForceInteger)
MOD_OPTION_1_INT(CotoviaVolumeAdd)
MOD_OPTION_1_FLOAT(CotoviaVolumeMultiply)
MOD_OPTION_1_INT(CotoviaVolumeForceInteger)
MOD_OPTION_3_HT(CotoviaLanguage, code, name, charset)

static char cotovia_msg_pitch_str[16];
static char cotovia_msg_rate_str[16];
static char cotovia_msg_volume_str[16];
static char* cotovia_msg_voice_str = NULL;
static TCotoviaLanguage* cotovia_msg_language = NULL;
static VoiceDescription **list_koto_voices;
static int num_voices = 0;

/* Public functions */
int
module_load(void)
{

    INIT_SETTINGS_TABLES();

    MOD_OPTION_1_STR_REG(CotoviaExecuteSynth, "");

    REGISTER_DEBUG();

    MOD_OPTION_1_INT_REG(CotoviaMaxChunkLength, 300);
    MOD_OPTION_1_STR_REG(CotoviaDelimiters, ".");
    MOD_OPTION_1_STR_REG(CotoviaStripPunctChars, "");
    MOD_OPTION_1_STR_REG(CotoviaRecodeFallback, "?"); //UTF-8 string to use in place of character not present in the target encoding

    MOD_OPTION_1_INT_REG(CotoviaRateAdd, 0);
    MOD_OPTION_1_FLOAT_REG(CotoviaRateMultiply, 1);
    MOD_OPTION_1_INT_REG(CotoviaRateForceInteger, 0);

    MOD_OPTION_1_INT_REG(CotoviaPitchAdd, 0);
    MOD_OPTION_1_FLOAT_REG(CotoviaPitchMultiply, 1);
    MOD_OPTION_1_INT_REG(CotoviaPitchForceInteger, 0);

    MOD_OPTION_1_INT_REG(CotoviaVolumeAdd, 0);
    MOD_OPTION_1_FLOAT_REG(CotoviaVolumeMultiply, 1);
    MOD_OPTION_1_INT_REG(CotoviaVolumeForceInteger, 0);

    MOD_OPTION_HT_REG(CotoviaLanguage);
  
    module_register_settings_voices();

    return 0;
}

int
module_init(char **status_info)
{
    int ret;

    *status_info = NULL;

    DBG("CotoviaMaxChunkLength = %d\n", CotoviaMaxChunkLength);
    DBG("CotoviaDelimiters = %s\n", CotoviaDelimiters);
    DBG("CotoviaExecuteSynth = %s\n", CotoviaExecuteSynth);

    cotovia_msg_language = (TCotoviaLanguage*) xmalloc(sizeof(TCotoviaLanguage));
    cotovia_msg_language->code = strdup("gl");
    cotovia_msg_language->charset = strdup("iso-8859-1");
    cotovia_msg_language->name = strdup("gl");    

    cotovia_message = malloc (sizeof (char*));    
    cotovia_semaphore = module_semaphore_init();

    DBG("Cotovia: creating new thread for cotovia_speak\n");
    cotovia_speaking = 0;
    ret = pthread_create(&cotovia_speak_thread, NULL, _cotovia_speak, NULL);
    if(ret != 0){
        DBG("Cotovia: thread failed\n");
	*status_info = strdup("The module couldn't initialize threads"
			      "This can be either an internal problem or an"
			      "architecture problem. If you are sure your architecture"
			      "supports threads, please report a bug.");
        return -1;
    }
								
    *status_info = strdup("Everything ok so far.");
    return 0;
}

int
module_audio_init(char **status_info){
  status_info = NULL;
  return 0;
}


void set_koto_voice (gpointer key, gpointer value, gpointer i) {
  SPDVoiceDef *voices = (SPDVoiceDef*) value;
  int j = num_voices; 

  if(voices->male1 != NULL) {
    list_koto_voices[j] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    list_koto_voices[j]->language = (char*)key;
    list_koto_voices[j]->name = voices->male1;
    j++;
  }

  if(voices->male2 != NULL) {
    list_koto_voices[j] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    list_koto_voices[j]->language = (char*)key; 
    list_koto_voices[j]->name = voices->male2;
    j++;
  }
  
  if(voices->male3 != NULL) {
    list_koto_voices[j] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    list_koto_voices[j]->language = (char*)key; 
    list_koto_voices[j]->name = voices->male3;
    j++;
  }

  if(voices->female1 != NULL) {
    list_koto_voices[j] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    list_koto_voices[j]->language = (char*)key; 
    list_koto_voices[j]->name = voices->female1;
    j++;
  }

  if(voices->female2 != NULL) {
    list_koto_voices[j] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    list_koto_voices[j]->language = (char*)key; 
    list_koto_voices[j]->name = voices->female2;
    j++;
  }

  if(voices->female3 != NULL) {
    list_koto_voices[j] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    list_koto_voices[j]->language = (char*)key; 
    list_koto_voices[j]->name = voices->female3;
    j++;
  }

  if(voices->child_female != NULL) {
    list_koto_voices[j] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    list_koto_voices[j]->language = (char*)key; 
    list_koto_voices[j]->name = voices->child_female;
    j++;
  }

  if(voices->child_male != NULL) {
    list_koto_voices[j] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    list_koto_voices[j]->language = (char*)key; 
    list_koto_voices[j]->name = voices->child_male;
    j++;
  }
  
  num_voices = j;
}


VoiceDescription** module_list_voices(void) {
  
  VoiceDescription **result;
  int Vnum = 0;
  int i;

  Vnum = g_hash_table_size(module_voice_table);
  Vnum = Vnum*8;

  list_koto_voices = (VoiceDescription**) malloc((Vnum)*sizeof(VoiceDescription*));  
  g_hash_table_foreach (module_voice_table, set_koto_voice, NULL);

  result = (VoiceDescription**) malloc((num_voices + 1)*sizeof(VoiceDescription*));  
  for (i=0; i<num_voices; i++) {
    result[i] = (VoiceDescription*) malloc(sizeof(VoiceDescription));
    result[i]->name = list_koto_voices[i]->name;
    result[i]->language = list_koto_voices[i]->language;
    result[i]->dialect = NULL;
  }
  result[num_voices] = NULL;
   
  free(list_koto_voices);

  return result;
}


int
module_speak(gchar *data, size_t bytes, EMessageType msgtype)
{
    char *tmp;

    DBG("speak()\n");

    if (cotovia_speaking){
        DBG("Speaking when requested to write");
        return 0;
    }


    if(module_write_data_ok(data) != 0) return -1;

    UPDATE_STRING_PARAMETER(language, cotovia_set_language);
    UPDATE_PARAMETER(voice, cotovia_set_voice);
    UPDATE_PARAMETER(pitch, cotovia_set_pitch);
    UPDATE_PARAMETER(rate, cotovia_set_rate);
    UPDATE_PARAMETER(volume, cotovia_set_volume);

    /* Set the appropriate charset */
    assert(cotovia_msg_language != NULL);
    if (cotovia_msg_language->charset != NULL){
	DBG("Recoding from UTF-8 to %s...", cotovia_msg_language->charset);
	tmp = 
	    (char*) g_convert_with_fallback(data, bytes, cotovia_msg_language->charset,
					    "UTF-8", CotoviaRecodeFallback, NULL, NULL,
					    NULL);
    }else{
	DBG("Warning: Prefered charset not specified, recoding to iso-8859-1");
	tmp = 
	    (char*) g_convert_with_fallback(data, bytes, "iso-8859-1",
					    "UTF-8", CotoviaRecodeFallback, NULL, NULL,
					    NULL);
    }

    if (tmp == NULL) return -1;

    if (msgtype == MSGTYPE_TEXT)
      *cotovia_message = module_strip_ssml(tmp);
    else
      *cotovia_message = strdup(tmp);
    xfree(tmp);

    module_strip_punctuation_some(*cotovia_message, CotoviaStripPunctChars);

    cotovia_message_type = MSGTYPE_TEXT;

    DBG("Requested data: |%s|\n", data);
	
    /* Send semaphore signal to the speaking thread */
    cotovia_speaking = 1;    
    sem_post(cotovia_semaphore);    
		
    DBG("Cotovia: leaving write() normaly\n\r");
    return bytes;
}

int
module_stop(void) 
{
    DBG("cotovia: stop()\n");

    if(cotovia_speaking && cotovia_pid != 0){
        DBG("cotovia: stopping process group pid %d\n", cotovia_pid);
        kill(-cotovia_pid, SIGKILL);
    }
    return 0;
}

size_t
module_pause(void)
{
    DBG("pause requested\n");
    if(cotovia_speaking){
        DBG("Sending request to pause to child\n");
        cotovia_pause_requested = 1;

        DBG("paused at byte: %d", cotovia_position);
        return 0;        
    }else{
        return -1;
    }
}

char*
module_is_speaking(void)
{
    return NULL ; 
}

void
module_close(int status)
{
    DBG("cotovia: close()\n");

    if(cotovia_speaking){
        module_stop();
    }

    if (module_terminate_thread(cotovia_speak_thread) != 0)
        exit(1);

    exit(status);
}


/* Internal functions */

/* Replace all occurances of 'token' in 'sting'
   with 'data' */
char*
string_replace(char *string, char* token, char* data)
{
    char *p;
    char *str1;
    char *str2;
    char *new;
    char *mstring;

    mstring = g_strdup(string);
    while (1){
      /* Split the string in two parts, ommit the token */
      p = strstr(mstring, token);
      if (p == NULL){
	return mstring;
      }
      *p = 0;

      str1 = mstring;
      str2 = p + (strlen(token));        

      /* Put it together, replacing token with data */
      new = g_strdup_printf("%s%s%s", str1, data, str2);
      xfree(mstring);
      mstring = new;
    }

}

void*
_cotovia_speak(void* nothing)
{	
    TModuleDoublePipe module_pipe;
    int ret;
    int status;

    DBG("cotovia: speaking thread starting.......\n");

    set_speaking_thread_parameters();

    while(1){        
        sem_wait(cotovia_semaphore);
        DBG("Semaphore on\n");

        ret = pipe(module_pipe.pc);
        if (ret != 0){
            DBG("Can't create pipe pc\n");
            cotovia_speaking = 0;
            continue;
        }

        ret = pipe(module_pipe.cp);
        if (ret != 0){
            DBG("Can't create pipe cp\n");
            close(module_pipe.pc[0]);     close(module_pipe.pc[1]);
            cotovia_speaking = 0;
            continue;
        }

	module_report_event_begin();

        /* Create a new process so that we could send it signals */
        cotovia_pid = fork();

        switch(cotovia_pid){
        case -1:	
            DBG("Can't say the message. fork() failed!\n");
            close(module_pipe.pc[0]);     close(module_pipe.pc[1]);
            close(module_pipe.cp[0]);     close(module_pipe.cp[1]);
            cotovia_speaking = 0;
            continue;

        case 0:
            {
		char *e_string;
		char *p;	       
	
		/* Set this process as a process group leader (so that SIGKILL
		   is also delivered to the child processes created by system()) */
		if (setpgid(0,0) == -1) DBG("Can't set myself as project group leader!");

		e_string = strdup(CotoviaExecuteSynth);

		e_string = string_replace(e_string, "$PITCH", cotovia_msg_pitch_str);
		e_string = string_replace(e_string, "$RATE", cotovia_msg_rate_str);
		e_string = string_replace(e_string, "$VOLUME", cotovia_msg_volume_str);
	
		e_string = string_replace(e_string, "$LANGUAGE", cotovia_msg_language->name);	
		if (cotovia_msg_voice_str != NULL)
		    e_string = string_replace(e_string, "$VOICE", cotovia_msg_voice_str);
		else
		    e_string = string_replace(e_string, "$VOICE", "no_voice");

		/* Cut it into two strings */           
		p = strstr(e_string, "$DATA");
		if (p == NULL) exit(1);
		*p = 0;
		execute_synth_str1 = strdup(e_string);
		execute_synth_str2 = strdup(p + (strlen("$DATA")));
		
		free(e_string);
		
		/* execute_synth_str1 se sem musi nejak dostat */
		DBG("Starting child...\n");
		_cotovia_child(module_pipe, CotoviaMaxChunkLength);
            }
            break;

        default:
            /* This is the parent. Send data to the child. */

            cotovia_position = module_parent_wfork(module_pipe, *cotovia_message,
                                                   cotovia_message_type,
                                                   CotoviaMaxChunkLength, CotoviaDelimiters,
                                                   &cotovia_pause_requested);

            DBG("Waiting for child...");
            waitpid(cotovia_pid, &status, 0); 
            cotovia_speaking = 0;

	    // Report CANCEL if the process was signal-terminated
	    // and END if it terminated normally
	    if (WIFSIGNALED(status)) module_report_event_stop();
	    else module_report_event_end();

            DBG("child terminated -: status:%d signal?:%d signal number:%d.\n",
                WIFEXITED(status), WIFSIGNALED(status), WTERMSIG(status));
        }        
    }

    cotovia_speaking = 0;

    DBG("cotovia: speaking thread ended.......\n");    

    pthread_exit(NULL);
}	

void
_cotovia_child(TModuleDoublePipe dpipe, const size_t maxlen)
{
    char *text;  
    sigset_t some_signals;
    int bytes;
    char *command;
    GString *message;
    int i;
    int ret;

    sigfillset(&some_signals);
    module_sigunblockusr(&some_signals);

    module_child_dp_init(dpipe);

    DBG("Entering child loop\n");
    while(1){
        /* Read the waiting data */
        text = malloc((maxlen + 1) * sizeof(char));
        bytes = module_child_dp_read(dpipe, text, maxlen);
        DBG("read %d bytes in child", bytes);
        if (bytes == 0){
            free(text);
            cotovia_child_close(dpipe);
        }

        text[bytes] = 0;
        DBG("text read is: |%s|\n", text);

        /* Escape any quotes */
        message = g_string_new("");
        for(i=0; i<=bytes-1; i++){
            if (text[i] == '\'')
                message = g_string_append(message, "'\\''");
	    else{
                g_string_append_printf(message, "%c", text[i]);
            }
        }

        DBG("child: escaped text is |%s|", message->str);

        command = malloc((strlen(message->str)+strlen(execute_synth_str1)+
                          strlen(execute_synth_str2) + 8) * sizeof(char));

        if (strlen(message->str) != 0){
            sprintf(command, "%s%s%s", execute_synth_str1, message->str, execute_synth_str2);

            DBG("child: synth command = |%s|", command);

            DBG("Speaking in child...");
            module_sigblockusr(&some_signals);        
            {
               ret = system(command);
	       DBG("Executed shell command returned with %d", ret);
            }
        }
        module_sigunblockusr(&some_signals);        

        xfree(command);
        xfree(text);
        g_string_free(message, 1);

        DBG("child->parent: ok, send more data");      
        module_child_dp_write(dpipe, "C", 1);
    }
}

static void
cotovia_child_close(TModuleDoublePipe dpipe)
{   
    DBG("child: Pipe closed, exiting, closing pipes..\n");
    module_child_dp_close(dpipe);          
    DBG("Child ended...\n");
    exit(0);
}

void
cotovia_set_pitch(int pitch)
{
    float hpitch;

    hpitch = (((float) pitch) + CotoviaPitchAdd) * CotoviaPitchMultiply;
    if (!CotoviaPitchForceInteger){
	snprintf(cotovia_msg_pitch_str, 15, "%.2f", hpitch);
    }else{
	snprintf(cotovia_msg_pitch_str, 15, "%d", (int) hpitch);
    }
}

void
cotovia_set_rate(int rate)
{
    float hrate;

    hrate = (((float) rate) + CotoviaRateAdd) * CotoviaRateMultiply ;
    if (hrate < 10) 
      hrate = 10;
    if (!CotoviaRateForceInteger){
	snprintf(cotovia_msg_rate_str, 15, "%.2f", hrate);
    }else{
	snprintf(cotovia_msg_rate_str, 15, "%d", (int) hrate);
    }
}

void
cotovia_set_volume(int volume)
{
    float hvolume;

    DBG("Volume: %d", volume);

    hvolume = ((float) volume) * CotoviaVolumeMultiply + CotoviaVolumeAdd;
    DBG("HVolume: %f", hvolume);
    if (!CotoviaVolumeForceInteger){
	snprintf(cotovia_msg_volume_str, 15, "%.2f", hvolume);
    }else{
	snprintf(cotovia_msg_volume_str, 15, "%d", (int) hvolume);
    }
}

void
cotovia_set_language(char *lang)
{

    cotovia_msg_language = (TCotoviaLanguage*) module_get_ht_option(CotoviaLanguage,
							lang);
    if (cotovia_msg_language == NULL){
	DBG("Language %s not found in the configuration file, using defaults.", lang);
	cotovia_msg_language = (TCotoviaLanguage*) xmalloc(sizeof(TCotoviaLanguage));
	cotovia_msg_language->code = strdup(lang);
	cotovia_msg_language->charset = NULL;
	cotovia_msg_language->name = strdup(lang);
    }

    if (cotovia_msg_language->name == NULL){
	DBG("Language name for %s not found in the configuration file.", lang);
	cotovia_msg_language = (TCotoviaLanguage*) xmalloc(sizeof(TCotoviaLanguage));
	cotovia_msg_language->code = strdup("gl");
	cotovia_msg_language->charset = strdup("iso-8859-1");
	cotovia_msg_language->name = strdup("gl");
    }
    
    cotovia_set_voice(msg_settings.voice);
}

void
cotovia_set_voice(EVoiceType voice)
{
  if (voice != 0) {
    assert(cotovia_msg_language);
    cotovia_msg_voice_str = module_getvoice(cotovia_msg_language->code, voice);
    if (cotovia_msg_voice_str == NULL){
	DBG("Invalid voice type specified or no voice available!");
    }	
  }
  else // Voice is not a simbolic name (male1, female2 ...)
    cotovia_msg_voice_str = (char*)msg_settings.synthesis_voice;
}


#include "module_main.c"
