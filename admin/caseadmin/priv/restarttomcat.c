#include <unistd.h>
int main(int argc,char **argv) {
   char *newargv[] = { "/bin/sh", "/etc/init.d/tomcat6","restart" , NULL };
   char *newenviron[] = { NULL };
   execve("/bin/sh", newargv, newenviron);
}
