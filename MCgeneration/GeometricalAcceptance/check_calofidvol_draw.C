#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <iostream>

using namespace std;

int check()
{

  TFile *f = TFile::Open("tree.root");

  TTree *t = (TTree*)f->Get("tree");

  const int ndir=10;
  const char* dir[10] = {"xpos","xneg","ypos","yneg","zpos","zneg","xnegyneg","xposyneg","xnegypos","xposypos"};

  for(int idir=0; idir<ndir; idir++){
    TCanvas *c = new TCanvas( Form("c%s",dir[idir]),Form("c%s",dir[idir]) );
    t->Draw(
	       Form("calofidvol%sEntry[][2]:calofidvol%sEntry[][1]:calofidvol%sEntry[][0]",dir[idir],dir[idir],dir[idir]),
	       Form("mcTracklengthcalox0>0 && calofidvol%s",dir[idir]),
	       "");
      }

  return 0;




}
