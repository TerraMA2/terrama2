var DataManager = require("../DataManager");
class ChartLegendFacade {
  async list(chartId){
    const charts = await DataManager.orm.models.ChartLegends.findAll({"chart_id": chartId});
    return charts;
  }

  async saveAll(charts, chartId){
    try {
      charts.forEach(async chart=>{
        chart.chart_id = chartId;
        await DataManager.orm.models.ChartLegends.create(chart);
      });
    }catch(error){
      return false;
    }
  }
  async delete(chart){
    try{
      const deletedChart = await DataManager.orm.models.ChartLegends.delete(chart);
    }catch(error){
      return false;
    }
  }
}
module.exports = ChartFacade;