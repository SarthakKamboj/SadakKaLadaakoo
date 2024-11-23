#pragma once

#include <stdint.h>

#include "defines.h"
#include "skl_mem.h"

template <typename T>
struct skl_ufixed_pt {
    int m_num_frac_bits = 0;
    T m_val = 0;

    skl_ufixed_pt();
    skl_ufixed_pt(T& init_val, int num_frac_bits);

    skl_ufixed_pt<T> operator+(uint32_t in);
    skl_ufixed_pt<T> operator-(uint32_t in);
    skl_ufixed_pt<T> operator*(uint32_t in);
    skl_ufixed_pt<T> operator/(uint32_t in);

    float get_val();
}; 

template <typename T>
float skl_ufixed_pt<T>::get_val() {
    float res = 0;
    res += (m_val >> m_num_frac_bits);
    T mask = 0;
    skl_memset(static_cast<void*>(&mask), 0xff, sizeof(T));
    mask = mask >> ((sizeof(T) * 8) - m_num_frac_bits);
    float fractional = static_cast<float>(m_val & mask) / (1 << m_num_frac_bits);
    res += fractional;
    return res;
}

template <typename T>
skl_ufixed_pt<T>::skl_ufixed_pt() {
    m_val = 0;
    m_num_frac_bits = 0;
}

template <typename T>
skl_ufixed_pt<T>::skl_ufixed_pt(T& init_val, int num_frac_bits) {
    m_val = init_val;
    m_num_frac_bits = num_frac_bits;
}

template <typename T>
skl_ufixed_pt<T> skl_ufixed_pt<T>::operator+(uint32_t in) {
    skl_ufixed_pt<T> res;
    uint32_t inter = in << m_num_frac_bits;
    res.m_val = m_val + inter;
    res.m_num_frac_bits = m_num_frac_bits;
    return res;
}

template <typename T>
skl_ufixed_pt<T> skl_ufixed_pt<T>::operator-(uint32_t in) {
    skl_ufixed_pt<T> res;
    uint32_t inter = in << m_num_frac_bits;
    res.m_val = m_val - inter;
    res.m_num_frac_bits = m_num_frac_bits;
    return res;
}

template <typename T>
skl_ufixed_pt<T> skl_ufixed_pt<T>::operator*(uint32_t in) {
    skl_ufixed_pt<T> res;
    res.m_val = m_val * in;
    res.m_num_frac_bits = m_num_frac_bits;
    return res;
}

template <typename T>
skl_ufixed_pt<T> skl_ufixed_pt<T>::operator/(uint32_t in) {
    skl_ufixed_pt<T> res;
    res.m_val = m_val / in;
    res.m_num_frac_bits = m_num_frac_bits;
    return res;
}