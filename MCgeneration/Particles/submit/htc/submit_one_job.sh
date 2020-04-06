#!/bin/sh
#
USER=`whoami`

NAME="protons_sphere_10GeV_10000GeV_E-1"

WORKDIR=/storage/gpfs_ams/ams/users/vvagelli/HERD/analysis/herd-vv-svn/MCgeneration/Particles/
SUBMITDIR=${WORKDIR}/submit/htc

JOBTEMPLATE=${SUBMITDIR}/job.template
SUBTEMPLATE=${SUBMITDIR}/submit.template

OUTDIR=${SUBMITDIR}/output/${NAME}
LOGDIR=${SUBMITDIR}/logs/${NAME}
JOBDIR=${SUBMITDIR}/jobs/${NAME}

if [ ! -d $OUTDIR ]; then mkdir -pv ${OUTDIR}; fi
if [ ! -d $LOGDIR ]; then mkdir -pv ${LOGDIR}; fi
if [ ! -d $JOBDIR ]; then mkdir -pv ${JOBDIR}; fi

ii=$1

JOBNAME="job_${ii}"
JOB=${JOBDIR}/${JOBNAME}.job
SUB=${JOBDIR}/${JOBNAME}.sub
ERRFILE=${LOGDIR}/${JOBNAME}.err
LOGFILE=${LOGDIR}/${JOBNAME}.log
OUTFILE=${LOGDIR}/${JOBNAME}.out
rm -fv ${ERRFILE}
rm -fv ${LOGFILE}

GEOMETRY=${HERDINSTALL}/plugin/libHerdMCParametricGeo.so
GEODATACARD=${WORKDIR}/geometry.mac
DATACARD=${WORKDIR}/protons.mac
OUTPUT=${OUTDIR}/${NAME}_${ii}.root
SEED1="$RANDOM"
SEED2="$RANDOM"

cp -v ${JOBTEMPLATE}                        ${JOB}
sed -i "s%_JOBNAME_%${JOBNAME}%g"           ${JOB}
sed -i "s%_GEOMETRY_%${GEOMETRY}%g"         ${JOB}
sed -i "s%_GEODATACARD_%${GEODATACARD}%g"   ${JOB}
sed -i "s%_DATACARD_%${DATACARD}%g"         ${JOB}
sed -i "s%_OUTDIR_%${OUTDIR}%g"             ${JOB}
sed -i "s%_OUTPUT_%${OUTPUT}%g"             ${JOB}
sed -i "s%_SEED1_%${SEED1}%g"               ${JOB}
sed -i "s%_SEED2_%${SEED2}%g"               ${JOB}
chmod 777 ${JOB}

cp -v ${SUBTEMPLATE}                        ${SUB}
sed -i "s%_JOB_%${JOB}%g"                   ${SUB}
sed -i "s%_OUTPUT_%${OUTFILE}%g"            ${SUB}
sed -i "s%_ERROR_%${ERRFILE}%g"             ${SUB}
sed -i "s%_LOG_%${LOGFILE}%g"               ${SUB}

CMD="condor_submit -spool -name sn-01.cr.cnaf.infn.it ${SUB}"
echo ${CMD}
${CMD}

