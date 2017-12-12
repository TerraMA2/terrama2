(function(){
  "use strict";

  /**
   * Create a chart
   * @param {JSON} data - Value to fill the chart
   * @param {String} type - Type of char (http://www.chartjs.org/docs/latest/charts/)
   * @param {Object} element - Html element
   * @param {String} labelKey - Object key that represents the label
   * @param {String} valueKey - Object key that represents the value
   */
  function createChart(data, type, element, labelKey, valueKey){
    var labels = data.map(function(dataElem){
      return dataElem[labelKey];
    });
    var values = data.map(function(dataElem){
      return dataElem[valueKey];
    });

    var options = {};

    if (type == "bar" || type =="horizontalBar"){
      options = {
        legend: {display: false},
        title: {
          display: true,
          text: 'Contagem de focos'
        },
        scales: {
          yAxes: [{
            ticks: {
              beginAtZero:true
            }
          }]
        }
      };
    }
    var char = new Chart(element, {
      type: type,
      data: {
        labels: labels,
        datasets: [{
          label: "Focos",
          backgroundColor: 'rgb(247, 195, 121)',
          data: values
        }]
      },
      options: options
    });
  }

  /**
   * Create a table
   * @param {JSON} data - Value to fill the table
   * @param {String} element - Html element
   * @param {String} nameKey - Object key that represents the name
   * @param {String} valueKey - Object key that represents the value
   */
  function createTable(data, element, nameKey, valueKey){
    for (var i = 0; i < data.length; i++){
      var index = i + 1;
      var tableLine = "<tr><th scope=\"row\">" + index + "</th><td>"+data[i][nameKey]+"</td><td>"+data[i][valueKey]+"</td></tr>";
      element.append(tableLine);
    }
  }

  /**
   * Normalize data to create chart and table
   * @param {JSON} data - Data to be normalized
   * @param {Array} fieldsToNormalize - Arrays field to normalize
   * @return {JSON} [{name: String, quantidade: Number}]
   */
  function normalizeData(data, fieldsToNormalize){
    var dataNormalized = [];
    if (data && data.features && Array.isArray(data.features)){
      data.features.forEach(function(feature){
        var featureNormalized = {};
        fieldsToNormalize.forEach(function(field){
          if (feature.properties.hasOwnProperty(field)){
            featureNormalized[field] = feature.properties[field];
          }
        });
        if (feature.properties.ocorrencia != null)
          dataNormalized.push(featureNormalized);
      });
      return dataNormalized;
    } else {
      return dataNormalized;
    }
  }

  /**
   * 
   * @param {JSON} data - Data to create chart and table
   * @param {String} tableId - Html element id of table
   * @param {String} chartId - Html element id of chart
   * @param {String} chartType - Chart type
   * @param {Array} fieldsToFill - Arrays field to fill table and chart
   */
  function createDataObjects(data, tableId, chartId, chartType, fieldsToFill){
    var normalizedData = normalizeData(data, fieldsToFill);
    var tableBodyElement = $("#"+tableId+" tbody");
    createTable(normalizedData, tableBodyElement, fieldsToFill[0], fieldsToFill[1]);
    var chartElement = document.getElementById(chartId).getContext('2d');
    createChart(normalizedData, chartType, chartElement, fieldsToFill[0], fieldsToFill[1]);
  }

  $(document).ready(function() {
    var fields = ["monitored_nome", "ocorrencia"];

    var ajaxUrl = "http://localhost:8080/geoserver/wfs?service=wfs&version=2.0.0&request=GetFeature&outputFormat=application/json&typeNames=terrama2_11:view11&propertyName=ocorrencia,monitored_nome&sortBy=ocorrencia+D&startIndex=0&count=30";
    var urlEstados1d = "https://private-b1c17-terrama2monitor.apiary-mock.com/focosEstados1d";
    $.get(urlEstados1d, function(data, status){
      if (status == "success")
        createDataObjects(data, "tableEstados1d", "chartEstados1d", "bar", fields);
    });

    var urlEstados2d = "https://private-b1c17-terrama2monitor.apiary-mock.com/focosEstados2d";
    $.get(urlEstados2d, function(data, status){
      if (status == "success")
        createDataObjects(data, "tableEstados2d", "chartEstados2d", "horizontalBar", fields);
    });

    var urlEstados3d = "https://private-b1c17-terrama2monitor.apiary-mock.com/focosEstados3d";
    $.get(urlEstados3d, function(data, status){
      if (status == "success")
        createDataObjects(data, "tableEstados3d", "chartEstados3d", "horizontalBar", fields);
    });

  });
}());