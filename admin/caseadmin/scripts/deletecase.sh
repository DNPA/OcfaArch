#! /bin/bash
#
# Shell script that deletes all references to a case
#
#
if [ -z $1 ] 
    then echo usage: $0 \<case name\> 
    exit -1
fi
OCFAUSER=ocfa.ocfa
OCFAVAR=/var/ocfa
CASEVAR=$OCFAVAR/cases/$1
CASEHTTPDDIR=$OCFAVAR/httpd
CASEQUEUE=$OCFAVAR/queues/$1
CASE_APACHE_CONF_FILE=$CASEHTTPDDIR/$1.conf
CARVFSDD=$CASEVAR/carvfs/mnt/0/CarvFS.crv
if [ -f "$CARVFSDD" ]
then
    echo "ERROR: can't delete case while CarvFs is still mounted for case."
    exit -2
fi
echo are your sure you want to delete database $1 on host $PGHOST? \(y/N\)
read CONFIRM
if [ $CONFIRM = 'y' ] 
    then dropdb $1
    echo database deleted
fi
echo are you sure you want to delete directory $CASEVAR \(y/N\)
read CONFIRM
if [ $CONFIRM = 'y' ] 
    then rm -rf $CASEVAR
    echo $CASEVAR deleted
fi
echo removing $CASEQUEUE
rm -r $CASEQUEUE
echo removing $CASE_APACHE_CONF_FILE
rm -r $CASE_APACHE_CONF_FILE
echo restart apache for all changes to take effect
