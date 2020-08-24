DIR=${STORAGE}/herd-vv-svn/MCgeneration/Particles/submit/htc/output
#NAME="protons_10GeV_10000GeV_E-1"
#NAME="electrons_1000GeV_10000GeV_E-1"
#NAME="protons_1TeV"

#for NAME in "protons_10GeV" "protons_50GeV" "protons_100GeV" "protons_500GeV" "protons_1TeV" "protons_5TeV" "protons_10TeV" "protons_50TeV" "protons_100TeV" "protons_10GeV_1000GeV_E-1" "protons_1TeV_100TeV_E-1" "electrons_10GeV_1000GeV_E-1" "electrons_1TeV_100TeV_E-1"

#for NAME in "protons_10GeV_1000GeV_E-1" "protons_1TeV_100TeV_E-1" "electrons_10GeV_1000GeV_E-1" "electrons_1TeV_100TeV_E-1"
#for NAME in "protons_1TeV_100TeV_E-1" "electrons_1TeV_100TeV_E-1"
for NAME in "protons_100TeV"

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
	if [ -f $OUTFILE ]; then
	    echo "${BASENAME} already digitized"
	else
	    echo "$f" >> $LISTFILE
	fi
    done

    N=`cat ${LISTFILE} | wc -l`
    echo "$N files in $LISTFILE to be produced"
done
