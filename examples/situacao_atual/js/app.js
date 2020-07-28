am4core.ready(function() {
  am4core.useTheme(am4themes_animated);
  // Focos bar chart
  var barChart = am4core.create(`focus-bar-chart`, am4charts.XYChart);
  barChart.id = `focus-bar-chart`
  barChart.data = [{"date":"2015","value":183},{"date":"2016","value":1},{"date":"2017","value":184},{"date":"2018","value":4},{"date":"2019","value":1}];

  barChart.responsive.enabled = true;
  barChart.tapToActivate = true;

  var categoryAxis = barChart.xAxes.push(new am4charts.CategoryAxis());
  categoryAxis.dataFields.category = "date";
  categoryAxis.renderer.minGridDistance = 30;
  
  var valueAxis = barChart.yAxes.push(new am4charts.ValueAxis());
  valueAxis.title.text = "Focos";
  
  var barSeries = barChart.series.push(new am4charts.ColumnSeries());
  barSeries.dataFields.valueY = "value";
  barSeries.dataFields.categoryX = "date";
  barSeries.name = "Focos";
  barSeries.tooltipText = "Focos: {valueY}";
  barSeries.tooltip.pointerOrientation = "vertical";
  barSeries.tooltip.background.cornerRadius = 20;
  barSeries.tooltip.background.fillOpacity = 0.5;
  barSeries.tooltip.label.padding(12,12,12,12);
  barSeries.columns.template.adapter.add("fill", (fill, target)=>{
    return barChart.colors.getIndex(target.dataItem.index);
  })

  barSeries.columns.template.adapter.add("stroke", (stroke, target)=>{
    return barChart.colors.getIndex(target.dataItem.index);
  })

  barChart.cursor = new am4charts.XYCursor();
  barChart.cursor.xAxis = categoryAxis;
  barChart.cursor.snapToSeries = barSeries;

  barChart.scrollbarX = new am4core.Scrollbar();
  barChart.scrollbarY = new am4core.Scrollbar();

  // Sum area bar chart
  var barChart = am4core.create(`area-bar-chart`, am4charts.XYChart);
  barChart.id = `area-bar-chart`
  barChart.data = [{"date":"2015","value":33.66},{"date":"2017","value":27.33}, {"date":"2019","value":0.57}]

  barChart.responsive.enabled = true;
  barChart.tapToActivate = true;

  var categoryAxis = barChart.xAxes.push(new am4charts.CategoryAxis());
  categoryAxis.dataFields.category = "date";
  categoryAxis.renderer.minGridDistance = 30;
  
  var valueAxis = barChart.yAxes.push(new am4charts.ValueAxis());
  valueAxis.title.text = "Área (km²)";
  
  var barSeries = barChart.series.push(new am4charts.ColumnSeries());
  barSeries.dataFields.valueY = "value";
  barSeries.dataFields.categoryX = "date";
  barSeries.name = "Área";
  barSeries.tooltipText = "Área: {valueY.formatNumber('#,###.##')} km²";
  barSeries.tooltip.pointerOrientation = "vertical";
  barSeries.tooltip.background.cornerRadius = 20;
  barSeries.tooltip.background.fillOpacity = 0.5;
  barSeries.tooltip.label.padding(12,12,12,12);

  barSeries.columns.template.adapter.add("fill", (fill, target)=>{
    return barChart.colors.getIndex(target.dataItem.index);
  })

  barSeries.columns.template.adapter.add("stroke", (stroke, target)=>{
    return barChart.colors.getIndex(target.dataItem.index);
  })

  barChart.cursor = new am4charts.XYCursor();
  barChart.cursor.xAxis = categoryAxis;
  barChart.cursor.snapToSeries = barSeries;

  barChart.scrollbarX = new am4core.Scrollbar();
  barChart.scrollbarY = new am4core.Scrollbar();

  
  var pieChart = am4core.create("area-pie-chart1", am4charts.PieChart);
  pieChart.legend = new am4charts.Legend();
          
  pieChart.responsive.enabled = true;
  pieChart.tapToActivate = true;

  pieChart.data = [{"area": "Área queimada","value": 33.6},{"area": "Área imóvel","value": 69.64}];

  var pieSeries = pieChart.series.push(new am4charts.PieSeries());
  pieSeries.dataFields.value = "value";
  pieSeries.dataFields.category = "area";
  pieSeries.slices.template.stroke = am4core.color("#fff");
  pieSeries.slices.template.strokeWidth = 2;
  pieSeries.slices.template.strokeOpacity = 1;

  pieSeries.hiddenState.properties.opacity = 1;
  pieSeries.hiddenState.properties.endAngle = -90;
  pieSeries.hiddenState.properties.startAngle = -90;

  var pieChart = am4core.create("area-pie-chart2", am4charts.PieChart);
  pieChart.legend = new am4charts.Legend();
          
  pieChart.responsive.enabled = true;
  pieChart.tapToActivate = true;

  pieChart.data = [{"area": "Área queimada","value": 27.33},{"area": "Área imóvel","value": 69.64}];

  var pieSeries = pieChart.series.push(new am4charts.PieSeries());
  pieSeries.dataFields.value = "value";
  pieSeries.dataFields.category = "area";
  pieSeries.slices.template.stroke = am4core.color("#fff");
  pieSeries.slices.template.strokeWidth = 2;
  pieSeries.slices.template.strokeOpacity = 1;

  pieSeries.hiddenState.properties.opacity = 1;
  pieSeries.hiddenState.properties.endAngle = -90;
  pieSeries.hiddenState.properties.startAngle = -90;

  var pieChart = am4core.create("area-pie-chart3", am4charts.PieChart);

  pieChart.legend = new am4charts.Legend();
          
  pieChart.responsive.enabled = true;
  pieChart.tapToActivate = true;

  pieChart.data = [{"area": "Área queimada","value": 0.57},{"area": "Área imóvel","value": 69.64}];

  var pieSeries = pieChart.series.push(new am4charts.PieSeries());
  pieSeries.dataFields.value = "value";
  pieSeries.dataFields.category = "area";
  pieSeries.slices.template.stroke = am4core.color("#fff");
  pieSeries.slices.template.strokeWidth = 2;
  pieSeries.slices.template.strokeOpacity = 1;

  pieSeries.hiddenState.properties.opacity = 1;
  pieSeries.hiddenState.properties.endAngle = -90;
  pieSeries.hiddenState.properties.startAngle = -90;
});