--this is example plugin

function pluginName()
    --steppers move example
    stepper["X"].move(10)
    stepper["Y"].move(10)
    stepper["Z"].move(10)
    stepper["HOTEND"].move(10)

    --steppers stop example
    stepper["X"].stop()
    stepper["Y"].stop()
    stepper["Z"].stop()
    stepper["HOTEND"].stop()
end