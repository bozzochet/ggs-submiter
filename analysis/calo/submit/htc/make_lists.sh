DIGDIR="/home/HERD/vvagelliherd/storage/herd-vv-svn/MCgeneration/Digitize/submit/htc/output"
STREAM="electrons_10GeV_1000GeV_E-1"

DIGLIST=lists/${STREAM}.dig.list.txt
LISTDIR=lists/${STREAM}
rm -r $LISTDIR
mkdir -v lists/$STREAM

ls $DIGDIR/$STREAM/*.root >> $DIGLIST

FILEPERLIST=2
split --lines $FILEPERLIST --numeric-suffixes --suffix-length=5 --additional-suffix=.list.txt $DIGLIST $STREAM.
mv -v $STREAM.*.list.txt lists/$STREAM


