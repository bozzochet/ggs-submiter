#!/usr/bin/env python
import os
import datetime

indir="/storage/gpfs_data/herd/vvagelliherd/herd-vv-svn/analysis/calo/submit/htc/output"

stream = ["protons_10GeV","protons_50GeV","protons_100GeV","protons_500GeV","protons_1TeV","protons_5TeV","protons_10TeV","protons_50TeV","protons_100TeV","protons_10GeV_1000GeV_E-1","electrons_10GeV_1000GeV_E-1","protons_1TeV_100TeV_E-1","electrons_1TeV_100TeV_E-1"]

for s in stream:
    
    runs = []
    i=0
    for f in os.listdir(indir+'/'+s):
        if f.endswith("tree.root"):
            runs.append(indir+'/'+s+'/'+f)
            i=i+1
            
    print("{}: {} runs in stream".format(s,i))

    outfile=indir+'/hadd/'+s+'.root'
    cmd="hadd {}".format(outfile)
    print("{}: {} output".format(s,outfile))

    for f in runs:
        cmd+=" {}".format(f)
    
    subfile = indir+'/hadd/'+s+'.sub'
    jobfile = indir+'/hadd/'+s+'.job'
    outfile = indir+'/hadd/'+s+'.out'
    logfile = indir+'/hadd/'+s+'.log'
    errfile = indir+'/hadd/'+s+'.err'

    with open(subfile, 'w') as fsub:
        fsub.write('universe = vanilla\n')
        fsub.write('executable = {}\n'.format(jobfile))
        fsub.write('output = {}\n'.format(outfile))
        fsub.write('error = {}\n'.format(errfile))
        fsub.write('log = {}\n'.format(logfile))
        fsub.write('ShouldTransferFiles = YES\n')
        fsub.write('WhenToTransferOutput = ON_EXIT\n') 
        fsub.write('queue 1\n')
        os.chmod(subfile, 0o777)

    with open(jobfile, 'w') as fjob:
        fjob.write('#!/bin/bash\n')
        fjob.write('source /storage/gpfs_data/herd/vvagelliherd/setcommon.sh\n')
        fjob.write('source /storage/gpfs_data/herd/vvagelliherd/setherd.sh\n')
        fjob.write('env\n')
        fjob.write('rm -f {}\n'.format(outfile))
        fjob.write('{}\n'.format(cmd))
        os.chmod(jobfile, 0o777)
        
    os.system('condor_submit -spool -name sn-01.cr.cnaf.infn.it {} -batch-name hadd.{}'.format(subfile,s))

