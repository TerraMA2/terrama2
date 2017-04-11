define([], function() {
  'use strict';

  function AlertRegisterUpdate($timeout, AlertService, DataSeriesService, Service){
    var self = this;
    self.name = 'Alert 1';
    self.css = {
      boxType: "box-solid"
    };
    self.AlertService = AlertService;

    self.alert = {
      active: true,
      name: "Alerta teste",
      description: "Sem description",
      risk_attribute: "atributo",
      project_id: 1,
      data_series_id: 4,
      instance_id: 4,
      risk_attribute: "risco atributo",
      schedule: {
        data_ids: [1],
        scheduleType: 4
      },
      additionalData: [
        {
          dataseries_id: 1,
          data_set_id: 1,
          referrer_attribute: "attreferer",
          referred_attribute: "attrefered",
          data_attributes: "atributos"
        }
      ],
      reportMetadata: {
        title: "TituloReport",
        abstract: "Abstract",
        description: "Descricao",
        author: "Author",
        contact: "contato",
        copyrigth: "Copyright",
        timestamp_format: "formatoData",
        logo_path: "Caminho logo",
        document_format: "PDF"
      },
      risk: {
        name: "Risco",
        levels: [
          {
            name: "level1",
            level: 1,
            value: 100
          },
          {
            name: "level2",
            level: 2,
            value: 200
          }
        ]
      },
      notifications: [
        {
          include_report: true,
          notify_on_change: true,
          simplified_report: true,
          notify_on_risk_level: 1,
          recipients: "recipientes"
        }
      ]
    };
    self.isValid = true;
    self.save = saveOperation;

    function saveOperation(shouldRun) {
      $timeout(function(){
        var operation = self.AlertService.create(self.alert);
        operation.then(function(response){
          console.log(response);
        })
        .catch(function(err){
          console.log(err);
        });
      });
      
    }
  }

  AlertRegisterUpdate.$inject = ["$timeout", "AlertService", "DataSeriesService", "Service"];
  return AlertRegisterUpdate;
});
