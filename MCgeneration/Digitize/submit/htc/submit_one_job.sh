#!/bin/sh
#
USER=`whoami`

NAME=$1
INFILE=$2
WORKDIR=/storage/gpfs_ams/ams/users/vvagelli/HERD/analysis/herd-vv-svn/MCgeneration/Digitize
SUBMITDIR=${WORKDIR}/submit/htc

BASENAME=`basename $INFILE ".root"`
echo $BASENAME

JOBTEMPLATE=${SUBMITDIR}/job.dig.template
SUBTEMPLATE=${SUBMITDIR}/submit.template
DATACARDTEMPLATE=${SUBMITDIR}/digitize.eaconf.template

OUTDIR=${SUBMITDIR}/output/${NAME}
LOGDIR=${SUBMITDIR}/logs/${NAME}
JOBDIR=${SUBMITDIR}/jobs/${NAME}

if [ ! -d $OUTDIR ]; then mkdir -pv ${OUTDIR}; fi
if [ ! -d $LOGDIR ]; then mkdir -pv ${LOGDIR}; fi
if [ ! -d $JOBDIR ]; then mkdir -pv ${JOBDIR}; fi

JOBNAME="${BASENAME}"
OUTROOT=${OUTDIR}/${BASENAME}.dig.root

JOB=${JOBDIR}/${JOBNAME}.job
SUB=${JOBDIR}/${JOBNAME}.sub
ERRFILE=${LOGDIR}/${JOBNAME}.err
LOGFILE=${LOGDIR}/${JOBNAME}.log
OUTFILE=${LOGDIR}/${JOBNAME}.out
DATACARD=${JOBDIR}/${BASENAME}.eaconf
rm -fv ${ERRFILE}
rm -fv ${LOGFILE}

cp -v ${DATACARDTEMPLATE}                   ${DATACARD}
sed -i "s%_INFILE_%${INFILE}%g"             ${DATACARD}

cp -v ${JOBTEMPLATE}                        ${JOB}
sed -i "s%_DATACARD_%${DATACARD}%g"         ${JOB}
sed -i "s%_OUTROOT_%${OUTROOT}%g"           ${JOB}
chmod 777 ${JOB}

cp -v ${SUBTEMPLATE}                        ${SUB}
sed -i "s%_JOB_%${JOB}%g"                   ${SUB}
sed -i "s%_OUTPUT_%${OUTFILE}%g"            ${SUB}
sed -i "s%_ERROR_%${ERRFILE}%g"             ${SUB}
sed -i "s%_LOG_%${LOGFILE}%g"               ${SUB}

CMD="condor_submit -spool -name sn-01.cr.cnaf.infn.it ${SUB}"
echo ${CMD}
${CMD}

