 /*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2013-2014 Inviwo Foundation
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Main file author: Erik Sund�n
 *
 *********************************************************************************/

#ifndef IVW_MINMAXPROPERTY_H
#define IVW_MINMAXPROPERTY_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/properties/templateproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>

namespace inviwo {

template<typename T>
class MinMaxProperty : public TemplateProperty<glm::detail::tvec2<T> > {

public:
    MinMaxProperty(std::string identifier,
                   std::string displayName,
                   T valueMin = Defaultvalues<T>::getMin(),
                   T valueMax = Defaultvalues<T>::getMax(),
                   T rangeMin = Defaultvalues<T>::getMin(),
                   T rangeMax = Defaultvalues<T>::getMax(),
                   T increment = Defaultvalues<T>::getInc(),
                   PropertyOwner::InvalidationLevel invalidationLevel = PropertyOwner::INVALID_OUTPUT,
                   PropertySemantics semantics = PropertySemantics::Default);

    T getRangeMin() const;
    T getRangeMax() const;
    T getIncrement() const;

    glm::detail::tvec2<T> getRange() const;

    void setRangeMin(const T& value);
    void setRangeMax(const T& value);
    void setIncrement(const T& value);

    void setRange(const glm::detail::tvec2<T>& value);

    virtual std::string getClassName()  const;

    virtual Variant getVariant();
    virtual void setVariant(const Variant& v);
    virtual int getVariantType();

    virtual void serialize(IvwSerializer& s) const;
    virtual void deserialize(IvwDeserializer& s);

private:
    glm::detail::tvec2<T> range_;
    T increment_;
};

typedef MinMaxProperty<float> FloatMinMaxProperty;
typedef MinMaxProperty<double> DoubleMinMaxProperty;
typedef MinMaxProperty<int> IntMinMaxProperty;


template<typename T>
std::string MinMaxProperty<T>::getClassName() const {
    std::stringstream ss;
    ss << Defaultvalues<T>::getName() << "MinMaxProperty";
    return ss.str();
}

template <typename T>
MinMaxProperty<T>::MinMaxProperty(std::string identifier, std::string displayName,
                                  T valueMin, T valueMax, T rangeMin, T rangeMax, T increment,
                                  PropertyOwner::InvalidationLevel invalidationLevel,
                                  PropertySemantics semantics)
    : TemplateProperty<glm::detail::tvec2<T> >(identifier, displayName, glm::detail::tvec2<T>(valueMin, valueMax), invalidationLevel,
            semantics),
    range_(glm::detail::tvec2<T>(rangeMin, rangeMax)),
    increment_(increment)
{}

template <typename T>
T MinMaxProperty<T>::getRangeMin() const {
    return range_.x;
}

template <typename T>
T MinMaxProperty<T>::getRangeMax() const {
    return range_.y;
}

template <typename T>
T MinMaxProperty<T>::getIncrement() const {
    return increment_;
}

template <typename T>
glm::detail::tvec2<T> MinMaxProperty<T>::getRange() const {
    return range_;
}

template <typename T>
void MinMaxProperty<T>::setRangeMin(const T& value) {
    if (TemplateProperty<glm::detail::tvec2<T> >::value_.x < value || TemplateProperty<glm::detail::tvec2<T> >::value_.x == range_.x)
        TemplateProperty<glm::detail::tvec2<T> >::value_.x = value;

    range_.x = value;
    TemplateProperty<glm::detail::tvec2<T> >::propertyModified();
}

template <typename T>
void MinMaxProperty<T>::setRangeMax(const T& value) {
    if (TemplateProperty<glm::detail::tvec2<T> >::value_.y > value || TemplateProperty<glm::detail::tvec2<T> >::value_.y == range_.y)
        TemplateProperty<glm::detail::tvec2<T> >::value_.y = value;

    range_.y = value;
    TemplateProperty<glm::detail::tvec2<T> >::propertyModified();
}

template <typename T>
void MinMaxProperty<T>::setIncrement(const T& value) {
    increment_ = value;
}

template <typename T>
void MinMaxProperty<T>::setRange(const glm::detail::tvec2<T>& value) {
    setRangeMin(value.x);
    setRangeMax(value.y);
}

template <typename T>
Variant MinMaxProperty<T>::getVariant() {
    return Variant(TemplateProperty<glm::detail::tvec2<T> >::value_);
}

template <typename T>
void MinMaxProperty<T>::setVariant(const Variant& v) {
    if (v.canConvert(getVariantType()))
        this->set(v.get<glm::detail::tvec2<T> >());
}

template <typename T>
int MinMaxProperty<T>::getVariantType() {
    return getVariant().getType();
}

template <typename T>
void MinMaxProperty<T>::serialize(IvwSerializer& s) const {
    Property::serialize(s);
    s.serialize("value", TemplateProperty<glm::detail::tvec2<T> >::get());
    s.serialize("range", MinMaxProperty<T>::getRange());
    s.serialize("increment", MinMaxProperty<T>::getIncrement());
}

template <typename T>
void MinMaxProperty<T>::deserialize(IvwDeserializer& d) {
    Property::deserialize(d);
    glm::detail::tvec2<T> value;
    d.deserialize("range", value);
    MinMaxProperty<T>::setRange(value);
    T increment;
    d.deserialize("increment", increment);
    MinMaxProperty<T>::setIncrement(increment);
    d.deserialize("value", value);
    TemplateProperty<glm::detail::tvec2<T> >::set(value);
}

} // namespace

#endif // IVW_MINMAXPROPERTY_H
