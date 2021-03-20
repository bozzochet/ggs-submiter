#!/bin/sh
#
USER=`whoami`

WORKDIR=${STORAGE}/herd-vv-svn/MCgeneration/Digitize
SUBMITDIR=${WORKDIR}/submit/htc

argv=("$@")
argc=${#argv[@]}

NAME=${argv[0]}    
#NAME=${NAME}_calopddig

JOBTEMPLATE=${SUBMITDIR}/job.dig.template
JOBTEMPLATEHEAD=${SUBMITDIR}/job.dig.head.template
JOBTEMPLATEBODY=${SUBMITDIR}/job.dig.body.template
JOBTEMPLATETAIL=${SUBMITDIR}/job.dig.tail.template
SUBTEMPLATE=${SUBMITDIR}/submit.template
#DATACARDTEMPLATE=${SUBMITDIR}/digitize.calov2_fit_psdbar.eaconf.template
#DATACARDTEMPLATE=${SUBMITDIR}/digitize.calov2_stk_psdbar.eaconf.template
#DATACARDTEMPLATE=${SUBMITDIR}/digitize.calov2_calopddig_stk_psdbar.eaconf.template
DATACARDTEMPLATE=${argv[1]}
if [ ! -f $DATACARDTEMPLATE ]; then echo "DATACARD ${DATACARDTEMPLATE} does not exist"; exit; fi

OUTDIR=${SUBMITDIR}/output/${NAME}
LOGDIR=${SUBMITDIR}/logs/${NAME}
JOBDIR=${SUBMITDIR}/jobs/${NAME}

if [ ! -d $OUTDIR ]; then mkdir -pv ${OUTDIR}; fi
if [ ! -d $LOGDIR ]; then mkdir -pv ${LOGDIR}; fi
if [ ! -d $JOBDIR ]; then mkdir -pv ${JOBDIR}; fi

UNIQUE=`echo -n "${argv[1]}" | cksum | awk '{print $1}'`
JOBNAME="${NAME}_${UNIQUE}"

JOB=${JOBDIR}/${JOBNAME}.job
if [ -f $JOB ]; then rm -f $JOB; fi

SUB=${JOBDIR}/${JOBNAME}.sub
ERRFILE=${LOGDIR}/${JOBNAME}.err
LOGFILE=${LOGDIR}/${JOBNAME}.log
OUTFILE=${LOGDIR}/${JOBNAME}.out
rm -fv ${ERRFILE}
rm -fv ${LOGFILE}

echo $INFILE

#head -n 15 ${JOBTEMPLATE} >> ${JOB}
cat ${JOBTEMPLATEHEAD} >> ${JOB}

for INFILE in "${argv[@]:2}"; do   #loop on array of arguments starting from second

echo $INFILE
BASENAME=`basename $INFILE ".root"`
echo $BASENAME

rm -fv job.temp
#head -n 27 ${JOBTEMPLATE} | tail -n 13 >> job.temp
cat ${JOBTEMPLATEBODY} >> ${JOB}

DATACARD=${JOBDIR}/${BASENAME}.eaconf
cp -v ${DATACARDTEMPLATE}                   ${DATACARD}
sed -i "s%_INFILE_%${INFILE}%g"             ${DATACARD}

OUTROOT=${OUTDIR}/${BASENAME}.dig.root
#sed -i "s%_DATACARD_%${DATACARD}%g"         job.temp
#sed -i "s%_OUTROOT_%${OUTROOT}%g"           job.temp
sed -i "s%_DATACARD_%${DATACARD}%g"         ${JOB}
sed -i "s%_OUTROOT_%${OUTROOT}%g"           ${JOB}

#cat job.temp >> ${JOB}

done

#tail -n 6 ${JOBTEMPLATE} >> ${JOB}
cat ${JOBTEMPLATETAIL} >> ${JOB}
chmod 777 ${JOB}

cp -v ${SUBTEMPLATE}                        ${SUB}
sed -i "s%_JOB_%${JOB}%g"                   ${SUB}
sed -i "s%_OUTPUT_%${OUTFILE}%g"            ${SUB}
sed -i "s%_ERROR_%${ERRFILE}%g"             ${SUB}
sed -i "s%_LOG_%${LOGFILE}%g"               ${SUB}

CMD="condor_submit -spool -name sn-01.cr.cnaf.infn.it ${SUB} -batch-name dig.${NAME}"
echo ${CMD}
#cat ${JOB}
${CMD}

