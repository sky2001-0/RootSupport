#ifndef ROOTSTYLE_H
#define ROOTSTYLE_H

#include <memory>

#include "Rtypes.h"
#include "TAttMarker.h"
#include "TColor.h"
#include "TStyle.h"



namespace rs
{
  namespace utils
  {
    inline void SetStyle()
    {
      auto s = new TStyle("MyStyle", "my style");
      s->cd();

      const Font_t font = 42;
      const Float_t size = 0.035;

      /*
        SetAxis
      */
      s->SetTitleXOffset    (1.0);
      s->SetTitleYOffset    (1.0);
      s->SetLabelFont       (font,  "xyz");
      s->SetTitleFont       (font,  "xyz");
      s->SetTitleColor      (kBlack,"xyz");
      s->SetLabelSize       (size,  "xyz");
      s->SetTitleSize       (size,  "xyz");
      s->SetTitleOffset     (1.20,  "xyz");
      s->SetLabelOffset     (0.01,  "xyz");
      s->SetTickLength      (0.03,  "xyz");
      s->SetNdivisions      (510,   "x");
      s->SetNdivisions      (405,   "yz");
      // s->SetXAxisExpXOffset (0);
      // s->SetXAxisExpYOffset (0);
      // s->SetYAxisExpXOffset (0);
      // s->SetYAxisExpYOffset (0);
      s->SetAxisMaxDigits   (3);
      // s->SetOptLogx         (0);
      // s->SetOptLogy         (0);
      // s->SetOptLogz         (0);

      /*
        SetCanvas
      */
      s->SetCanvasColor     (kWhite);
      // s->SetCanvasBorderSize(2);
      s->SetCanvasBorderMode(0);
      // s->SetCanvasDefH      (500);
      // s->SetCanvasDefW      (700);
      // s->SetCanvasDefX      (10);
      // s->SetCanvasDefY      (10);

      /*
        SetDate
      */
      // s->SetOptDate         (0);
      // s->SetTimeOffset      (788918400); // UTC time at 01/01/95
      // s->SetDateX           ();
      // s->SetDateY           ();

      /*
        SetError
      */
      s->SetEndErrorSize    (0.);
      s->SetErrorX          (0.001);

      /*
        SetFrame
      */
      // s->SetFrameLineColor  (1);
      s->SetFrameFillColor  (kWhite);
      // s->SetFrameFillStyle  (1001);
      // s->SetFrameLineStyle  (1);
      s->SetFrameLineWidth  (2);
      // s->SetFrameBorderSize (1);
      s->SetFrameBorderMode (0);

      /*
        SetFit
      */
      s->SetOptFit          (1011);
      s->SetFitFormat       (".4g");

      /*
        SetFunc
      */
      s->SetFuncColor       (kRed);
      // s->SetFuncStyle       (1);
      s->SetFuncWidth       (1);

      /*
        SetHist
      */
      s->SetHistLineColor   (kBlue + 2);
      // s->SetHistFillColor   (kWhite);
      // s->SetHistFillStyle   (1001);
      // s->SetHistLineStyle   (1);
      s->SetHistLineWidth   (1);
      // s->SetHistMinimumZero (kFALSE);
      // s->SetHistTopMargin   (0.05);
      s->SetPalette         (1);

      /*
        SetLegend
      */
      s->SetLegendBorderSize(1);
      s->SetLegendFont      (font);
      // s->SetLegendTextSize  (size);
      s->SetLegendFillColor (kWhite);

      /*
        SetMarker
      */
      s->SetMarkerStyle     (kFullCircle);
      s->SetMarkerSize      (0.3);

      /*
        SetPad
      */
      // s->SetPadBorderSize   (2);
      // s->SetPadGridX        (kFALSE);
      // s->SetPadGridY        (kFALSE);
      s->SetPadTickX        (1);
      s->SetPadTickY        (1);
      s->SetPadBorderMode   (0);
      s->SetPadColor        (kWhite);
      s->SetPadTopMargin    (0.08);
      s->SetPadRightMargin  (0.07);
      s->SetPadBottomMargin (0.13);
      s->SetPadLeftMargin   (0.14);

      /*
        SetStat
      */
      s->SetOptStat         ("RM");
      s->SetStatStyle       (1001);
      s->SetStatColor       (kWhite);
      s->SetStatTextColor   (kBlack);
      s->SetStatBorderSize  (1);
      s->SetStatFont        (font);
      s->SetStatW           (0.25);
      s->SetStatH           (0.15);
      s->SetStatX           (0.98);
      s->SetStatY           (0.94);
      s->SetStatFormat      (".4g");

      /*
        SetTitle
      */
      s->SetOptTitle        (1);
      s->SetTitleAlign      (23);
      s->SetTitleTextColor  (kBlack);
      s->SetTitleFont       (font, "");
      s->SetTitleFontSize   (0.05);
      s->SetTitleStyle      (0);
      s->SetTitleBorderSize (0);
      // s->SetTitleW          (0);
      // s->SetTitleH          (0);
      s->SetTitleX          (0.555);
      // s->SetTitleY          (0.995);

      /*
        Set others
      */
      // s->SetDrawBorder      (0);
      // s->SetImageScaling    (1.);
      // s->SetLegoInnerR      (.5);
      // s->SetNumberContours  (20);
      // s->SetOptFile         (0);
      // s->SetOrthoCamera     (kFALSE);
      // s->SetPaintTextFormat ();
      s->SetPaperSize       (20, 26);
      // s->SetScreenFactor    (1);
      // s->SetStripDecimals   (kTRUE);
      // s->SetViolinScaled    (kTRUE);
    }
  }
}



#endif // ROOTSTYLE_H
