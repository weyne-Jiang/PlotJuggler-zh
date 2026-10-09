// SPDX-License-Identifier: MPL-2.0
function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/plotjuggler.exe",
            "@StartMenuDir@/PlotJuggler 3.17.2 中文版.lnk",
            "workingDirectory=@TargetDir@", "description=PlotJuggler 3.17.2 中文版");
        component.addOperation("CreateShortcut", "@TargetDir@/plotjuggler.exe",
            "@DesktopDir@/PlotJuggler 3.17.2 中文版.lnk",
            "workingDirectory=@TargetDir@", "description=PlotJuggler 3.17.2 中文版");
    }
};
