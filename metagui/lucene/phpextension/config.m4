PHP_ARG_ENABLE(ocfalucene, whether to enable ocfalucene support,
[ --enable-ocfalucene Enable OcfaLucene support] )

dnl  if test "$PHP_OCFALUCENE" = "yes"; then
   AC_DEFINE(HAVE_OCFALUCENE, 1, [Whether you have ocfalucene])
   PHP_NEW_EXTENSION(ocfalucene, src/lucenewrapper.c, $ext_shared)
   PHP_ADD_INCLUDE(/usr/local/digiwash/inc)
   PHP_ADD_INCLUDE(/usr/include/pgsql)
   PHP_ADD_LIBRARY_WITH_PATH(ocfaquerylucene, /usr/local/digiwash/lib) 
    PHP_ADD_LIBRARY_WITH_PATH(ocfamisc, /usr/local/digiwash/lib) 
    PHP_ADD_LIBRARY_WITH_PATH(ocfastore, /usr/local/digiwash/lib) 
    PHP_ADD_LIBRARY_WITH_PATH(ocfaevidence, /usr/local/digiwash/lib) 
    PHP_ADD_LIBRARY_WITH_PATH(ocfaevent, /usr/local/digiwash/lib) 
    PHP_ADD_LIBRARY_WITH_PATH(ocfamessage, /usr/local/digiwash/lib) 
    PHP_ADD_LIBRARY_WITH_PATH(ocfaevent, /usr/local/digiwash/lib) 

dnl fi
