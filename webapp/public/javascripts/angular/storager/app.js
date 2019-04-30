define([
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/data-series/services",
  './components/storage',
  './components/storage-list',
  './services/storage-service'
], (coreModule, dataSeriesServiceModule, storageComponent, storageListComponent, storageService) => {
  const terrama2Module = angular.module("storage", [coreModule, dataSeriesServiceModule])
    .component("storageComponent", storageComponent)
    .component('storageListComponent', storageListComponent)
    .service('StorageService', storageService)

  return terrama2Module.name;
})