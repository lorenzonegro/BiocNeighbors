#include "hnsw.h"
#include <cmath>

template<class Space>
Hnsw<Space>::Hnsw(Rcpp::NumericMatrix dat, const std::string& fname, int efs) : 
    data(dat), space(data.nrow()), obj(&space, fname), holding(data.nrow()), ef_search(efs)
{
    if (ef_search < 1) {
        throw std::runtime_error("ef.search should be a positive integer scalar");
    }
    return;
}

template<class Space>
MatDim_t Hnsw<Space>::get_nobs() const { 
    return data.ncol();
}

template<class Space>
MatDim_t Hnsw<Space>::get_ndims() const {
    return data.nrow();
}

template<class Space>
const std::deque<CellIndex_t>& Hnsw<Space>::get_neighbors () const {
    return kept_idx;
}

template<class Space>
const std::deque<double>& Hnsw<Space>::get_distances () const {
    return kept_dist;
}

template<class Space>
void Hnsw<Space>::set_ef_search(NumNeighbors_t K) {
    obj.ef_ = std::max(K, ef_search);
    return;
}

template<class Space>
void Hnsw<Space>::find_nearest_neighbors(CellIndex_t c, NumNeighbors_t K, const bool index, const bool distance) {
    auto curcol=data.column(c);
    std::copy(curcol.begin(), curcol.end(), holding.begin());
    set_ef_search(K+1);
    auto Q=obj.searchKnn(holding.data(), K+1);

    // Auto-filter self.
    bool found_self=false;
    kept_idx.clear();
    kept_dist.clear();

    while (!Q.empty()) {
        const auto& current=Q.top();
        if (static_cast<CellIndex_t>(current.second)!=c) {
            if (index) {
                kept_idx.push_front(current.second);
            } 
            if (distance) {
                kept_dist.push_front(normalize(current.first));
            }
        } else {
            found_self=true;
        }
        Q.pop();
    }

    if (!found_self) {
        if (!kept_idx.empty()) {
            kept_idx.pop_back();
        }
        if (!kept_dist.empty()) {
            kept_dist.pop_back();
        }
    }

    return;
}

template<class Space>
void Hnsw<Space>::find_nearest_neighbors(const double* query, NumNeighbors_t K, const bool index, const bool distance) {
    std::copy(query, query+holding.size(), holding.begin());
    set_ef_search(K);
    auto Q=obj.searchKnn(holding.data(), K);
    kept_idx.clear();
    kept_dist.clear();

    while (!Q.empty()) {
        const auto& current=Q.top();
        if (index) {
            kept_idx.push_front(current.second);
        }
        if (distance) {
            kept_dist.push_front(normalize(current.first));
        }
        Q.pop();
    }

    return;
}

template<>
double Hnsw<L1Space>::normalize(double x) { return x; }

template<>
double Hnsw<hnswlib::L2Space>::normalize(double x) { return std::sqrt(x); }

template class Hnsw<L1Space>;
template class Hnsw<hnswlib::L2Space>;

/******* Defining the Manhattan distance class ********/

L1Space::L1Space(size_t ndim) : data_size_(ndim*sizeof(float)), dim_(ndim) {}

L1Space::~L1Space() {}

size_t L1Space::get_data_size() { return data_size_; }

hnswlib::DISTFUNC<float> L1Space::get_dist_func() {
    return L1;
}

void * L1Space::get_dist_func_param() {
    return &dim_;
}

float L1Space::L1(const void *pVect1v, const void *pVect2v, const void *qty_ptr) {
    //return *((float *)pVect2);
    const float* pVect1=static_cast<const float*>(pVect1v);
    const float* pVect2=static_cast<const float*>(pVect2v);
    size_t qty = *((size_t *) qty_ptr);
    float res = 0;
    for (; qty > 0; --qty, ++pVect1, ++pVect2) {
        res += std::fabs(*pVect1 - *pVect2);
    }
    return (res);
}
