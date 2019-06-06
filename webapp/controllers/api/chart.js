(function() {
  'use strict';

  // Dependencies
  var handleRequestError = require("../../core/Utils").handleRequestError;
  var TokenCode = require('../../core/Enums').TokenCode;
  var Utils = require('../../core/Utils');
  var DataManager = require('../../core/DataManager');
  var {Connection} = require('../../core/utility/connection');

  // Facade
  var ViewFacade = require("../../core/facade/View");

  /**
   * Injecting NodeJS App configuration as dependency. It retrieves a Views controllers API
   * 
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    return {
      get: async (request, response) => {
        const {
          viewId, attributeName, functionGrouping, groupBy, label, dateFrom, dateTo
        } = request.query

        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        const tableName = dataSeries.dataSets[0].format.table_name
        const timestampProperty = dataSeries.dataSets[0].format.timestamp_property
        const conn = new Connection(dataProvider.uri)
        await conn.connect()

        let sql = "";
        if(functionGrouping === "count"){
          sql = `SELECT ${groupBy}, COUNT(*) AS value FROM ${tableName} where ${timestampProperty} >= '${dateFrom}'::date AND ${timestampProperty} <= '${dateTo}'::date GROUP BY ${groupBy}`;
        } else if(functionGrouping === "sum"){
          sql = `SELECT ${groupBy}, SUM(${attributeName}) AS value FROM ${tableName} where ${timestampProperty} >= '${dateFrom}'::date AND ${timestampProperty} <= '${dateTo}'::date GROUP BY ${groupBy}`;
        }

        const result = await conn.execute(sql)
        await conn.disconnect()
        let rows = result.rows
        response.json(rows)
      },
    }
  };
} ());