/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2015 Inviwo Foundation
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
 * Contact: Erik Sund�n
 *
 *********************************************************************************/

#include <modules/pvm/mpvmvolumereader.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumetypeclassification.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/formatconversion.h>
#include <inviwo/core/util/stringconversion.h>
#include "ext/tidds/ddsbase.h"
#include "pvmvolumereader.h"

namespace inviwo {

MPVMVolumeReader::MPVMVolumeReader()
    : DataReaderType<Volume>() {
    addExtension(FileExtension("mpvm", "MPVM (Multiple PVMs) file format"));
}

MPVMVolumeReader::MPVMVolumeReader(const MPVMVolumeReader& rhs)
    : DataReaderType<Volume>(rhs) {};

MPVMVolumeReader& MPVMVolumeReader::operator=(const MPVMVolumeReader& that) {
    if (this != &that) {
        DataReaderType<Volume>::operator=(that);
    }

    return *this;
}

MPVMVolumeReader* MPVMVolumeReader::clone() const { return new MPVMVolumeReader(*this); }

Volume* MPVMVolumeReader::readMetaData(std::string filePath) {
    if (!filesystem::fileExists(filePath)) {
        std::string newPath = filesystem::addBasePath(filePath);

        if (filesystem::fileExists(newPath)) {
            filePath = newPath;
        }
        else {
            throw DataReaderException("Error could not find input file: " + filePath);
        }
    }

    std::string fileDirectory = filesystem::getFileDirectory(filePath);

    // Read the mpvm file content
    std::istream* f = new std::ifstream(filePath.c_str());
    std::string textLine;
    std::vector<std::string> files;

    while (!f->eof()) {
        getline(*f, textLine);
        textLine = trim(textLine);
        files.push_back(textLine);
    };

    delete f;

    if (files.empty())
        throw DataReaderException("Error: No PVM files found in " +
        filePath);

    if (files.size() > 4)
        throw DataReaderException("Error: Maximum 4 pvm files are supported, file: " +
        filePath);

    // Read all pvm volumes
    std::vector<Volume*> volumes;
    for (int i = 0; i < files.size(); i++){
        Volume* newVol = PVMVolumeReader::readPVMData(fileDirectory + files[i]);
        if (newVol)
            volumes.push_back(newVol);
        else
            LogWarn("Could not load " << fileDirectory << files[i]);
    }

    if (volumes.empty())
        throw DataReaderException("No PVM volumes could be read from file: " +
        filePath);

    if (volumes.size() == 1){
        printPVMMeta(volumes[0], fileDirectory + files[0]);
        return volumes[0];
    }

    // Make sure dimension and format match
    const DataFormatBase* format = volumes[0]->getDataFormat();
    uvec3 mdim = volumes[0]->getDimension();
    bool returnFirst = false;
    for (int i = 1; i < volumes.size(); i++){
        if (format != volumes[i]->getDataFormat()) {
            if ((format == DataUINT12::get() && volumes[i]->getDataFormat() == DataUINT16::get())
                || (format == DataUINT16::get() && volumes[i]->getDataFormat() == DataUINT12::get())) {
                format = DataUINT16::get();
            } else {
                returnFirst = true;
            }
        }

        if (mdim != volumes[i]->getDimension())
            returnFirst = true;

        if (returnFirst)
            break;
    }

    if (returnFirst){
        LogWarn("PVM volumes did not have the same format or dimension, using first volume.");
        printPVMMeta(volumes[0], fileDirectory + files[0]);
        return volumes[0];
    }

    // Create new format
    const DataFormatBase* mformat = DataFormatBase::get(format->getNumericType(), volumes.size(), format->getBitsStored());

    // Create new volume
    Volume* volume = new UniformRectiLinearVolume();
    glm::mat3 basis = volumes[0]->getBasis();
    volume->setBasis(basis);
    volume->setOffset(-0.5f*(basis[0] + basis[1] + basis[2]));
    volume->setDimension(mdim);
    volume->dataMap_.initWithFormat(mformat);
    volume->setDataFormat(mformat);
    volume->copyMetaDataFrom(*volumes[0]);

    // Merge descriptions but ignore the rest
    StringMetaData* metaData = volume->getMetaData<StringMetaData>("description");
    if (metaData){
        std::string descStr = metaData->get();
        for (int i = 1; i < volumes.size(); i++){
            metaData = volumes[0]->getMetaData<StringMetaData>("description");
            if (metaData) descStr = descStr + ", " + metaData->get();
        }
        volume->setMetaData<StringMetaData>("description", descStr);
    }

    // Create RAM volume
    VolumeRAM* mvolRAM = createVolumeRAM(mdim, mformat);
    unsigned char* dataPtr = static_cast<unsigned char*>(mvolRAM->getData());

    std::vector<const unsigned char*> volumesDataPtr;
    for (int i = 0; i < volumes.size(); i++){
        volumesDataPtr.push_back(static_cast<const unsigned char*>(volumes[i]->getRepresentation<VolumeRAM>()->getData()));
    }

    // Copy the data from the other volumes to the new multichannel volume
    size_t mbytes = mformat->getBytesAllocated();
    size_t bytes = format->getBytesAllocated();
    size_t dims = mdim.x*mdim.y*mdim.z;
    size_t vsize = volumesDataPtr.size();
    for (size_t i = 0; i < dims; i++) {
        for (size_t j = 0; j < vsize; j++) {
            for (size_t b = 0; b < bytes; b++) {
                dataPtr[i*mbytes + (j*bytes) + b] = volumesDataPtr[j][i*bytes + b];
            }
        }
    }

    // Delete the single channel volumes
    for (int i = 0; i < volumes.size(); i++){
        delete volumes[i];
    }

    volume->addRepresentation(mvolRAM);

    printPVMMeta(volume, filePath);
    return volume;
}

void MPVMVolumeReader::readDataInto(void*) const {
    return;
}

void* MPVMVolumeReader::readData() const {
    return NULL;
}

void MPVMVolumeReader::printPVMMeta(Volume* volume, std::string filePath){
    uvec3 dim = volume->getDimension();
    size_t bytes = dim.x * dim.y * dim.z * (volume->getDataFormat()->getBytesAllocated());
    std::string size = formatBytesToString(bytes);
    LogInfo("Loaded volume: " << filePath << " size: " << size);
    printMetaInfo(volume, "description");
    printMetaInfo(volume, "courtesy");
    printMetaInfo(volume, "parameter");
    printMetaInfo(volume, "comment");
}

void MPVMVolumeReader::printMetaInfo(MetaDataOwner* metaDataOwner, std::string key){
    StringMetaData* metaData = metaDataOwner->getMetaData<StringMetaData>(key);
    if (metaData){
        std::string metaStr = metaData->get();
        replaceInString(metaStr, "\n", ", ");
        key[0] = toupper(key[0]);
        LogInfo(key << ": " << metaStr);
    }
}

}  // namespace