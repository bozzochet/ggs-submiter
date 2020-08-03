#!/bin/sh
#
USER=`whoami`

NAME=$1
LISTFILE=$2
OPTS=$3

WORKDIR=/storage/gpfs_ams/ams/users/vvagelli/HERD/analysis/herd-vv-svn/analysis/calo
SUBMITDIR=${WORKDIR}/submit/htc

FULLBASENAME=`basename $LISTFILE .list.txt`
echo $FULLBASENAME
BASENAME=${FULLBASENAME%??????} #remove last 6 trailing characters

JOBTEMPLATE=${SUBMITDIR}/job.template
SUBTEMPLATE=${SUBMITDIR}/submit.template
#DATACARD=${SUBMITDIR}/analysis.$BASENAME.eaconf
DATACARD=${SUBMITDIR}/analysis.generic.eaconf
LIST=${LISTFILE}

if [ ! -f $JOBTEMPLATE ]; then echo "$JOBTEMPLATE not found"; exit; fi
if [ ! -f $SUBTEMPLATE ]; then echo "$SUBTEMPLATE not found"; exit; fi
if [ ! -f $DATACARD ];    then echo "$DATACARD    not found"; exit; fi
if [ ! -f $LIST ];        then echo "$LIST        not found"; exit; fi

OUTDIR=${SUBMITDIR}/output/${BASENAME}
LOGDIR=${SUBMITDIR}/logs/${BASENAME}
JOBDIR=${SUBMITDIR}/jobs/${BASENAME}

if [ ! -d $OUTDIR ]; then mkdir -pv ${OUTDIR}; fi
if [ ! -d $LOGDIR ]; then mkdir -pv ${LOGDIR}; fi
if [ ! -d $JOBDIR ]; then mkdir -pv ${JOBDIR}; fi

JOBNAME="${NAME}"

JOB=${JOBDIR}/${JOBNAME}.job
SUB=${JOBDIR}/${JOBNAME}.sub
ERRFILE=${LOGDIR}/${JOBNAME}.err
LOGFILE=${LOGDIR}/${JOBNAME}.log
OUTFILE=${LOGDIR}/${JOBNAME}.out
rm -fv ${ERRFILE}
rm -fv ${LOGFILE}

cp -v ${DATACARD} ${SUBMITDIR}/output/${BASENAME}/

cp -v ${JOBTEMPLATE}                        ${JOB}
sed -i "s%_DATACARD_%${DATACARD}%g"         ${JOB}
sed -i "s%_OUTDIR_%${OUTDIR}%g"             ${JOB}
sed -i "s%_INFILE_%${LIST}%g"               ${JOB}
sed -i "s%_OPTS_%${OPTS}%g"                 ${JOB}
sed -i "s%_NAME_%${NAME}%g"                 ${JOB}

chmod 777 ${JOB}

cp -v ${SUBTEMPLATE}                        ${SUB}
sed -i "s%_JOB_%${JOB}%g"                   ${SUB}
sed -i "s%_OUTPUT_%${OUTFILE}%g"            ${SUB}
sed -i "s%_ERROR_%${ERRFILE}%g"             ${SUB}
sed -i "s%_LOG_%${LOGFILE}%g"               ${SUB}

CMD="condor_submit -spool -name sn-01.cr.cnaf.infn.it ${SUB}"
echo ${CMD}
${CMD}

