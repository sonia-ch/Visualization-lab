/**********************************************************************
 * Copyright (C) 2012-2013 Scientific Visualization Group - Link�ping University
 * All Rights Reserved.
 * 
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * No part of this software may be reproduced or transmitted in any
 * form or by any means including photocopying or recording without
 * written permission of the copyright owner.
 *
 * Primary author : Alexander Johansson
 *
 **********************************************************************/

#include <inviwo/core/properties/property.h>
#include <inviwo/core/util/variant.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/settings/systemsettings.h>

namespace inviwo {

std::map<std::string,std::string> Property::groupDisplayNames_;

Property::Property(std::string identifier,
                   std::string displayName,
                   PropertyOwner::InvalidationLevel invalidationLevel,
                   PropertySemantics semantics)
    : VoidObservable()
    , identifier_(identifier)
    , displayName_(displayName)
    , readOnly_(false)
    , semantics_(semantics)
    , visibilityMode_(APPLICATION)
    , propertyModified_(false)
    , invalidationLevel_(invalidationLevel)
    , owner_(0)
    , groupID_("") {
}

Property::Property()
    : identifier_("")
    , displayName_("")
    , propertyModified_(false){
}

std::string Property::getIdentifier() const {
    return identifier_;
}

void Property::setIdentifier(const std::string& identifier) {
    identifier_ = identifier;
}

std::string Property::getDisplayName() const {
    return displayName_;
}

void Property::setDisplayName(const std::string& displayName) {
    displayName_ = displayName;
}

void Property::setSemantics( const PropertySemantics& semantics) {
    semantics_ = semantics;
}

inviwo::PropertySemantics Property::getSemantics() const {
    return semantics_;
}

PropertyOwner* Property::getOwner() {
    return owner_;
}

void Property::setOwner(PropertyOwner* owner) {
    owner_ = owner;
}

void Property::registerWidget(PropertyWidget* propertyWidget) {
    propertyWidgets_.push_back(propertyWidget);
    if(this->visibilityMode_ == INVISIBLE)
        updateVisibility();
}

void Property::updateWidgets() {
    for (size_t i=0; i<propertyWidgets_.size(); i++)
        if (propertyWidgets_[i] != 0)
            propertyWidgets_[i]->updateFromProperty();
}

void Property::propertyModified() { 
    onChangeCallback_.invoke();
    setPropertyModified(true); 
    //FIXME: if set() is called before addProperty(), getOwner() will be 0 ( case for option properties )    
    if (getOwner()) getOwner()->invalidate(getInvalidationLevel());    
    updateWidgets();
}

Variant Property::getVariant() {
    return Variant(getVariantType());
}

void Property::setVariant(const Variant&) {}

int Property::getVariantType() {
    return Variant::VariantTypeInvalid;
}

void Property::serialize(IvwSerializer& s) const {
    s.serialize("type", getClassName(), true);
    s.serialize("identifier", identifier_, true);
    s.serialize("displayName", displayName_, true);
}

void Property::deserialize(IvwDeserializer& d) {
    std::string className;
    d.deserialize("type", className, true);
    d.deserialize("identifier", identifier_, true);
    d.deserialize("displayName", displayName_, true);
}

bool Property::operator==(const Property& prop){
	// TODO: this is not an equal operator, change to isSameType or similar
    if (this->getClassName()==prop.getClassName()) return true;
	else return false;
}

void Property::setGroupDisplayName(std::string groupID, std::string groupDisplayName) {
	Property::groupDisplayNames_.insert(std::pair<std::string,std::string>(groupID, groupDisplayName));
}

std::string Property::getGroupDisplayName() {
    return groupDisplayNames_[groupID_];
}

void Property::setVisibility(PropertyVisibilityMode visibilityMode) {
    this->visibilityMode_ = visibilityMode;
    updateVisibility();
}

void Property::updateVisibility() {
    InviwoApplication* inviwoApp = InviwoApplication::getPtr();
    Settings* mainSettings = inviwoApp->getSettingsByType<SystemSettings>();
    PropertyVisibilityMode appMode =  static_cast<PropertyVisibilityMode>(dynamic_cast<OptionPropertyInt*>(mainSettings->getPropertyByIdentifier("viewMode"))->get());

    if (visibilityMode_ == INVISIBLE) {
        for (size_t i=0; i<propertyWidgets_.size(); i++){
            propertyWidgets_[i]->hideWidget();
        }
    }
    if (visibilityMode_ == APPLICATION) {
        for (size_t i=0; i<propertyWidgets_.size(); i++){
            propertyWidgets_[i]->showWidget();
        }
    }
    else if (visibilityMode_ == DEVELOPMENT && appMode == DEVELOPMENT) {
        for (size_t i=0; i<propertyWidgets_.size(); i++){
            propertyWidgets_[i]->showWidget();
        }
    }
    else if (visibilityMode_ == DEVELOPMENT && appMode == APPLICATION ) {
        for (size_t i=0; i<propertyWidgets_.size(); i++)
            propertyWidgets_[i]->hideWidget();
    }

}

void Property::setVisible( bool val ) {
    if (val)
        setVisibility(APPLICATION);
    else
        setVisibility(INVISIBLE);
}

} // namespace
