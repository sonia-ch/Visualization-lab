/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2014 Inviwo Foundation
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
 * Contact: Sathish Kottravel
 *
 *********************************************************************************/

#ifndef  IVW_LINKDIALOG_PROCESSORGRAPHICSITEMS_H
#define  IVW_LINKDIALOG_PROCESSORGRAPHICSITEMS_H

#include <inviwo/qt/editor/inviwoqteditordefine.h>
#include <QGraphicsView>
#include <QDialog>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QStandardItemModel>
#include <QEventLoop>
#include <QCheckBox>

#include <inviwo/core/links/processorlink.h>
#include <inviwo/core/links/linkconditions.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/qt/widgets/labelgraphicsitem.h>
#include <inviwo/qt/editor/connectiongraphicsitem.h>
#include <inviwo/qt/widgets/inviwodockwidget.h>

#include <inviwo/qt/editor/linkdialog/linkdialoggraphicsitems.h>

namespace inviwo {

class LinkDialogPropertyGraphicsItem;

class IVW_QTEDITOR_API LinkDialogProcessorGraphicsItem : public GraphicsItemData<Processor> {

public:
    LinkDialogProcessorGraphicsItem();
    ~LinkDialogProcessorGraphicsItem();

    void setProcessor(Processor* processor, bool expandProperties=false);
    Processor* getProcessor() {return getGraphicsItemData();}

    void updatePropertyItemPositions();

    const std::vector<LinkDialogPropertyGraphicsItem*>& getPropertyItemList() const {return propertyGraphicsItems_;}

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    //override for qgraphicsitem_cast (refer qt documentation)
    enum { Type = UserType + LinkDialogProcessorGraphicsItemType };
    int type() const  {return Type; }

protected:
    virtual void paint(QPainter* p, const QStyleOptionGraphicsItem* options, QWidget* widget);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    LabelGraphicsItem* nameLabel_;
    LabelGraphicsItem* classLabel_;
    std::vector<LinkDialogPropertyGraphicsItem*> propertyGraphicsItems_;
};

} //namespace

#endif //IVW_LINKDIALOG_PROCESSORGRAPHICSITEMS_H