//
// Created by lg on 17-9-5.
//

#pragma once

#include <utility>

template<class destType,class srcType>
union value_cast_impl{
    destType value;
    srcType srcValue;
    constexpr explicit value_cast_impl(srcType value):srcValue(value){}
    ~value_cast_impl(){};
};


template<class destType,class srcType,
        class =typename std::enable_if<!std::is_reference<destType>::value
                                       &&!std::is_reference<srcType>::value>::type>
constexpr destType value_cast(srcType value){
    return value_cast_impl<destType, srcType>(value).value;
};