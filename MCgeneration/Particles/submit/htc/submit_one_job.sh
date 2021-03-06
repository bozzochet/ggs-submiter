#!/bin/sh
#
USER=`whoami`
WORKDIR=${STORAGE}/MoriStuff/install/ggs-submiter/MCgeneration/Particles/

NAME=$1
GEODATACARD=$2
if [ ! -f $GEODATACARD ]; then echo "Cannot find ${GEODATACARD}"; exit; fi

GEOMETRY=${DTPINSTALL}/lib/libTestGeometry.so
if [ ! -f $GEOMETRY ]; then echo "Cannot find ${GEOMETRY}"; exit; fi

DATACARD=${WORKDIR}/datacards/${NAME}.mac

SUBMITDIR=${WORKDIR}/submit/htc

JOBTEMPLATE=${SUBMITDIR}/job.template
SUBTEMPLATE=${SUBMITDIR}/submit.template

OUTDIR=${SUBMITDIR}/output/${NAME}
LOGDIR=${SUBMITDIR}/logs/${NAME}
JOBDIR=${SUBMITDIR}/jobs/${NAME}

if [ ! -d $OUTDIR ]; then mkdir -pv ${OUTDIR}; fi
if [ ! -d $LOGDIR ]; then mkdir -pv ${LOGDIR}; fi
if [ ! -d $JOBDIR ]; then mkdir -pv ${JOBDIR}; fi

SETENVCOMMON=${COMMONINSTALLDIR}/ggs-submiter/setcommon.sh
if [ ! -f $SETENVCOMMON ]; then echo "Cannot find ${SETENVCOMMON}"; exit; fi
SETENVHERD=${COMMONINSTALLDIR}/ggs-submiter/setenvHERD_master.sh
if [ ! -f $SETENVHERD ]; then echo "Cannot find ${SETENVHERD}"; exit; fi

ii=$3
SEED1="$RANDOM"
SEED2="$RANDOM"
JOBNAME="job_${ii}_${SEED1}_${SEED2}"
JOB=${JOBDIR}/${JOBNAME}.job
SUB=${JOBDIR}/${JOBNAME}.sub
ERRFILE=${LOGDIR}/${JOBNAME}.err
LOGFILE=${LOGDIR}/${JOBNAME}.log
OUTFILE=${LOGDIR}/${JOBNAME}.out
rm -fv ${ERRFILE}
rm -fv ${LOGFILE}


OUTPUT=${OUTDIR}/${NAME}_${ii}_${SEED1}_${SEED2}.root

cp -v ${JOBTEMPLATE}                        ${JOB}
sed -i "s%_JOBNAME_%${JOBNAME}%g"           ${JOB}
sed -i "s%_GEOMETRY_%${GEOMETRY}%g"         ${JOB}
sed -i "s%_GEODATACARD_%${GEODATACARD}%g"   ${JOB}
sed -i "s%_DATACARD_%${DATACARD}%g"         ${JOB}
sed -i "s%_OUTDIR_%${OUTDIR}%g"             ${JOB}
sed -i "s%_OUTPUT_%${OUTPUT}%g"             ${JOB}
sed -i "s%_SEED1_%${SEED1}%g"               ${JOB}
sed -i "s%_SEED2_%${SEED2}%g"               ${JOB}
sed -i "s%_SETENVCOMMON_%${SETENVCOMMON}%g" ${JOB}
sed -i "s%_SETENVHERD_%${SETENVHERD}%g"     ${JOB}
sed -i "s%_SUBMITDIR_%${SUBMITDIR}%g"       ${JOB}
chmod 777 ${JOB}

cp -v ${SUBTEMPLATE}                        ${SUB}
sed -i "s%_JOB_%${JOB}%g"                   ${SUB}
sed -i "s%_OUTPUT_%${OUTFILE}%g"            ${SUB}
sed -i "s%_ERROR_%${ERRFILE}%g"             ${SUB}
sed -i "s%_LOG_%${LOGFILE}%g"               ${SUB}

CMD="condor_submit -spool -name sn-01.cr.cnaf.infn.it ${SUB} -batch-name ggs.${NAME}"
echo ${CMD}
${CMD}

