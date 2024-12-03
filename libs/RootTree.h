#ifndef ROOTTREE_H
#define ROOTTREE_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Rtypes.h"
#include "TLeaf.h"
#include "TObjArray.h"
#include "TTree.h"



namespace rs
{
  class TreeHelper
  {
  public:
    using LeafType = std::variant<Bool_t, Int_t, Double_t>;


  private:
    std::unique_ptr<TTree> tree_;
    std::unordered_map<std::string, LeafType> vals_;


  public:
    TreeHelper() = delete;

    TreeHelper(std::unique_ptr<TTree>&& tree)
    : tree_(std::move(tree))
    {
      TObjArray* branches = tree_->GetListOfBranches();

      for (Int_t i = 0; i < branches->GetEntries(); ++i) {
        auto* branch = static_cast<TBranch*>(branches->At(i));
        const std::string bname = branch->GetName();
        TLeaf* leaf = branch->GetLeaf(bname.c_str());
        const std::string type_name = leaf->GetTypeName();

        if (type_name == "Bool_t") {
          Bool_t val;
          vals_[bname + "/B"] = val;
          Bool_t* ptr = std::get_if<0>(&vals_[bname + "/B"]);
          tree_->SetBranchAddress(bname.c_str(), ptr);
        } else if (type_name == "Int_t") {
          Int_t val;
          vals_[bname + "/I"] = val;
          Int_t* ptr = std::get_if<1>(&vals_[bname + "/I"]);
          tree_->SetBranchAddress(bname.c_str(), ptr);
        } else if (type_name == "Double_t") {
          Double_t val;
          vals_[bname + "/D"] = val;
          Double_t* ptr = std::get_if<2>(&vals_[bname + "/D"]);
          tree_->SetBranchAddress(bname.c_str(), ptr);
        } else {
          std::cout
            << bname
            << " has an unknown type : "
            << leaf->GetTypeName()
            << std::endl;
        }
      }
    }

    TreeHelper(const std::vector<std::string>& branch_names)
    : tree_(std::make_unique<TTree>("tree", "tree"))
    {
      for (const std::string bname : branch_names) {
        if (bname.size() <= 2) {
          throw std::runtime_error("branch name error.");
        }

        switch (bname.back()) {
          case 'B' : {
            Bool_t val;
            vals_[bname] = val;
            Bool_t* ptr = std::get_if<0>(&vals_[bname]);
            tree_->Branch(
              bname.substr(0, bname.size() - 2).c_str(), ptr, bname.c_str()
            );
            break;
          }
          case 'I' : {
            Int_t val;
            vals_[bname] = val;
            Int_t* ptr = std::get_if<1>(&vals_[bname]);
            tree_->Branch(
              bname.substr(0, bname.size() - 2).c_str(), ptr, bname.c_str()
            );
            break;
          }
          case 'D' : {
            Double_t val;
            vals_[bname] = val;
            Double_t* ptr = std::get_if<2>(&vals_[bname]);
            tree_->Branch(
              bname.substr(0, bname.size() - 2).c_str(), ptr, bname.c_str()
            );
            break;
          }
          default : {
            throw std::runtime_error("unknown type for " + bname);
          }
        }
      }
    }

    ~TreeHelper() = default;

    TreeHelper(const TreeHelper& rh) = delete;

    TreeHelper(TreeHelper&& rh) = default;

    TreeHelper& operator=(const TreeHelper& rh) = delete;

    TreeHelper& operator=(TreeHelper&& rh) = delete;


    LeafType cget(const std::string bname) const
    {
      return vals_.at(bname);
    }

    LeafType& get(const std::string bname)
    {
      return vals_.at(bname);
    }

    Long64_t GetEntries() const
    {
      return tree_->GetEntries();
    }

    Int_t GetEntry(Long64_t entry)
    {
      return tree_->GetEntry(entry);
    }

    Int_t Fill()
    {
      return tree_->Fill();
    }

    Int_t Write()
    {
      return tree_->Write();
    }
  };
}



#endif // ROOTTREE_H
