/*Header-MicMac-eLiSe-25/06/2007

    MicMac : Multi Image Correspondances par Methodes Automatiques de Correlation
    eLiSe  : ELements of an Image Software Environnement

    www.micmac.ign.fr

   
    Copyright : Institut Geographique National
    Author : Marc Pierrot Deseilligny
    Contributors : Gregoire Maillet, Didier Boldo.

[1] M. Pierrot-Deseilligny, N. Paparoditis.
    "A multiresolution and optimization-based image matching approach:
    An application to surface reconstruction from SPOT5-HRS stereo imagery."
    In IAPRS vol XXXVI-1/W41 in ISPRS Workshop On Topographic Mapping From Space
    (With Special Emphasis on Small Satellites), Ankara, Turquie, 02-2006.

[2] M. Pierrot-Deseilligny, "MicMac, un lociel de mise en correspondance
    d'images, adapte au contexte geograhique" to appears in 
    Bulletin d'information de l'Institut Geographique National, 2007.

Francais :

   MicMac est un logiciel de mise en correspondance d'image adapte 
   au contexte de recherche en information geographique. Il s'appuie sur
   la bibliotheque de manipulation d'image eLiSe. Il est distibue sous la
   licences Cecill-B.  Voir en bas de fichier et  http://www.cecill.info.


English :

    MicMac is an open source software specialized in image matching
    for research in geographic information. MicMac is built on the
    eLiSe image library. MicMac is governed by the  "Cecill-B licence".
    See below and http://www.cecill.info.

Header-MicMac-eLiSe-25/06/2007*/


#include "NewRechPH.h"

/*****************************************************/
/*                                                   */
/*            Constructeur                           */
/*                                                   */
/*****************************************************/


cOneScaleImRechPH::cOneScaleImRechPH(cAppli_NewRechPH &anAppli,const Pt2di & aSz,const double & aScale,const int & aNiv) :
   mAppli (anAppli),
   mSz    (aSz),
   mIm    (aSz.x,aSz.y),
   mTIm   (mIm),
   mScale (aScale),
   mNiv   (aNiv)
{
}

cOneScaleImRechPH* cOneScaleImRechPH::FromFile(cAppli_NewRechPH & anAppli,const std::string & aName,const Pt2di & aP0,const Pt2di & aP1Init)
{
   Tiff_Im aTifF = Tiff_Im::StdConvGen(aName,1,true);
   Pt2di aP1 = (aP1Init.x > 0) ? aP1Init : aTifF.sz();
   cOneScaleImRechPH * aRes = new cOneScaleImRechPH(anAppli,aP1-aP0,1.0,0);

   ELISE_COPY ( aRes->mIm.all_pts(),trans(aTifF.in_proj(),-aP0),aRes->mIm.out());

   return aRes;
}

cOneScaleImRechPH* cOneScaleImRechPH::FromScale(cAppli_NewRechPH & anAppli,cOneScaleImRechPH & anIm,const double & aSigma)
{

// MakeFlagMontant(anIm.mIm);
// template<class T1,class T2> Im2D_U_INT1 MakeFlagMontant(Im2D<T1,T2> anIm)

     cOneScaleImRechPH * aRes = new cOneScaleImRechPH(anAppli,anIm.mSz,aSigma,anIm.mNiv+1);

     // Pour reduire le temps de calcul, si deja plusieurs iters la fon de convol est le resultat
     // de plusieurs iters ...
     int aNbIter = 4;
     if (aRes->mNiv==2) aNbIter = 3;
     if (aRes->mNiv==1) aNbIter = 2;

     // anIm.mIm.dup(aRes->mIm);
     aRes->mIm.dup(anIm.mIm);
     double aParamG = sqrt(ElMax(0.0,ElSquare(aSigma)-ElSquare(anIm.mScale)));
     FilterGauss(aRes->mIm,aParamG,aNbIter);

     return aRes;
}

tImNRPH cOneScaleImRechPH::Im() {return mIm;}


bool   cOneScaleImRechPH::SelectVois(const Pt2di & aP,const std::vector<Pt2di> & aVVois,int aValCmp)
{
    tElNewRechPH aV0 =  mTIm.get(aP);
    for (int aKV=0 ; aKV<int(aVVois.size()) ; aKV++)
    {
        const Pt2di & aVois = aVVois[aKV];
        tElNewRechPH aV1 =  mTIm.get(aP+aVois,aV0);
        if (CmpValAndDec(aV0,aV1,aVois) != aValCmp)
           return false;
    }
    return true;
}



void cOneScaleImRechPH::CalcPtsCarac()
{
   std::vector<Pt2di> aVoisMinMax  = SortedVoisinDisk(0.5,mAppli.DistMinMax(),true);


   bool DoMin = mAppli.DoMin();
   bool DoMax = mAppli.DoMax();

   Im2D_U_INT1 aIFlag = MakeFlagMontant(mIm);
   TIm2D<U_INT1,INT> aTF(aIFlag);
   Pt2di aP ;
   for (aP.x = 0 ; aP.x <mSz.x ; aP.x++)
   {
       for (aP.y = 0 ; aP.y <mSz.y ; aP.y++)
       {
           int aFlag = aTF.get(aP);
           eTypePtRemark aLab = eTPR_NoLabel;
           
           if (/*DoMax &&  (aFlag == 0) &&*/ SelectVois(aP,aVoisMinMax,1))
           {
std::cout << "DAxx "<< DoMax << " " << aFlag << "\n";
               aLab = eTPR_Max;
           }
           if (/*DoMin &&  (aFlag == 255) &&*/ SelectVois(aP,aVoisMinMax,-1))
           {
std::cout << "DInnn "<< DoMin << " " << aFlag << "\n";
               aLab = eTPR_Min;
           }

           if (aLab!=eTPR_NoLabel)
              mLIPM.push_back(cIntPtRemark(aP,aLab));
        }
   }

}

void cOneScaleImRechPH::Show(Video_Win* aW)
{
   if (! aW) return;

   Im2D_U_INT1 aIR(mSz.x,mSz.y);
   Im2D_U_INT1 aIV(mSz.x,mSz.y);
   Im2D_U_INT1 aIB(mSz.x,mSz.y);

   ELISE_COPY(mIm.all_pts(),Max(0,Min(255,mIm.in())),aIR.out()|aIV.out()|aIB.out());

   for (std::list<cIntPtRemark>::const_iterator itIPM=mLIPM.begin(); itIPM!=mLIPM.end() ; itIPM++)
   {
       Pt3di aC = CoulOfType(itIPM->mType);

       ELISE_COPY
       (
          disc(Pt2dr(itIPM->mPt),2.0),
          Virgule(aC.x,aC.y,aC.z),
          Virgule(aIR.oclip(),aIV.oclip(),aIB.oclip())
       );
   }

   ELISE_COPY
   (
      mIm.all_pts(),
      Virgule(aIR.in(),aIV.in(),aIB.in()),
      aW->orgb()
   );
}




/*Footer-MicMac-eLiSe-25/06/2007

Ce logiciel est un programme informatique servant à la mise en
correspondances d'images pour la reconstruction du relief.

Ce logiciel est régi par la licence CeCILL-B soumise au droit français et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL-B telle que diffusée par le CEA, le CNRS et l'INRIA 
sur le site "http://www.cecill.info".

En contrepartie de l'accessibilité au code source et des droits de copie,
de modification et de redistribution accordés par cette licence, il n'est
offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
seule une responsabilité restreinte pèse sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les concédants successifs.

A cet égard  l'attention de l'utilisateur est attirée sur les risques
associés au chargement,  à l'utilisation,  à la modification et/ou au
développement et à la reproduction du logiciel par l'utilisateur étant 
donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
manipuler et qui le réserve donc à des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités à charger  et  tester  l'adéquation  du
logiciel à leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 

Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
termes.
aooter-MicMac-eLiSe-25/06/2007*/
