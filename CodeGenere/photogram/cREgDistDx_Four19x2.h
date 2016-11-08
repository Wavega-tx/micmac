// File Automatically generated by eLiSe
#include "StdAfx.h"


class cREgDistDx_Four19x2: public cElCompiledFonc
{
   public :

      cREgDistDx_Four19x2();
      void ComputeVal();
      void ComputeValDeriv();
      void ComputeValDerivHessian();
      double * AdrVarLocFromString(const std::string &);
      void SetFour19x2_State_0_0(double);
      void SetFour19x2_State_1_0(double);
      void SetFour19x2_State_2_0(double);
      void SetRegDistu1_x(double);
      void SetRegDistu1_y(double);
      void SetRegDistu2_x(double);
      void SetRegDistu2_y(double);
      void SetRegDistu3_x(double);
      void SetRegDistu3_y(double);


      static cAutoAddEntry  mTheAuto;
      static cElCompiledFonc *  Alloc();
   private :

      double mLocFour19x2_State_0_0;
      double mLocFour19x2_State_1_0;
      double mLocFour19x2_State_2_0;
      double mLocRegDistu1_x;
      double mLocRegDistu1_y;
      double mLocRegDistu2_x;
      double mLocRegDistu2_y;
      double mLocRegDistu3_x;
      double mLocRegDistu3_y;
};
