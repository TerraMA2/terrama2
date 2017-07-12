requirejs.config({
  baseUrl: BASE_URL + 'javascripts',
  paths: {
    TerraMA2WebComponents: BASE_URL + 'TerraMA2WebComponents.min'
  }
});

requirejs(
  ['TerraMA2WebMonitor',
  'components/Calendar',
  'components/Slider',
  'components/AttributesTable',
  'components/Legends',
  'components/GetAttributesTool',
  'components/MapTools',
  'components/LayerStatus',
  'components/Utils',
  'components/AddLayerByUri',
  'TerraMA2WebComponents'],
  function(TerraMA2WebMonitor, Calendar, Slider, AttributesTable, Legends, GetAttributesTool,
            MapTools, LayerStatus, Utils, AddLayerByUri, TerraMA2WebComponents) {
    TerraMA2WebComponents.LayerExplorer.init();
    TerraMA2WebComponents.MapDisplay.init();
    
    Utils.init();
    TerraMA2WebMonitor.init();
    LayerStatus.init();
    Calendar.init();
    Slider.init();
    AttributesTable.init();
    Legends.init();
    GetAttributesTool.init();
    MapTools.init();
    AddLayerByUri.init();
  }
);
