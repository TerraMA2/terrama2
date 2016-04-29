({
  name: 'almond',
  baseUrl: ".",
  out: "TerraMA2WebComponents.min.js",
  paths: {
    TerraMA2WC: "."
  },
  include: [
    'TerraMA2WC/TerraMA2WebComponents',
    'TerraMA2WC/components/LayerExplorer.TerraMA2WebComponents',
    'TerraMA2WC/components/MapDisplay.TerraMA2WebComponents'
  ],
  wrap: true
  
})
