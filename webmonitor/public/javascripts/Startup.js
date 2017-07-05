requirejs.config({
  baseUrl: BASE_URL + 'javascripts',
  paths: {
    TerraMA2WebComponents: BASE_URL + 'TerraMA2WebComponents.min'
  }
});

requirejs(
  ['TerraMA2WebMonitor', 'components/Calendar', 'components/Slider', 'TerraMA2WebComponents'],
  function(TerraMA2WebMonitor, Calendar, Slider, TerraMA2WebComponents) {
    TerraMA2WebComponents.LayerExplorer.init();
    TerraMA2WebComponents.MapDisplay.init();

    TerraMA2WebMonitor.init();
    Calendar.init();
    Slider.init();
  }
);
