
#ifndef _PEPS_H_
#define _PEPS_H_

#include "TPO.h"
#include "peps_indexset.h"

//
//class PEPSt
//the lower t is for template
//
//using PEPS for ITensor while IQPEPS for IQTensor
//
template <class TensorT>
class PEPSt
{
    public:

        //
        //type alias
        //
        using IndexT=typename TensorT::IndexT;
        using IndexValT=typename TensorT::IndexValT;
        using CombinerT=typename TensorT::CombinerT;

        //
        //Constructors
        //
        //PEPSt(): lattice_ptr_(new Lattice_Base), indexset_ptr_(new PEPSt_IndexSet_Base<IndexT>) {}
        PEPSt(const Lattice_Base &lattice);
        PEPSt(const Lattice_Base &lattice, const PEPSt_IndexSet_Base<IndexT> &index_set);

        //Initialize site tensors by tensor data in one unit cell, mu_12 represents PSG of translation symmetry
        //Thus, the PEPS is translationally invariant (up to mu_12).
        PEPSt(const Lattice_Base &lattice, const PEPSt_IndexSet_Base<IndexT> &index_set, std::vector<TensorT> site_tensors_uc, std::vector<TensorT> bond_tensors_uc, double mu_12=1);

        //
        //Access Methods
        //
        int d() const
        {
            return d_;
        }

        int D() const
        {
            return D_;
        }

        const std::array<int,2> &n_uc() const
        {
            return lattice_ptr_->n_uc();
        }

        int n_sites_uc() const
        {
            return lattice_ptr_->n_sites_uc();
        }

        int n_bonds_uc() const
        {
            return lattice_ptr_->n_bonds_uc();
        }

        int n_bonds_to_one_site() const
        {
            return lattice_ptr_->n_bonds_to_one_site();
        }

        int n_sites_to_one_bond() const
        {
            return lattice_ptr_->n_sites_to_one_bond();
        }

        int n_sites_total() const
        {
            return lattice_ptr_->n_sites_total();
        }

        int n_bonds_total() const
        {
            return lattice_ptr_->n_bonds_total();
        }

        int n_boundary_legs() const
        {
            return lattice_ptr_->n_boundary_legs();
        }

        const Lattice_Base &lattice() const
        {
            return *(lattice_ptr_);
        }
        
        const PEPSt_IndexSet_Base<IndexT> &indexset() const
        {
            return *(indexset_ptr_);
        }

        const IndexT &phys_legs(const int &site_i) const
        {
            return indexset_ptr_->phys_legs(site_i);
        }

        inline const IndexT &virt_legs(const int &leg_i) const
        {
            return indexset_ptr_->virt_legs(leg_i);
        }
        inline const IndexT &virt_legs(int sitei, int neighi) const
        {
            return indexset_ptr_->virt_legs(sitei*n_bonds_to_one_site()+neighi);
        }

        const TensorT &site_tensors(int i) const
        {
            return site_tensors_[i];
        }
        TensorT &site_tensors(int i)
        {
            return site_tensors_[i];
        }
        const TensorT &site_tensors(const Coordinate &coord) const
        {
            int siteind=lattice().site_coord_to_list(coord);
            return site_tensors_[siteind];
        }
        TensorT &site_tensors(const Coordinate &coord)
        {
            int siteind=lattice().site_coord_to_list(coord);
            return site_tensors_[siteind];
        }
        const std::vector<TensorT> &site_tensors() const
        {
            return site_tensors_;
        }

        const TensorT &bond_tensors(int i) const
        {
            return bond_tensors_[i];
        }
        TensorT &bond_tensors(int i)
        {
            return bond_tensors_[i];
        }
        const TensorT &bond_tensors(const Coordinate &coord) const
        {
            int bondind=lattice().bond_coord_to_list(coord);
            return bond_tensors_[bondind];
        }
        TensorT &bond_tensors(const Coordinate &coord)
        {
            int bondind=lattice().bond_coord_to_list(coord);
            return bond_tensors_[bondind];
        }
        const std::vector<TensorT> &bond_tensors() const
        {
            return bond_tensors_;
        }

        const TensorT &combined_tensors(int i) const { return combined_site_tensors_[i]; }
        TensorT &combined_tensors(int i) { return combined_site_tensors_[i]; }
        const TensorT &combined_tensors(const Coordinate &coord) const
        {
            int site_ind=lattice().site_coord_to_list(coord);
            return combined_site_tensors_[site_ind];
        }
        TensorT &combined_tensors(const Coordinate &coord)
        {
            int site_ind=lattice().site_coord_to_list(coord);
            return combined_site_tensors_[site_ind];
        }
        const std::vector<TensorT> &combined_tensors() const { return combined_site_tensors_; }

        const TensorT &boundary_tensors(int i) const
        {
            return boundary_tensors_[i];
        }

        TensorT &boundary_tensors(int i)
        {
            return boundary_tensors_[i];
        }

        const std::vector<TensorT> &boundary_tensors() const
        {
            return boundary_tensors_;
        }

        std::vector<TensorT> &boundary_tensors()
        {
            return boundary_tensors_;
        }

        const std::string &name() const
        {
            return name_;
        }

        //
        //Methods
        //
        //given tensors in one uc, generate all translation related tensors
        //this function requires order for indices of given tensors must match those in peps
        void generate_site_tensors(std::vector<TensorT> site_tensors_uc);
        void generate_bond_tensors(std::vector<TensorT> bond_tensors_uc, double mu_12);
        //void generate_boundary_tensors(TensorT single_boundary_tensor);

        //this function returns site tensors that absorb the neighbour bond tensors and boundary tensors
        //this function only works for PEPS with canonical bond tensors (two ends)
        std::vector<TensorT> combined_site_tensors() const;
        void obtain_combined_site_tensors();
        bool have_combined_tensors() const { return !(combined_site_tensors_.empty()); }

        //
        //Constructor Helpers
        //

        //According to the itensor library, ITensor is constructed by IndexSet<Index>(indexset), while IQTensor is constructed by indexset directly
        void construct_tensor(TensorT &tensor, std::vector<IndexT> &indexset);
        //construct new tensors
        void new_site_tensors();
        void new_bond_tensors();
        void new_boundary_tensors();
        //using random initial site tensors
        //for IQTensor we need to make sure at least one block is created
        //void random_site_tensors();

        //read/write from/to file
        void read(std::istream &s);
        void write(std::ostream &s) const;

    private:
        //
        //Data Member
        //
        //d_ is physical index dimension while D_ is virtual leg dim.
        int d_, D_;

        std::shared_ptr<Lattice_Base> lattice_ptr_;

        std::shared_ptr<PEPSt_IndexSet_Base<IndexT>> indexset_ptr_;

        std::vector<TensorT> site_tensors_, bond_tensors_, combined_site_tensors_; 

        //using for boundary condition
        std::vector<TensorT> boundary_tensors_;


        //name stores information about lattice, spin symmetry, extra degenerate
        std::string name_;
};
using PEPS=PEPSt<ITensor>;
using IQPEPS=PEPSt<IQTensor>;


//translate PEPS to Tnetwork_Storage
template <class TensorT>
Tnetwork_Storage<TensorT> peps_to_tnetwork_storage(const PEPSt<TensorT> &peps);

#endif
