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
CASEREPDIR=$CASEVAR/repository
CASEWORKDIR=$CASEVAR/work
CASEINDEXDIR=$CASEVAR/index
CASEHASHSETDIR=$CASEVAR/hashsets
CASEPPQDIR=$OCFAVAR/queues/$1
CARVFSDD=$CASEVAR/carvfs/mnt/0/CarvFS.crv
if [ -f "$CARVFSDD" ]
then
    echo "ERROR: can't cleanup case while CarvFs is still mounted for case."
    exit -2
fi
echo are your sure you want to cleanup $1?  \(y/N\)\(all data will be deleted\)
read CONFIRM
if [ $CONFIRM = 'y' ] 
then $OCFAROOT/bin/cleanupdb.sh $1	
    "rm" -rf $CASEREPDIR/*
    "rm" -rf $CASEHASHSETDIR/*
    "rm" /usr/local/digiwash/log/*
    "rm" -rf $CASEWORKDIR/*
    "rm" -rf $CASEINDEXDIR/*
fi
"rm" -rf $CASEPPQDIR/*
