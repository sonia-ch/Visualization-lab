#ifndef IVW_ORDINALPROPERTYANIMATOR_H
#define IVW_ORDINALPROPERTYANIMATOR_H

#include <modules/base/basemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/util/timer.h>
#include <inviwo/core/properties/propertyfactory.h>

namespace inviwo {

class IVW_MODULE_BASE_API OrdinalPropertyAnimator : public Processor {
public:
    OrdinalPropertyAnimator();
    virtual ~OrdinalPropertyAnimator();

    InviwoProcessorInfo();

    void initialize();
    void deinitialize();

protected:
    virtual void process();

    void updateTimerInterval();
    void timerEvent();
    void changeProperty();
    void changeActive();

private:
    
    struct BaseProp {
        BaseProp(std::string classname,
                 std::string displayName)
            : classname_(classname)
            , displayName_(displayName) {
        }
        virtual ~BaseProp() {}
        std::string classname_;
        std::string displayName_;

        virtual Property* getProp() = 0;
        virtual Property* getDelta() = 0;
        virtual void update(bool pbc) = 0;
    };

    template <typename T>
    struct VecProp : public BaseProp {
        VecProp(std::string classname, std::string displayName)
        : BaseProp(classname, displayName)
        , prop_(NULL)
        , delta_(NULL) {

            prop_ = dynamic_cast<OrdinalProperty<T>*>(
                PropertyFactory::getPtr()->getProperty(classname, classname, displayName));

            std::stringstream ss1;
            ss1 << classname << "-" << "Delta";
            std::string identifier = ss1.str();

            delta_ = dynamic_cast<OrdinalProperty<T>*>(
                PropertyFactory::getPtr()->getProperty(classname, identifier, "Delta"));

            prop_->onChange(this, &VecProp<T>::setLimits);

        }

        void setLimits() {
            T max = prop_->getMaxValue();

            T dmin = delta_->getMinValue();
            T dmax = delta_->getMaxValue();

            T newMin = -T(static_cast<typename T::value_type>(0.1))*max;
            T newMax =  T(static_cast<typename T::value_type>(0.1))*max;

            if (dmin != newMin) {
                delta_->setMinValue(newMin);
            }
            if (dmax != newMax) {
                delta_->setMaxValue(newMax);
            }
        }

        virtual ~VecProp() {
            delete prop_;
            delete delta_;
        }

        virtual Property* getProp() {
            return prop_;
        }
        virtual Property* getDelta() {
            return delta_;
        }
        virtual void update(bool pbc) {
            T p = prop_->get();
            T d = delta_->get();
            T r = p+d;
            for (glm::length_t i = 0; i<static_cast<glm::length_t>(prop_->getDim().x); ++i) {
                if (r[i]>prop_->getMaxValue()[i]) {
                    if (pbc) {
                        r[i] = r[i] - (prop_->getMaxValue()[i] - prop_->getMinValue()[i]);
                    } else {
                        r[i] = prop_->getMaxValue()[i];
                    }
                } else if (r[i] < prop_->getMinValue()[i]) {
                    if (pbc) {
                        r[i] = r[i] + (prop_->getMaxValue()[i] - prop_->getMinValue()[i]);
                    } else {
                        r[i] = prop_->getMinValue()[i];
                    }
                }
            }
            if (r != p) {
                prop_->set(r);
            }
        }

        OrdinalProperty<T>* prop_;
        OrdinalProperty<T>* delta_;
        typedef T valueType;
    };


    template <typename T>
    struct PrimProp : public BaseProp {
        PrimProp(std::string classname, std::string displayName)
        : BaseProp(classname, displayName)
        , prop_(NULL)
        , delta_(NULL) {

            prop_ = dynamic_cast<OrdinalProperty<T>*>(
                PropertyFactory::getPtr()->getProperty(classname, classname, displayName));

            std::stringstream ss1;
            ss1 << classname << "-" << "Delta";
            std::string identifier = ss1.str();

            delta_ = dynamic_cast<OrdinalProperty<T>*>(
                PropertyFactory::getPtr()->getProperty(classname, identifier, "Delta"));

            prop_->onChange(this, &PrimProp<T>::setLimits);
        }

        void setLimits() {
            T max = prop_->getMaxValue();

            T dmin = delta_->getMinValue();
            T dmax = delta_->getMaxValue();

            if (dmin != - max / 10) {
                delta_->setMinValue(-max / 10);
            }
            if (dmax != max / 10) {
                delta_->setMaxValue(max/10);
            }        
        }

        virtual ~PrimProp() {
            delete prop_;
            delete delta_;
        }

        virtual Property* getProp() {
            return prop_;
        }
        virtual Property* getDelta() {
            return delta_;
        }
        virtual void update(bool pbc) {
            T p = prop_->get();
            T d = delta_->get();
            T r = p + d;
            
            if (r>prop_->getMaxValue()) {
                if (pbc) {
                    r = r - (prop_->getMaxValue() - prop_->getMinValue());
                } else {
                    r = prop_->getMaxValue();
                }
            } else if (r < prop_->getMinValue()) {
                if (pbc) {
                    r = r + (prop_->getMaxValue() - prop_->getMinValue());
                } else {
                    r = prop_->getMinValue();
                }
            }
            
            if (r != p) {
                prop_->set(r);
            }
        }

        OrdinalProperty<T>* prop_;
        OrdinalProperty<T>* delta_;
        typedef T valueType;
    };

    OptionPropertyInt type_;
    IntProperty delay_;
    BoolProperty pbc_;
    BoolProperty active_;
    Timer* timer_;

    std::vector<BaseProp*> properties_;
};

} // namespace

#endif // IVW_ORDINALPROPERTYANIMATOR_H
