#include "control.hpp"

ts::controls::Control ts::controls::control_from_string(const std::string& control_string)
{
    using controls::Control;

    if (control_string == "accelerate") return Control::Accelerate;
    if (control_string == "brake") return Control::Brake;
    if (control_string == "left") return Control::Left;
    if (control_string == "right") return Control::Right;
    if (control_string == "fire") return Control::Fire;
    if (control_string == "alt_fire") return Control::Alt_fire;

    return Control::None;
}