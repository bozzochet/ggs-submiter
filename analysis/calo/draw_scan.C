{

  const int nx0=6;
  const int x0[nx0] = {100,50,40,30,20,10};
  const int nth=9;
  const float th[nth] = {0.100, 0.050, 0.030, 0.020, 0.015, 0.010, 0.005, 0.002, 0.001};
  const int nene=9;
  const float enebins[10] = {10,21.5,46.4,100,215.4,464.2,1000,2154.4,4641.6,10000};
  const int icolor[11]  = { kRed+1, kRed-4, kRed-9, kMagenta-5, kBlue-7, kBlue, kCyan-6, kGreen-6, kGreen+1, kBlack, kBlack};
  //const int icolor[6]  = { kRed+3, kRed+1, kRed, kRed-7, kMagenta-7, kMagenta};
  
  TFile *fx0[nx0];
  TFile *fth[nth];
  for(int ix0=0; ix0<nx0; ix0++){
    //fx0[ix0] = TFile::Open( Form("fout.x0_%02d.root",x0[ix0]) );
    fx0[ix0] = TFile::Open( Form("fout.pro.x0_%02d.root",x0[ix0]) );
  }
  for(int ith=0; ith<nth; ith++){
    fth[ith] = TFile::Open( Form("fout.pro.edepthreshold_%.3f.root",th[ith]) );
    //fth[ith] = TFile::Open( Form("fout.edepthreshold_%.3f.root",th[ith]) );
  }
  
  TH1F *hdiffx0[nx0][nene];
  TH1F *hdiffth[nth][nene];
  TGraphErrors *gdiff68x0[nx0];
  TGraphErrors *gdiff68th[nth];
  TGraphErrors *gdiff95x0[nx0];
  TGraphErrors *gdiff95th[nth];

  for(int ix0=0; ix0<nx0; ix0++){
    gdiff68x0[ix0] = (TGraphErrors*)(fx0[ix0]->Get( Form("gdiff68") ) );
    gdiff95x0[ix0] = (TGraphErrors*)(fx0[ix0]->Get( Form("gdiff95") ) );
    for(int iene=0; iene<nene; iene++){
      hdiffx0[ix0][iene] = (TH1F*)(fx0[ix0]->Get( Form("hdiff_%d", iene) ));
      hdiffx0[ix0][iene]->SetName( Form("hdiffx0_%d_%d",ix0,iene) );
    }
  }
  for(int ith=0; ith<nth; ith++){
    gdiff68th[ith] = (TGraphErrors*)(fth[ith]->Get( Form("gdiff68") ) );
    gdiff95th[ith] = (TGraphErrors*)(fth[ith]->Get( Form("gdiff95") ) );
    for(int iene=0; iene<nene; iene++){
      hdiffth[ith][iene] = (TH1F*)(fth[ith]->Get( Form("hdiff_%d", iene) ));
      hdiffth[ith][iene]->SetName( Form("hdiffth_%d_%d",ith,iene) );
    }
  }
  
  TCanvas *chdiffx0[nene];
  for(int iene=0; iene<nene; iene++){
    chdiffx0[iene] = new TCanvas( Form("chdiffx0_%d",iene), Form("chdiffx0_%d",iene) ); chdiffx0[iene]->SetLogy();
    for(int ix0=0; ix0<nx0; ix0++){
      hdiffx0[ix0][iene]->SetTitle( Form("[%.1f - %.1f] GeV [MC]", enebins[iene], enebins[iene+1] ));
      hdiffx0[ix0][iene]->SetLineColor( icolor[ix0*2] );
      hdiffx0[ix0][iene]->DrawNormalized( Form("%s",ix0==0 ? "" : "same") );
    }
  }

  TCanvas *chdiffth[nene];
  for(int iene=0; iene<nene; iene++){
    chdiffth[iene] = new TCanvas( Form("chdiffth_%d",iene), Form("chdiffth_%d",iene) ); chdiffth[iene]->SetLogy();
    for(int ith=0; ith<nth; ith++){
      hdiffth[ith][iene]->SetTitle( Form("[%.1f - %.1f] GeV [MC]", enebins[iene], enebins[iene+1] ));
      hdiffth[ith][iene]->SetLineColor( icolor[ith] );
      hdiffth[ith][iene]->DrawNormalized( Form("%s",ith==0 ? "" : "same") );
    }
  }

  TCanvas *cangresx0 = new TCanvas("cangresx0","cangresx0"); cangresx0->cd(1)->SetLogx();  cangresx0->cd(1)->SetTicks();
  TH2F *h2 = new TH2F("h2",";Energy (GeV) [MC];Ang. Resolution (deg)",100,10,10000,100,0,30); h2->SetStats(0); h2->Draw("");
  for(int ix0=0; ix0<nx0; ix0++){
    gdiff68x0[ix0]->Draw("P"); gdiff68x0[ix0]->SetLineColor( icolor[ix0*2] ); gdiff68x0[ix0]->SetMarkerColor( icolor[ix0*2] );
    //gdiff95x0[ix0]->Draw("P"); gdiff95x0[ix0]->SetLineColor( icolor[ix0*2] ); gdiff95x0[ix0]->SetMarkerColor( icolor[ix0*2] ); gdiff95x0[ix0]->SetMarkerStyle( 21 );
  }
  
  TCanvas *cangresth = new TCanvas("cangresth","cangresth"); cangresth->cd(1)->SetLogx();  cangresth->cd(1)->SetTicks();
  h2->Draw("");
  for(int ith=0; ith<nth; ith++){
    gdiff68th[ith]->Draw("P"); gdiff68th[ith]->SetLineColor( icolor[ith] ); gdiff68th[ith]->SetMarkerColor( icolor[ith] ); 
    gdiff95th[ith]->Draw("P"); gdiff95th[ith]->SetLineColor( icolor[ith] ); gdiff95th[ith]->SetMarkerColor( icolor[ith] ); gdiff95th[ith]->SetMarkerStyle( 21 );
  }
  
}
      
  
