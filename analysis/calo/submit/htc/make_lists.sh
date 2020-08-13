DIGDIR=${STORAGE}/herd-vv-svn/MCgeneration/Digitize/submit/htc/output
#STREAM="protons_10GeV"
FILEPERLIST=20

mkdir lists
for STREAM in "protons_10GeV" "protons_50GeV" "protons_100GeV" "protons_500GeV" "protons_1TeV" "protons_5TeV" "protons_10TeV" "protons_50TeV" "protons_10GeV_1000GeV_E-1" "electrons_10GeV_1000GeV_E-1" "protons_100TeV"
do
    DIGLIST=lists/${STREAM}.dig.list.txt
    LISTDIR=lists/${STREAM}

    rm -r $LISTDIR
    mkdir -v $LISTDIR
    
    rm -fv $DIGLIST
    ls $DIGDIR/$STREAM/*.root >> $DIGLIST


    split --lines $FILEPERLIST --numeric-suffixes --suffix-length=5 --additional-suffix=.list.txt $DIGLIST $STREAM.
    mv -v $STREAM.*.list.txt lists/$STREAM
done

