{
  baseUrl: ".",
  out: "../dist/TerraMA2WebComponents.min.js",
  preserveLicenseComments: false,
  findNestedDependencies: true,
  paths: {
    TerraMA2WebComponentsPath: "./src"
  },
  include: [
    'externals/almond',
    'TerraMA2WebComponentsPath/TerraMA2WebComponents',
    'TerraMA2WebComponentsPath/components/LayerExplorer.TerraMA2WebComponents',
    'TerraMA2WebComponentsPath/components/MapDisplay.TerraMA2WebComponents'
  ],
  wrap: {
    startFile: "Wrap.TerraMA2WebComponents.start",
    endFile: "Wrap.TerraMA2WebComponents.end"
  }
}
