DIGDIR=${STORAGE}/herd-vv-svn/MCgeneration/Digitize/submit/htc/output
OUTDIR=output.caloonly
FILEPERLIST=20

CHECK_FOR_EXISTING=0

mkdir lists

for STREAM in "electrons_10GeV_1000GeV_E-1_3030_caloonly" "electrons_10GeV_1000GeV_E-1_2828_caloonly"
do
    STREAM="${STREAM}"

    DIGLIST=lists/${STREAM}.dig.list.txt
    LISTDIR=lists/${STREAM}

    rm -r $LISTDIR
    mkdir -v $LISTDIR
    
    rm -fv $DIGLIST

    for f in `ls $DIGDIR/$STREAM/*.root`
    do
        BASENAME=`basename $f .dig.root`
        OUTFILE="${OUTDIR}/$STREAM/$BASENAME.tree.root"
	if [ $CHECK_FOR_EXISTING -eq 1 ]; then
	    if [ -f $OUTFILE ]; then
		echo "${BASENAME} already digitized"
            else
		echo "$f" >> $DIGLIST
            fi
	else
	    echo "$f" >> $DIGLIST
	fi

    done

    split --lines $FILEPERLIST --numeric-suffixes --suffix-length=5 --additional-suffix=.list.txt $DIGLIST $STREAM.
    mv -v $STREAM.*.list.txt $LISTDIR
done

