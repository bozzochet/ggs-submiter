for x0 in 10 20 30 40 50 100
do
#    fname=data/electrons_10GeV_10000GeV_E-1.edepthreshold_0.020.tree.root
    fname=data/protons_10GeV_10000GeV_E-1.edepthreshold_0.020.tree.root
    root -q -b caloaxis.C++\(\"${fname}\",1,${x0}\)
#    mv -v fout.root fout.x0_${x0}.root
    mv -v fout.root fout.pro.x0_${x0}.root
#    mv -v Plots/Plots.pdf Plots/Plots.x0_${x0}.pdf
    mv -v Plots/Plots.pdf Plots/Plots.pro.x0_${x0}.pdf
done

exit

for t in 0.001 0.002 0.005 0.010 0.015 0.020 0.030 0.050 0.100
##for t in 0.002 0.005 0.015
do
#    fname=data/electrons_10GeV_10000GeV_E-1.edepthreshold_${t}.tree.root
     fname=data/protons_10GeV_10000GeV_E-1.edepthreshold_${t}.tree.root
     root -q -b caloaxis.C++\(\"${fname}\"\)
     mv -v fout.root fout.pro.edepthreshold_${t}.root
#    mv -v fout.root fout.edepthreshold_${t}.root
     mv -v Plots/Plots.pdf Plots/Plots.pro.edepthreshold_${t}.pdf
#    mv -v Plots/Plots.pdf Plots/Plots.edepthreshold_${t}.pdf
done


