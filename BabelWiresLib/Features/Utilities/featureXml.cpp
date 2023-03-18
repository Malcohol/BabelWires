#include <BabelWiresLib/Features/Utilities/featureXml.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/feature.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>
#include <BabelWiresLib/Features/stringFeature.hpp>
#include <BabelWiresLib/FileFormat/fileFeature.hpp>
#include <BabelWiresLib/ValueNames/valueNames.hpp>

#include <tinyxml2.h>

namespace {

    struct FeatureToXml {
        FeatureToXml(bool writeTrackEvents)
            : m_writeTrackEvents(writeTrackEvents) {}

        void intFeatureToXml(const babelwires::IntFeature& feature) {
            m_xml.OpenElement("INT");
            const int value = feature.get();
            { m_xml.PushAttribute("VALUE", std::to_string(value).c_str()); }
            if (const babelwires::ValueNames* valueNames = feature.getValueNames()) {
                std::string name;
                if (valueNames->getNameForValue(value, name)) {
                    m_xml.PushAttribute("VALUE_NAME", name.c_str());
                }
            }
            m_xml.CloseElement();
        }

        void rationalFeatureToXml(const babelwires::RationalFeature& feature) {
            m_xml.OpenElement("RATIONAL");
            const babelwires::Rational value = feature.get();
            { m_xml.PushAttribute("VALUE", value.toString().c_str()); }
            m_xml.CloseElement();
        }

        void stringFeatureToXml(const babelwires::StringFeature& feature) {
            m_xml.OpenElement("STRING");
            std::string value = feature.get();
            { m_xml.PushAttribute("VALUE", value.c_str()); }
            m_xml.CloseElement();
        }

        void recordFeatureToXml(const babelwires::RecordFeature& feature) {
            m_xml.OpenElement("RECORD");
            for (int i = 0; i < feature.getNumFeatures(); ++i) {
                m_xml.OpenElement("FIELD");
                m_xml.PushAttribute("IDENTIFIER", feature.getFieldIdentifier(i).serializeToString().c_str());
                const babelwires::Feature* subFeature = feature.getFeature(i);
                featureToXml(*subFeature);
                m_xml.CloseElement();
            }
            m_xml.CloseElement();
        }

        void arrayFeatureToXml(const babelwires::ArrayFeature& feature) {
            m_xml.OpenElement("ARRAY");
            for (int i = 0; i < feature.getNumFeatures(); ++i) {
                m_xml.OpenElement("ENTRY");
                m_xml.PushAttribute("INDEX", std::to_string(i).c_str());
                const babelwires::Feature* subFeature = feature.getFeature(i);
                featureToXml(*subFeature);
                m_xml.CloseElement();
            }
            m_xml.CloseElement();
        }

        void featureToXml(const babelwires::Feature& feature) {
            if (auto intFeature = feature.as<const babelwires::IntFeature>()) {
                intFeatureToXml(*intFeature);
            } else if (auto ratFeature = feature.as<const babelwires::RationalFeature>()) {
                rationalFeatureToXml(*ratFeature);
            } else if (auto stringFeature = feature.as<const babelwires::StringFeature>()) {
                stringFeatureToXml(*stringFeature);
            } else if (auto recordFeature = feature.as<const babelwires::RecordFeature>()) {
                recordFeatureToXml(*recordFeature);
            } else if (auto arrayFeature = feature.as<const babelwires::ArrayFeature>()) {
                arrayFeatureToXml(*arrayFeature);
            } else {
                assert(!"Unrecognized feature encountered");
            }
        }

        void write(std::ostream& stream) { stream << m_xml.CStr(); }

        tinyxml2::XMLPrinter m_xml;
        bool m_writeTrackEvents;
    };
} // namespace

void babelwires::featureToXml(const babelwires::Feature& feature, std::ostream& stream, bool writeTrackEvents) {
    FeatureToXml writer(writeTrackEvents);
    writer.featureToXml(feature);
    writer.write(stream);
}
