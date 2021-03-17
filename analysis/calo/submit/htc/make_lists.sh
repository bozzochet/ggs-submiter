DIGDIR=${STORAGE}/herd-vv-svn/MCgeneration/Digitize/submit/htc/output
OUTDIR=output.allcalo.dig
#STREAM="protons_10GeV"
#FILEPERLIST=50
FILEPERLIST=2

CHECK_FOR_EXISTING=0

mkdir lists
#for STREAM in "protons_10GeV" "protons_50GeV" "protons_100GeV" "protons_500GeV" "protons_1TeV" "protons_5TeV" "protons_10TeV" "protons_50TeV" "protons_100TeV" "protons_10GeV_1000GeV_E-1" "electrons_10GeV_1000GeV_E-1"
#for STREAM in "protons_1TeV" "protons_5TeV" "protons_10TeV" "protons_50TeV" "protons_100TeV" #"protons_1TeV_100TeV_E-1" "electrons_1TeV_100TeV_E-1"
#for STREAM in "electrons_10GeV_1000GeV_E-1_calopddig" "protons_10GeV_1000GeV_E-1_calopddig"
for STREAM in "protons_10GeV_1000GeV_E-1_calopddig"
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

    #ls $DIGDIR/$STREAM/*.root >> $DIGLIST

    split --lines $FILEPERLIST --numeric-suffixes --suffix-length=5 --additional-suffix=.list.txt $DIGLIST $STREAM.
    mv -v $STREAM.*.list.txt $LISTDIR
done

