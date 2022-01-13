#ifndef slic3r_TextConfiguration_hpp_
#define slic3r_TextConfiguration_hpp_

#include <vector>
#include <string>
#include <optional>

namespace Slic3r {

// user defined font property
struct FontProp
{
    // define extra space between letters, negative mean closer letter
    std::optional<int> char_gap;
    // define extra space between lines, negative mean closer lines
    std::optional<int> line_gap;
    // Z depth of text [in mm]
    float emboss;

    // positive value mean wider character shape
    // negative value mean tiner character shape
    std::optional<float> boldness; // [in mm]

    // positive value mean italic of character (CW)
    // negative value mean CCW skew (unItalic)
    std::optional<float> skew;

    // TODO: add enum class Align: center/left/right

    //////
    // Duplicit data to wxFontDescriptor
    // used for store/load .3mf file
    //////

    // Height of letter [in mm],
    // duplicit to wxFont::PointSize
    float size_in_mm;
    // Define type of font
    std::optional<std::string> family;
    std::optional<std::string> face_name;
    std::optional<std::string> style;
    std::optional<std::string> weight;

    FontProp(float line_height = 10.f, float depth = 2.f)
        : emboss(depth), size_in_mm(line_height)
    {}

    bool operator==(const FontProp& other) const {
        auto is_equal = [](const float &v1, const float &v2) { 
            return fabs(v1 - v2) < std::numeric_limits<float>::epsilon();
        };
        auto is_equal_ = [&is_equal](const std::optional<float> &v1,
                            const std::optional<float> &v2) {
            return (!v1.has_value() && !v2.has_value()) ||
                   (v1.has_value() && v2.has_value() && is_equal(*v1, *v2));
        };
        return 
            char_gap == other.char_gap && 
            line_gap == other.line_gap &&
            is_equal(emboss, other.emboss) &&
            is_equal(size_in_mm, other.size_in_mm) && 
            is_equal_(boldness, other.boldness) &&
            is_equal_(skew, other.skew);
    }

    // undo / redo stack recovery
    template<class Archive> void serialize(Archive &ar)
    {
        ar(char_gap, line_gap, emboss, boldness, skew, size_in_mm, family, face_name, style, weight);
    }
};

// represent selected font
// Name must be human readable is visible in gui
// (Path + Type) must define how to open font for using on different OS
// NOTE: OnEdit fix serializations: FontListSerializable, TextConfigurationSerialization
struct FontItem
{
    std::string name;
    std::string path;
    enum class Type;
    Type type;
    FontProp prop;

    FontItem() : type(Type::undefined){} // set undefined type

    // when name is empty than Font item was loaded from .3mf file 
    // and potentionaly it is not reproducable
    FontItem(const std::string &name,
             const std::string &path,
             Type               type,
             const FontProp &   prop)
        : name(name), path(path), type(type), prop(prop)
    {}

    // define data stored in path
    enum class Type { 
        undefined = 0,
        file_path, // TrueTypeFont file loacation on computer - for privacy: path is NOT stored into 3mf
        // wx font descriptors are platform dependent
        wx_win_font_descr, // path is font descriptor generated by wxWidgets on windows
        wx_lin_font_descr, // path is font descriptor generated by wxWidgets on windows
        wx_mac_font_descr // path is font descriptor generated by wxWidgets on windows
    };

    // undo / redo stack recovery
    template<class Archive> void serialize(Archive &ar)
    {
        ar(name, path, (int) type, prop);
    }
};

// Font item name inside list is unique
// FontList is not map beacuse items order matters (view of list)
// It is stored into AppConfig by FontListSerializable
using FontList = std::vector<FontItem>;

// define how to create 'Text volume'
// It is stored into .3mf by TextConfigurationSerialization
// Also it is stored into undo / redo stack by cereal
struct TextConfiguration
{
    // define font
    FontItem font_item;

    std::string text = "None";

    TextConfiguration() = default; // optional needs empty constructor
    TextConfiguration(const FontItem &font_item, const std::string &text)
        : font_item(font_item), text(text)
    {}

    // undo / redo stack recovery
    template<class Archive> void serialize(Archive &ar){ ar(text, font_item); }
};    

} // namespace Slic3r

#endif // slic3r_TextConfiguration_hpp_
