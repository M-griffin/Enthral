


bin_PROGRAMS = enthral
enthral_SOURCES = configbbs.cpp conio.cpp console.cpp data_areas.cpp dtfunc.cpp \
	enthral.cpp file_area.cpp file_edit.cpp file_list.cpp files.cpp history.cpp \
	language.cpp logon.cpp main_system.cpp menu_func.cpp msg_area.cpp \
	msg_edit.cpp msg_email.cpp msg_fse.cpp msg_newscan.cpp msg_pack.cpp \
	msg_quote.cpp msg_read.cpp msg_readll.cpp msg_stats.cpp msg_title.cpp msgs.cpp \
	node.cpp user_list.cpp users.cpp \
	mb_jam.cpp mb_api.cpp pyenthral.cpp


# set the include path found by configure
INCLUDES = -I$(top_srcdir)/src $(all_includes) $(PY_CFLAGS)

# the library search path.
enthral_LDFLAGS = $(all_libraries) 

# sudo apt-get install libssl-dev || add -lssl 
# new boost libs added.
enthral_LDADD =  -lcrypto -lpthread -lboost_system -lboost_filesystem jamlib/jamlib.a $(PY_LIBS)

noinst_HEADERS = configbbs.h conio.h console.h \
	data_areas.h dtfunc.h file_area.h file_edit.h file_list.h files.h history.h \
	language.h logon.h main_system.h menu_func.h msg_email.h msg_fse.h \
	msg_pack.h msg_read.h msg_readll.h msg_stats.h msg_title.h \
	msgs.h node.h stamp.h struct.h user_list.h users.h mb_jam.h \
	mb_api.h jam.h pyenthral.h

