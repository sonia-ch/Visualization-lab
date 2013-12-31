/**********************************************************************
 * Copyright (C) 2013 Scientific Visualization Group - Link�ping University
 * All Rights Reserved.
 * 
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * No part of this software may be reproduced or transmitted in any
 * form or by any means including photocopying or recording without
 * written permission of the copyright owner.
 *
 * Primary author : Sathish Kottravel
 *
 **********************************************************************/

#include <modules/python/pythonscript.h>
#include <modules/python/pythonscriptrecorderutil.h>

#include <inviwo/core/util/assertion.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/filedirectory.h>

namespace inviwo {

PythonScriptRecorderUtil::PythonScriptRecorderUtil(PythonScript* script) :  recordScript_(false) , script_(script)
{}

PythonScriptRecorderUtil::~PythonScriptRecorderUtil() {    
}

void PythonScriptRecorderUtil::startRecording() {
    recordScript_ = true;
}
    
void PythonScriptRecorderUtil::endRecording() {
    recordScript_ = false;
}
    
bool PythonScriptRecorderUtil::isRecording() {
    return recordScript_;
}

void PythonScriptRecorderUtil::recordNetworkChanges() {
    if (recordScript_) {
        ProcessorNetwork* network = InviwoApplication::getPtr()->getProcessorNetwork();
        bool networkModified = network->isModified();
        //network modified
        if (networkModified) {
            Property* modifiedProperty = 0;

            std::vector<Processor*> processors = network->getProcessors();

            for (size_t i=0; i<processors.size(); i++) {
                std::vector<Property*> properties = processors[i]->getProperties();
                for (size_t j=0; j<properties.size(); j++) {
                    //property modified
                    if (properties[j]->isPropertyModified()) {
                        std::string pyCommand = getPyProperty(properties[j]);
                        std::string previousSrc = script_->getSource();
                        previousSrc+= pyCommand;
                        script_->setSource(previousSrc);
                    }
                }
            }
        }
    }
}

std::string PythonScriptRecorderUtil::getPyProperty(Property* property) {
    std::string propertyCommand("");

    std::string commandOpen = "inviwo.setPropertyValue(";
    std::string processorName = "\"" + dynamic_cast<Processor*>(property->getOwner())->getIdentifier() + "\"";
    std::string propertyName = "\"" + property->getIdentifier() + "\"";
    std::string propertyValue = "";
    std::string commandClose = ")";

    //FIXME: Replace all property values to string variants??
    std::stringstream ss;

    if (dynamic_cast<BoolProperty*>(property)) {
        BoolProperty* prop = dynamic_cast<BoolProperty*>(property);
        ss << prop->get();
        propertyValue += ss.str();
    }
    else if (dynamic_cast<ButtonProperty*>(property)) {
        commandOpen = "inviwo.clickButton(";
    }
    else if (dynamic_cast<CompositeProperty*>(property)) {
        commandOpen =+ "# NOT IMPLEMENTED # " + commandOpen ;
    }
    else if (dynamic_cast<DirectoryProperty*>(property)) {
        DirectoryProperty* prop = dynamic_cast<DirectoryProperty*>(property);
        ss << "\"";
        ss << prop->get();
        ss << "\"";
        propertyValue += ss.str();
    }    
    else if (dynamic_cast<FileProperty*>(property)) {
        FileProperty* prop = dynamic_cast<FileProperty*>(property);
        ss << "\"";
        ss << prop->get();
        propertyValue += ss.str();
        ss << "\"";
    }
    else if (dynamic_cast<FloatMat2Property*>(property)) {
        FloatMat2Property* prop = dynamic_cast<FloatMat2Property*>(property);
        mat2 m = prop->get();
        ss << "(";
        ss << m[0][0] << " ";
        ss << m[0][1] ;
        ss << ")";
        ss << ",";
        ss << "(";
        ss << m[1][0] << " ";
        ss << m[1][1] ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<FloatMat3Property*>(property)) {
        FloatMat3Property* prop = dynamic_cast<FloatMat3Property*>(property);
        mat3 m = prop->get();
        ss << "(";
        ss << m[0][0] << " ";
        ss << m[0][1] << " ";
        ss << m[0][2] ;
        ss << ")";
        ss << ",";
        ss << "(";
        ss << m[1][0] << " ";
        ss << m[1][1] << " ";
        ss << m[1][2] ;
        ss << ")";
        ss << ",";
        ss << "(";
        ss << m[2][0] << " ";
        ss << m[2][1] << " ";
        ss << m[2][2] ;
        ss << ")";   
        propertyValue += ss.str();
    }
    else if (dynamic_cast<FloatMat4Property*>(property)) {
        FloatMat4Property* prop = dynamic_cast<FloatMat4Property*>(property);
        mat4 m = prop->get();
        ss << "(";
        ss << m[0][0] << " ";
        ss << m[0][1] << " ";
        ss << m[0][2] << " ";
        ss << m[0][3] ;
        ss << ")";
        ss << ",";
        ss << "(";
        ss << m[1][0] << " ";
        ss << m[1][1] << " ";
        ss << m[1][2] << " ";
        ss << m[1][3] ;
        ss << ")";
        ss << ",";
        ss << "(";
        ss << m[2][0] << " ";
        ss << m[2][1] << " ";
        ss << m[2][2] << " ";
        ss << m[2][3] ;
        ss << ")"; 
        ss << ",";
        ss << "(";
        ss << m[3][0] << " ";
        ss << m[3][1] << " ";
        ss << m[3][2] << " ";
        ss << m[3][3] ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<FloatProperty*>(property)) {
        FloatProperty* prop = dynamic_cast<FloatProperty*>(property);
        ss << prop->get();
        propertyValue += ss.str();
    }
    else if (dynamic_cast<FloatMinMaxProperty*>(property)) {
        FloatMinMaxProperty* prop = dynamic_cast<FloatMinMaxProperty*>(property);
        vec2 value = prop->get();
        ss << "(";
        ss << value.x << ",";
        ss << value.y ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<FloatVec2Property*>(property)) {
        FloatVec2Property* prop = dynamic_cast<FloatVec2Property*>(property);
        vec2 value = prop->get();
        ss << "(";
        ss << value.x << ",";
        ss << value.y ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<FloatVec3Property*>(property)) {
        FloatVec3Property* prop = dynamic_cast<FloatVec3Property*>(property);
        vec3 value = prop->get();
        ss << "(";
        ss << value.x << ",";
        ss << value.y << ",";
        ss << value.z ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<FloatVec4Property*>(property)) {
        FloatVec4Property* prop = dynamic_cast<FloatVec4Property*>(property);
        vec4 value = prop->get();
        ss << "(";
        ss << value.x << ",";
        ss << value.y << ",";
        ss << value.z << ",";
        ss << value.w ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<IntMinMaxProperty*>(property)) {
        IntMinMaxProperty* prop = dynamic_cast<IntMinMaxProperty*>(property);
        ivec2 value = prop->get();
        ss << "(";
        ss << value.x << ",";
        ss << value.y ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<IntVec2Property*>(property)) {
        IntVec2Property* prop = dynamic_cast<IntVec2Property*>(property);
        ivec2 value = prop->get();
        ss << "(";
        ss << value.x << ",";
        ss << value.y ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<IntVec3Property*>(property)) {
        IntVec3Property* prop = dynamic_cast<IntVec3Property*>(property);
        ivec3 value = prop->get();
        ss << "(";
        ss << value.x << ",";
        ss << value.y << ",";
        ss << value.z ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<IntVec4Property*>(property)) {
        IntVec4Property* prop = dynamic_cast<IntVec4Property*>(property);
        ivec4 value = prop->get();
        ss << "(";
        ss << value.x << ",";
        ss << value.y << ",";
        ss << value.z << ",";
        ss << value.w ;
        ss << ")";
        propertyValue += ss.str();
    }
    else if (dynamic_cast<IntProperty*>(property)) {
        IntProperty* prop = dynamic_cast<IntProperty*>(property);
        ss << prop->get();
        propertyValue += ss.str();
    }
    else if (dynamic_cast<BaseOptionProperty*>(property)) {
        commandOpen =+ "# NOT IMPLEMENTED # " + commandOpen ;
    }
    else if (dynamic_cast<StringProperty*>(property)) {
        StringProperty* prop = dynamic_cast<StringProperty*>(property);
        ss << "\"";
        ss << prop->get();
        propertyValue += ss.str();
        ss << "\"";
    }
    else if (dynamic_cast<TransferFunctionProperty*>(property)) {
        commandOpen =+ "# NOT IMPLEMENTED # " + commandOpen ;
    }
    else {
        commandOpen =+ "# UNDETERMINED PROPERTY TYPE # " + commandOpen ;
    }

    if (propertyValue.empty())
        propertyCommand = commandOpen + processorName + "," + propertyName + commandClose + "\n";
    else
        propertyCommand = commandOpen + processorName + "," + propertyName + "," + propertyValue + commandClose + "\n";

    return propertyCommand;
}

} // namespace inviwo