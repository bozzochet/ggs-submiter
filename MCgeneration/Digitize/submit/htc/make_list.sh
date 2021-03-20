DIR=${STORAGE}/herd-vv-svn/MCgeneration/Particles/submit/htc/output
check_already_digitized=0

for NAME in "electrons_10GeV_1000GeV_E-1_2828_caloonly" "electrons_10GeV_1000GeV_E-1_3030_caloonly"

#for NAME in "electrons_10GeV_1000GeV_E-1"

do
    LISTNAME=${NAME}.list
    LISTFILE=lists/${LISTNAME}

    if [ -f $LISTFILE ]; then
	mv -v $LISTFILE $LISTFILE.bak
    fi

    for f in `ls ${DIR}/${NAME}/*.root`
    do
	BASENAME=`basename $f .root`
	OUTFILE="output/$NAME/$BASENAME.dig.root"
	#echo $OUTFILE
	if [ ${check_already_digitized} -eq 1 ]; then
	    if [ -f $OUTFILE ]; then
		echo "${BASENAME} already digitized"
	    else
		echo "$f" >> $LISTFILE
	    fi
	else
	    echo "$f" >> $LISTFILE
	fi
	
    done

    N=`cat ${LISTFILE} | wc -l`
    echo "$N files in $LISTFILE to be produced"
done
