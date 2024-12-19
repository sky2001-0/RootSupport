#ifndef ROOTSUPPORT_H
#define ROOTSUPPORT_H

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include "Rtypes.h"
#include "TAttLine.h"
#include "TAttMarker.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TCollection.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TError.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "THStack.h"
#include "TKey.h"
#include "TLine.h"
#include "TList.h"
#include "TMath.h"
#include "TMultiGraph.h"
#include "TObject.h"
#include "TPaveStats.h"
#include "TSystem.h"
#include "TVirtualFFT.h"

#include "libs/RootStyle.h"
#include "libs/RootTree.h"



namespace rs
{
  namespace rss
  {
    /*
      namespace rss provides assertion for template parameter.
    */

    template <typename TDirectoryLike>
    void Assert_if_is_inheritance_of_TDirectory()
    {
      static_assert(
        std::is_base_of_v<TDirectory, TDirectoryLike>,
        "Template parameter TDirectoryLike must be derived from TDirectory."
      );
    }


    template <typename TObjectLike>
    void Assert_if_is_inheritance_of_TObject()
    {
      static_assert(
        std::is_base_of_v<TObject, TObjectLike>,
        "Template parameter TObjectLike must be derived from TObject."
      );
    }


    template <typename TGraphLike>
    void Assert_if_is_inheritance_of_TGraph()
    {
      static_assert(
        std::is_base_of_v<TGraph, TGraphLike>,
        "Template parameter TGraphLike must be derived from TGraph."
      );
    }


    template <typename THistoLike>
    void Assert_if_is_inheritance_of_TH1()
    {
      static_assert(
        std::is_base_of_v<TH1, THistoLike>,
        "Template parameter THistoLike must be derived from TH1."
      );
    }
  }


  namespace file
  {
    inline std::unique_ptr<TFile> Open(const Char_t* filepath)
    {
      FileStat_t info;
      if (gSystem->GetPathInfo(filepath, info) != 0) {
        throw std::invalid_argument(std::string("No such file : ") + filepath);
      }

      auto file = std::make_unique<TFile>(filepath, "READ");
      if (!(file->IsOpen())) {
        throw std::runtime_error(
          std::string("failed to open this file : ") + filepath
        );
      }
      if (file->TestBit(TFile::kRecovered) || file->IsZombie()) {
        throw std::runtime_error(
          std::string("This file may be opend by other programs : ") + filepath
        );
      }
      return std::move(file);
    }


    inline std::unique_ptr<TFile> Open(const std::filesystem::path& filepath)
    {
      return std::move(Open(filepath.c_str()));
    }


    inline std::unique_ptr<TFile> Create(
      const Char_t* filepath,
      const Bool_t allow_override = kTRUE
    )
    {
      FileStat_t info;
      if (gSystem->GetPathInfo(filepath, info) == 0 && !allow_override) {
        throw std::runtime_error(
          std::string("This file already exists : ") + filepath
        );
      }
      return std::make_unique<TFile>(filepath, "RECREATE");
    }


    inline std::unique_ptr<TFile> Create(
      const std::filesystem::path& filepath,
      const Bool_t allow_override = kTRUE
    )
    {
      return std::move(Create(filepath.c_str(), allow_override));
    }


    template <typename TDirectoryLike>
    std::unique_ptr<TDirectory> MakeDir(
      const Char_t* dirname, TDirectoryLike* dir
    )
    {
      rss::Assert_if_is_inheritance_of_TDirectory<TDirectoryLike>();

      TDirectory* dir_new = nullptr;
      dir->GetObject(dirname, dir_new);
      if (!dir_new) {
        dir_new = dir->mkdir(dirname);
      }
      return std::unique_ptr<TDirectory>(dir_new);
    }


    template <typename TObjectLike, typename TDirectoryLike>
    std::unique_ptr<TObjectLike> GetObj(const Char_t* name, TDirectoryLike* dir)
    {
      rss::Assert_if_is_inheritance_of_TObject<TObjectLike>();
      rss::Assert_if_is_inheritance_of_TDirectory<TDirectoryLike>();

      TObjectLike* obj = nullptr;
      dir->GetObject(name, obj);
      if (!obj) {
        throw std::invalid_argument(
          std::string("No such object of ")
          + typeid(TObjectLike).name() + " : " + name
        );
      }
      return std::unique_ptr<TObjectLike>(obj);
    }


    template <typename TObjectLike, typename TDirectoryLike>
    std::vector<std::unique_ptr<TObjectLike>> GetObjList(
      TDirectoryLike* dir
    )
    {
      rss::Assert_if_is_inheritance_of_TObject<TObjectLike>();
      rss::Assert_if_is_inheritance_of_TDirectory<TDirectoryLike>();

      std::vector<std::unique_ptr<TObjectLike>> obj_list;
      TIter key_iter(dir->GetListOfKeys());
      while (true) {
        auto* key = static_cast<TKey*>(key_iter());
        if (!key) {
          break;
        }

        auto* obj = static_cast<TObjectLike*>(
          key->ReadObjectAny(TClass::GetClass<TObjectLike>())
        );
        if (obj) {
          obj->SetName(key->GetName());
          obj_list.push_back(std::unique_ptr<TObjectLike>(obj));
        }
      }
      return obj_list;
    }


    template <typename TObjectLike, typename TDirectoryLike>
    void Save(TObjectLike* obj, TDirectoryLike* dir)
    {
      rss::Assert_if_is_inheritance_of_TObject<TObjectLike>();
      rss::Assert_if_is_inheritance_of_TDirectory<TDirectoryLike>();

      dir->cd();
      obj->Write(obj->GetName());
    }
  }


  namespace graph
  {
    template <typename TGraphLike = TGraph>
    std::unique_ptr<TGraphLike> Create(
      const Int_t n,
      const Char_t* name,
      const Char_t* title,
      const Char_t* x_title,
      const Char_t* y_title,
      const Style_t style = kFullCircle
    )
    {
      rss::Assert_if_is_inheritance_of_TGraph<TGraphLike>();

      if (!(x_title && y_title)) {
        throw std::invalid_argument(
          "Both x_title and y_title must be provided."
        );
      }

      auto g = std::make_unique<TGraphLike>(n);
      if (title) {
        g->SetTitle(title);
      } else {
        g->SetTitle(y_title);
      }
      if (name) {
        g->SetName(name);
      } else {
        g->SetName(g->GetTitle());
      }
      g->GetXaxis()->SetTitle(x_title);
      g->GetYaxis()->SetTitle(y_title);
      g->SetMarkerStyle(style);
      return std::move(g);
    }


    template <typename TGraphLike>
    std::unique_ptr<TGraphLike> Create(const TGraphLike* g)
    {
      return std::move(Create(
        g->GetN(),
        g->GetName(),
        g->GetTitle(),
        g->GetXaxis()->GetTitle(),
        g->GetYaxis()->GetTitle()
      ));
    }


    template <typename TGraphLike>
    void SortX(TGraphLike* g)
    {
      rss::Assert_if_is_inheritance_of_TGraph<TGraphLike>();

      if (!(g->TestBit(TGraph::kIsSortedX))) {
        g->Sort();
      }
    }


    template <typename TGraphLike>
    void SetColor(const Color_t color, TGraphLike* g)
    {
      rss::Assert_if_is_inheritance_of_TGraph<TGraphLike>();

      g->SetMarkerColor(color);
      g->SetLineColor(color);
    }


    // template <typename TGraphLike>
    // void ScaleX(const Double_t factor, TGraphLike* g)
    // {
    //   rss::Assert_if_is_inheritance_of_TGraph<TGraphLike>();

    //   g->Scale(factor, "x");
    // }


    // template <typename TGraphLike>
    // void ScaleY(const Double_t factor, TGraphLike* g)
    // {
    //   rss::Assert_if_is_inheritance_of_TGraph<TGraphLike>();

    //   g->Scale(factor, "y");
    // }


    // template <typename TGraphLike>
    // void MovePoints(
    //   const Double_t x_move, const Double_t y_move, TGraphLike* g
    // )
    // {
    //   rss::Assert_if_is_inheritance_of_TGraph<TGraphLike>();

    //   g->MovePoints(x_move, y_move);
    // }


    template <typename TGraphLike>
    void InvertX(TGraphLike* g)
    {
      rss::Assert_if_is_inheritance_of_TGraph<TGraphLike>();

      auto Check_if_value_is_positive = [] (const Double_t val)
      {
        if (val <= 0) {
          throw std::range_error(
            "Unable to invert graph with non-positive x-values."
          );
        }
      };

      const Int_t n = g->GetN();
      Double_t* x = g->GetX();
      if constexpr (std::is_base_of_v<TGraphErrors, TGraphLike>) {
        Double_t* ex = g->GetEX();
        for (Int_t i = 0; i < n; ++i) {
          Check_if_value_is_positive(x[i]);
          x[i] = 1. / x[i];
          ex[i] = ex[i] / TMath::Power(x[i], 2);
        }
      } else {
        for (Int_t i = 0; i < n; ++i) {
          Check_if_value_is_positive(x[i]);
          x[i] = 1. / x[i];
        }
      }
      g->SetBit(TGraph::kIsSortedX, kFALSE);
    }


    inline void LogY(TGraph* g)
    {
      const Int_t n = g->GetN();
      Double_t* y = g->GetY();
      for (Int_t i = 0; i < n; ++i) {
        if (y[i] > 0) {
          y[i] = std::log(y[i]);
        } else {
          y[i] = std::nan("");
        }
      }
    }


    template <typename TGraphLike>
    void PushGraph(const TGraphLike* g_pushed, TGraphLike* g)
    {
      rss::Assert_if_is_inheritance_of_TGraph<TGraphLike>();

      const Int_t n = g->GetN();
      const Int_t n_pushed = g_pushed->GetN();
      g->Set(n + n_pushed);

      const Double_t* x_pushed = g_pushed->GetX();
      const Double_t* y_pushed = g_pushed->GetY();
      if constexpr (std::is_base_of_v<TGraphErrors, TGraphLike>) {
        const Double_t* ex_pushed = g_pushed->GetEX();
        const Double_t* ey_pushed = g_pushed->GetEY();
        for (Int_t i = 0; i < n_pushed; ++i) {
          g->SetPoint(n + i, x_pushed[i], y_pushed[i]);
          g->SetPointError(n + i, ex_pushed[i], ey_pushed[i]);
        }
      } else {
        for (Int_t i = 0; i < n_pushed; ++i) {
          g->SetPoint(n + i, x_pushed[i], y_pushed[i]);
        }
      }

      const Bool_t is_sorted_x = (
        g->TestBit(TGraph::kIsSortedX)
        && g_pushed->TestBit(TGraph::kIsSortedX)
        && (g->GetX()[n - 1] < x_pushed[0])
      );
      g->SetBit(TGraph::kIsSortedX, is_sorted_x);
    }


    inline std::unique_ptr<TGraph> FetchErrYGraph(const TGraphErrors* g)
    {
      auto g_err = std::make_unique<TGraph>(g->GetN(), g->GetX(), g->GetEY());
      g_err->GetXaxis()->SetTitle(g->GetXaxis()->GetTitle());
      return std::move(g_err);
    }


    template <typename TGraphLike>
    std::unique_ptr<TGraphLike> MakeGraphCoarseGrained(
      const TGraphLike* g, const Int_t step_grained
    )
    {
      if (step_grained <= 0) {
        throw std::invalid_argument("step_grained must be positive.");
      }
      if (step_grained == 1) {
        return std::move(Create(g));
      }

      const Int_t n_grained = g->GetN() / step_grained;
      auto g_grained = std::move(Create<TGraphLike>(
        n_grained,
        g->GetName(),
        g->GetTitle(),
        g->GetXaxis()->GetTitle(),
        g->GetYaxis()->GetTitle()
      ));

      if constexpr (std::is_base_of_v<TGraphErrors, TGraphLike>) {
        for (Int_t i_grained = 0; i_grained < n_grained; ++i_grained) {
          Double_t x_sum = 0.;
          Double_t y_sum = 0.;
          Double_t ex_sum2 = 0.;
          Double_t ey_sum2 = 0.;
          const Int_t end_index = (i_grained + 1) * step_grained;
          for (Int_t i = i_grained * step_grained; i < end_index; ++i) {
            x_sum += g->GetX()[i];
            y_sum += g->GetY()[i];
            ex_sum2 += std::pow(g->GetEX()[i], 2);
            ey_sum2 += std::pow(g->GetEY()[i], 2);
          }
          g_grained->SetPoint(
            i_grained,
            x_sum / step_grained,
            y_sum / step_grained
          );
          g_grained->SetPointError(
            i_grained,
            std::sqrt(ex_sum2 / step_grained),
            std::sqrt(ey_sum2 / step_grained / (step_grained - 1))
          );
        }
      } else {
        for (Int_t i_grained = 0; i_grained < n_grained; ++i_grained) {
          Double_t x_sum = 0.;
          Double_t y_sum = 0.;
          const Int_t end_index = (i_grained + 1) * step_grained;
          for (Int_t i = i_grained * step_grained; i < end_index; ++i) {
            x_sum += g->GetX()[i];
            y_sum += g->GetY()[i];
          }
          g_grained->SetPoint(
            i_grained,
            x_sum / step_grained,
            y_sum / step_grained
          );
        }
      }

      return std::move(g_grained);
    }


    template <typename THasAxis>
    void SetLimit(
      THasAxis* obj,
      const std::pair<Double_t, Double_t> x_limit = {0., 0.},
      const std::pair<Double_t, Double_t> y_limit = {0., 0.}
    )
    {
      static_assert(
        (
          std::is_base_of_v<TGraph, THasAxis>
          || std::is_base_of_v<TH1, THasAxis>
          || std::is_base_of_v<TMultiGraph, THasAxis>
          || std::is_base_of_v<THStack, THasAxis>
        ),
        "Template parameter THasAxis must be derived from TGraph or TH1."
      );

      if (x_limit.first < x_limit.second) {
        obj->GetXaxis()->SetLimits(x_limit.first, x_limit.second);
      }
      if (y_limit.first < y_limit.second) {
        obj->SetMinimum(y_limit.first);
        obj->SetMaximum(y_limit.second);
      }
    }


    template <typename THasAxis>
    void SetLimitY(
      THasAxis* obj, const std::pair<Double_t, Double_t> y_limit
    )
    {
      SetLimit(obj, {0., 0.}, y_limit);
    }
  }


  namespace draw
  {
    // inline void SetLogX(TCanvas* c)
    // {
    //   c->SetLogx();
    // }


    // inline void SetLogY(TCanvas* c)
    // {
    //   c->SetLogy();
    // }


    inline void DrawLine(
      TCanvas* c,
      std::pair<Double_t, Double_t> point_1,
      const std::pair<Double_t, Double_t> point_2,
      const Color_t color = kBlack,
      const Style_t style = kDashed,
      const Width_t width = 0
    )
    {
      auto l = new TLine(
        point_1.first, point_1.second, point_2.first, point_2.second
      );
      l->SetLineColor(color);
      l->SetLineStyle(style);
      if (width > 0) {
        l->SetLineWidth(width);
      }
      c->cd();
      l->Draw();
    }


    inline void DrawLineVertical(
      TCanvas* c,
      const Double_t x,
      const Color_t color = kBlack,
      const Style_t style = kDashed,
      const Width_t width = 0
    )
    {
      c->Update();
      DrawLine(c, {x, c->GetUymin()}, {x, c->GetUymax()}, color, style, width);
    }


    inline void DrawLineHorizontal(
      TCanvas* c,
      const Double_t y,
      const Color_t color = kBlack,
      const Style_t style = kDashed,
      const Width_t width = 0
    )
    {
      c->Update();
      DrawLine(c, {c->GetUxmin(), y}, {c->GetUxmax(), y}, color, style, width);
    }


    template <typename TObjectLike, typename TDirectoryLike>
    void FastSaveToRoot(
      TObjectLike* obj,
      TDirectoryLike* dir,
      Option_t* /* = const Char_t* */ opt,
      const Bool_t with_legend = kFALSE,
      const std::tuple<
        Double_t, Double_t, Double_t, Double_t
      > legend_position = {0.3, 0.21, 0.3, 0.21}
    )
    {
      rss::Assert_if_is_inheritance_of_TObject<TObjectLike>();
      rss::Assert_if_is_inheritance_of_TDirectory<TDirectoryLike>();

      auto c = std::make_unique<TCanvas>(obj->GetName(), obj->GetTitle());
      c->cd();
      obj->Draw(opt);
      if (with_legend) {
        const double& x_1 = std::get<0>(legend_position);
        const double& y_1 = std::get<1>(legend_position);
        const double& x_2 = std::get<2>(legend_position);
        const double& y_2 = std::get<3>(legend_position);
        c->BuildLegend(x_1, y_1, x_2, y_2);
      }
      dir->cd();
      c->Write();
    }


    template <typename TObjectLike>
    void FastSaveToFile(
      TObjectLike* obj,
      const Char_t* filepath,
      Option_t* /* = const Char_t* */ opt,
      const Char_t stats_position = 'U',
      const Bool_t with_legend = kFALSE,
      const std::tuple<
        Double_t, Double_t, Double_t, Double_t
      > legend_position = {0.3, 0.21, 0.3, 0.21}
    )
    {
      rss::Assert_if_is_inheritance_of_TObject<TObjectLike>();

      auto c = std::make_unique<TCanvas>(obj->GetName(), obj->GetTitle());
      obj->Draw(opt);
      if (with_legend) {
        const double& x_1 = std::get<0>(legend_position);
        const double& y_1 = std::get<1>(legend_position);
        const double& x_2 = std::get<2>(legend_position);
        const double& y_2 = std::get<3>(legend_position);
        c->BuildLegend(x_1, y_1, x_2, y_2);
      }

      c->Update();
      TPaveStats* stats = dynamic_cast<TPaveStats*>(
        obj->GetListOfFunctions()->FindObject("stats")
      );
      if (stats) {
        if (stats_position == 'U') {
          stats->SetX1NDC(0.98);
          stats->SetY1NDC(0.94);
          stats->SetX2NDC(0.73);
          stats->SetY2NDC(0.79);
        } else if (stats_position == 'L') {
          stats->SetX1NDC(0.98);
          stats->SetY1NDC(0.34);
          stats->SetX2NDC(0.73);
          stats->SetY2NDC(0.19);
        }
      }

      c->SaveAs(filepath);
    }


    template <typename TObjectLike>
    void FastSaveToFile(
      TObjectLike* obj,
      const std::filesystem::path& filepath,
      Option_t* /* = const Char_t* */ opt,
      const Char_t stats_position = 'U',
      const Bool_t with_legend = kFALSE,
      const std::tuple<
        Double_t, Double_t, Double_t, Double_t
      > legend_position = {0.3, 0.21, 0.3, 0.21}
    )
    {
      FastSaveToFile(
        obj, filepath.c_str(), opt, stats_position, with_legend, legend_position
      );
    }
  }


  namespace mul
  {
    inline void SetByFirstObject(const std::unique_ptr<TMultiGraph>& mg)
    {
      if (mg->GetListOfGraphs()->GetEntries() == 0) {
        throw std::invalid_argument(
          std::string("No graphs in this TMultiGraph : ") + mg->GetName()
        );
      }

      auto* g = static_cast<TGraph*>(mg->GetListOfGraphs()->At(0));
      mg->GetXaxis()->SetTitle(g->GetXaxis()->GetTitle());
      mg->GetYaxis()->SetTitle(g->GetYaxis()->GetTitle());
    }


    inline void SetByFirstObject(const std::unique_ptr<THStack>& hs)
    {
      if (hs->GetHists()->GetEntries() == 0) {
        throw std::invalid_argument(
          std::string("No histograms in this THStack : ") + hs->GetName()
        );
      }

      auto* h = static_cast<TH1*>(hs->GetHists()->At(0));
      hs->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());
      hs->GetYaxis()->SetTitle(h->GetYaxis()->GetTitle());
    }
  }


  namespace math
  {
    /* 1D FFT : real to complex */

    inline std::pair<std::vector<Double_t>, std::vector<Double_t>> FFT(
      const std::vector<Double_t> y,
      const Bool_t abs_arg = kFALSE
    )
    {
      Int_t size_tmp = y.size();
      TVirtualFFT* fft = TVirtualFFT::FFT(1, &size_tmp, "R2C ES");

      fft->SetPoints(y.data());
      fft->Transform();
      std::vector<Double_t> re_vec(y.size());
      std::vector<Double_t> im_vec(y.size());
      fft->GetPointsComplex(re_vec.data(), im_vec.data());

      if (abs_arg) {
        for (Int_t i = 0; i < y.size(); ++i) {
          const double re = re_vec[i];
          const double im = re_vec[i];
          re_vec[i] = TMath::Sqrt(
            TMath::Power(re, 2) + TMath::Power(im, 2)
          );
          im_vec[i] = TMath::ATan2(im, re);
        }
      }
      return {re_vec, im_vec};
    }


    inline std::vector<Double_t> IFFT(
      std::pair<std::vector<Double_t>, std::vector<Double_t>>& y_pair,
      const Bool_t abs_arg = kFALSE
    )
    {
      if (abs_arg) {
        throw std::runtime_error(
          "Not Implemented Exception : argument abs_arg must be false"
        );
      }
      const auto& re_vec = y_pair.first;
      const auto& im_vec = y_pair.second;
      const Int_t size = re_vec.size();

      Int_t size_tmp = size;
      TVirtualFFT* fft_back = TVirtualFFT::FFT(1, &size_tmp, "C2R ES");

      fft_back->SetPointsComplex(re_vec.data(), im_vec.data());
      fft_back->Transform();
      std::vector<Double_t> result(size);
      fft_back->GetPoints(result.data());

      for (auto& elem : result) {
        elem /= size;
      }
      return result;
    }
  }


  namespace utils
  {
    inline void IgnoreError()
    {
      gErrorIgnoreLevel = kFatal;
    }


    inline void IgnoreWarning()
    {
      gErrorIgnoreLevel = kWarning;
    }
  }
}



#endif // ROOTSUPPORT_H
