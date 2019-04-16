define([], () => {
  class StorageService {
    constructor(BaseService) {
      this.BaseService = BaseService;
    }

    async init(restriction = {}) {
      const { BaseService } = this;

      try {
        const response = await BaseService.$request(`${BASE_URL}api/storages`, 'GET', { params: restriction });

        this.model = response.data;
      } catch (err) {
        throw err;
      }

    }

    list(restriction) {
      return this.BaseService.$filter('filter')(this.model, restriction);
    }

    async _request(uri, method, options) {
      const { BaseService } = this;

      try {
        const response = await BaseService.$request(`${uri}`, method, options);

        return response.data;
      } catch (err) {
        throw new Error(err.message);
      }
    }

    async get(storageId) {
      const response = await this._request(`${BASE_URL}api/storages/${storageId}`, 'GET');

      return response;
    }

    async save(storageObj) {
      const response = await this._request(`${BASE_URL}api/storages`, 'POST', { data: storageObj });

      return response;
    }

    async update(storageId, storageObj) {
      const response = await this._request(`${BASE_URL}api/storages/${storageId}`, 'PUT', { data: storageObj });

      return response;
    }
  }

  StorageService.$inject = ['BaseService'];

  return StorageService;
});