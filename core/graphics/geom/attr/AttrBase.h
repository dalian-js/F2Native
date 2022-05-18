#ifndef XG_GRAPHICS_GEOM_ATTR_H
#define XG_GRAPHICS_GEOM_ATTR_H

#include <map>
#include <math.h>
#include <string>
#include <vector>
#include "../../canvas/Coord.h"
#include "../../scale/Category.h"
#include "../../scale/Scale.h"

using namespace xg::canvas::coord;
using namespace xg::scale;
using namespace xg::util;
using namespace std;

namespace xg {
namespace attr {

enum class AttrType { Position = 1, Color, Size, Shape, Adjust };

class AttrBase {
  public:
    AttrBase(const vector<string> &fields, const vector<string> &names) : fields_(fields), names_(names) {}

    virtual ~AttrBase() = default;

    const vector<string> &GetNames() const { return names_; }
    const vector<string> &GetFields() const { return fields_; }

    virtual AttrType GetType() const = 0;

    virtual void
    Mapping(nlohmann::json &groupData, std::size_t start, std::size_t end, AbstractScale &xScale, AbstractScale &yScale, const AbstractCoord &coord){};

  protected:
    vector<string> fields_;
    vector<string> names_;
};

class Position : public AttrBase {
  public:
    Position(const vector<string> &fields) : AttrBase(fields, {"x", "y"}) {
        // if(this->fields_.size() <= 0 || this->fields_.size() > 2) {
        // throw runtime_error("fields is invalid.");
        // }
    }

    AttrType GetType() const override { return AttrType::Position; }

    void Mapping(nlohmann::json &groupData, std::size_t start, std::size_t end, AbstractScale &xScale, AbstractScale &yScale, const AbstractCoord &coord) override {
        for(size_t i = start; i <= end; i++) {
            auto &item = groupData[i];
            nlohmann::json &xVal = item[fields_[0]];
            nlohmann::json &yVal = item[fields_[1]];

            if(xVal.is_null() && yVal.is_null()) {
                item["_x"] = std::nan("0"); // attr names[x, y]
                item["_y"] = std::nan("0");
            } else if(xVal.is_array() && yVal.is_array()) {
                // TODO
            } else if(xVal.is_array()) {
                // TODO
            } else if(yVal.is_array()) {
                double x = xScale.Scale(item[fields_[0]]);

                double rstX;
                nlohmann::json rstY;
                for(std::size_t index = 0; index < yVal.size(); ++index) {
                    util::Point _point = coord.ConvertPoint(util::Point{x, yScale.Scale(yVal[index])});
                    rstX = _point.x;
                    rstY.push_back(_point.y);
                }
                item["_x"] = rstX;
                item["_y"] = rstY;
            } else {
                double x = xScale.Scale(item[fields_[0]]);
                double y = yScale.Scale(item[fields_[1]]);
                util::Point point = coord.ConvertPoint(util::Point(x, y));
                item["_x"] = point.x; // attr names[x, y]
                item["_y"] = point.y;
            }
        }
    }
};

class Color : public AttrBase {
  public:
    Color(const vector<string> &fields, const vector<string> &colors)
        : AttrBase(fields, {"color"}), colors_(colors){
                                           // if(fields_.size() != 1) {
                                           // throw runtime_error("fields must be one element.");
                                           // }
                                       };

    Color(const string &color) : AttrBase({}, {"color"}) { colors_.push_back(color); }

    AttrType GetType() const override { return AttrType::Color; }

    inline const string &GetColor(int index) const { return index < colors_.size() ? colors_[index] : colors_[0]; }

    void Mapping(nlohmann::json &groupData, std::size_t start, std::size_t end, AbstractScale &xScale, AbstractScale &yScale, const AbstractCoord &coord) override {
        for(size_t i = start; i <= end; i++) {
            auto &item = groupData[i];
            if(!fields_.empty() && scale::IsCategory(xScale.GetType())) {
                const scale::Category &cat = (scale::Category &)xScale;
                std::size_t index = cat.Transform(item[fields_[0]]);
                item["_color"] = colors_[index];
            } else {
                item["_color"] = colors_[0];
            }
        }
    }

  private:
    vector<string> colors_{};
};

class Size : public AttrBase {
  public:
    Size(const string &field, const vector<float> &sizes) : AttrBase({field}, {"color"}), sizes_(sizes) {}

    Size(const float size) : AttrBase({}, {"size"}) { sizes_.push_back(size); }

    inline const float GetSize(int index) const { return index < sizes_.size() ? sizes_[index] : sizes_[0]; }

    AttrType GetType() const override { return AttrType::Size; }

    void Mapping(nlohmann::json &groupData, std::size_t start, std::size_t end, AbstractScale &xScale, AbstractScale &yScale, const AbstractCoord &coord) override {
        for(std::size_t index = start; index <= end; ++index) {
            nlohmann::json &item = groupData[index];
            if(scale::IsCategory(xScale.GetType())) {
                std::size_t val = static_cast<scale::Category &>(xScale).Transform(item[xScale.field]);
                item["_size"] = sizes_[val % sizes_.size()];
            } else {
                double percent = xScale.Scale(item[xScale.field]);
                item["_size"] = sizes_[GetLinear(percent) % sizes_.size()];
            }
        }
    }

  private:
    std::size_t GetLinear(double percent) {
        std::size_t steps = sizes_.size() - 1;
        std::size_t step = static_cast<std::size_t>(floor(steps * percent));
        double leftPercent = steps * percent - step;
        std::size_t start = sizes_[step];
        std::size_t end = step == steps ? start : sizes_[step + 1];
        std::size_t rst = static_cast<std::size_t>(start + (end - start) * leftPercent);
        return rst;
    }

  private:
    vector<float> sizes_{};
};

class Shape : public AttrBase {
  public:
    Shape(const string &field, const vector<string> &shapes) : AttrBase({field}, {"shape"}), shapes_(shapes){};

    Shape(const string &shape) : AttrBase({}, {"shape"}) { shapes_.push_back(shape); }

    AttrType GetType() const override { return AttrType::Shape; }

    // inline const string &GetShape(int index) const { return index < shapes_.size() ? shapes_[index] : shapes_[0]; }

    void Mapping(nlohmann::json &groupData, std::size_t start, std::size_t end, AbstractScale &xScale, AbstractScale &yScale, const AbstractCoord &coord) override {
        if(shapes_.empty()) {
            return;
        }
        for(std::size_t index = start; index <= end; ++index) {
            groupData[index]["_shape"] = shapes_[0];
        }
    }

  private:
    vector<string> shapes_{};
};

class Adjust : public AttrBase {
  public:
    Adjust(const string &field, const string &adjust) : AttrBase({field}, {"adjust"}), adjust_(adjust){};

    AttrType GetType() const override { return AttrType::Adjust; }

    inline const string &GetAdjust() const { return adjust_; }

    void Mapping(nlohmann::json &groupData, std::size_t start, std::size_t end, AbstractScale &xScale, AbstractScale &yScale, const AbstractCoord &coord) override {
        for(std::size_t index = start; index <= end; ++index) {
            groupData[index]["_adjust"] = adjust_;
        }
    }

  private:
    string adjust_;
};

} // namespace attr
} // namespace xg

#endif // XG_GRAPHICS_GEOM_ATTR_H
