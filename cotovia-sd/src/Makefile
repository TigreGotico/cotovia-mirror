OBJS = cotovia.o module_utils.o
INCLUDE = -I/usr/include/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
spdlibdir = /usr/lib/speech-dispatcher
CFLAGS =  $(INCLUDE) -O2
LDFLAGS = -Wl,-rpath,$(spdlibdir)
sd_cotovia_LDADD =  -lpthread -ldotconf -lglib-2.0 -pthread -lgthread-2.0 

sd_cotovia: $(OBJS) 
	gcc -Wall $(CFLAGS) -o sd_cotovia $(OBJS)  $(spdlibdir)/libsdaudio.so.2 $(sd_cotovia_LDADD) $(LDFLAGS)
%.o:%.c
	gcc $(CFLAGS) -c $< -o $@
clean:
	rm -fr *.o *.dep sd_cotovia

