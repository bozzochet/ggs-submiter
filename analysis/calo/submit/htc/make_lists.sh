#STREAM="protons_10GeV_10000GeV_E-1"
STREAM="electrons_1000GeV_10000GeV_E-1"
mkdir -v lists/$STREAM

#LIST="lists/protons_10GeV_10000GeV_E-1.dig.list.txt"
LIST="lists/electrons_1000GeV_10000GeV_E-1.dig.list.txt"
FILEPERLIST=20

split --lines $FILEPERLIST --numeric-suffixes --suffix-length=5 --additional-suffix=.list.txt $LIST $STREAM.
mv -v $STREAM.*.list.txt lists/$STREAM


