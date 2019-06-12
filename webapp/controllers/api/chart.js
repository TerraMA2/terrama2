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
          viewId, attributeName, functionGrouping, groupBy, label, legendFromMap, dateFrom, dateTo
        } = request.query

        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        const tableName = dataSeries.dataSets[0].format.table_name
        const timestampProperty = dataSeries.dataSets[0].format.timestamp_property
        const conn = new Connection(dataProvider.uri)
        await conn.connect()
        let dateSQL = "";
        if(timestampProperty) {
          dateSQL = `where ${timestampProperty}::date >= '${dateFrom}' AND ${timestampProperty}::date <= '${dateTo}'`;
        }

        let select = `SELECT coalesce(${groupBy}::text, 'Undefined') ${groupBy}`;
        let groupBySql = groupBy;
        let join = "";
        if(legendFromMap === "true"){
          select += `,SUBSTRING(color, 1, 7) as color`;
          join+=`JOIN terrama2.view_style_colors as vsc ON ${groupBy} = vsc.value `;
          groupBySql+=`, color`;
        }

        let sql = "";
        if(functionGrouping === "count"){
          sql = `${select}, COUNT(*) AS value FROM ${tableName} ${join} ${dateSQL} GROUP BY ${groupBySql}`;
        } else if(functionGrouping === "sum"){
          sql = `${select}, SUM(${attributeName}) AS value FROM ${tableName} ${join} ${dateSQL} GROUP BY ${groupBySql}`;
        }

        const result = await conn.execute(sql)
        let rows = result.rows
        await conn.disconnect()
        response.json(rows)
      },
    }
  };
} ());